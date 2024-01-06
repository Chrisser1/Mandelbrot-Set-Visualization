#include "Renderer.h"
#include <execution>

namespace Utils {

	static uint32_t ConvertToRGBA(const glm::vec4& color)
	{
		uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);

		uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
		return result;
	}

	static glm::vec3 ConvertHSVtoRGB(float hue, float saturation, float value) {
		float C = value * saturation;
		float X = C * (1 - std::abs(std::fmod(hue * 6, 2) - 1));
		float m = value - C;

		glm::vec3 colorRGB;
		if (0 <= hue && hue < 1.0f / 6.0f) {
			colorRGB = glm::vec3(C, X, 0);
		}
		else if (1.0f / 6.0f <= hue && hue < 2.0f / 6.0f) {
			colorRGB = glm::vec3(X, C, 0);
		}
		else if (2.0f / 6.0f <= hue && hue < 3.0f / 6.0f) {
			colorRGB = glm::vec3(0, C, X);
		}
		else if (3.0f / 6.0f <= hue && hue < 4.0f / 6.0f) {
			colorRGB = glm::vec3(0, X, C);
		}
		else if (4.0f / 6.0f <= hue && hue < 5.0f / 6.0f) {
			colorRGB = glm::vec3(X, 0, C);
		}
		else {
			colorRGB = glm::vec3(C, 0, X);
		}

		return colorRGB + m;
	}

	float lerp(float a, float b, float t) {
		return a + t * (b - a);
	}
}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	// Calculate the real and imaginary range using zoom and center parameters
	double rangeX = (1.0 - (-2.0)) + zoomFactor;
	double rangeY = (1.0 - (-1.0)) + zoomFactor;

	RE_START = centerX - rangeX / 2.0;
	RE_END = centerX + rangeX / 2.0;
	IM_START = centerY - rangeY / 2.0;
	IM_END = centerY + rangeY / 2.0;

	if (m_FinalImage)
	{
		// No resize necessary
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
			return;

		m_FinalImage->Resize(width, height);
	}
	else
	{
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];

	m_ImageHorizontalIter.resize(width);
	m_ImageVerticalIter.resize(height);
	for (uint32_t i = 0; i < width; i++)
		m_ImageHorizontalIter[i] = i;
	for (uint32_t i = 0; i < height; i++)
		m_ImageVerticalIter[i] = i;
}

void Renderer::Render()
{
	const uint32_t imageWidth = m_FinalImage->GetWidth();
	const uint32_t imageHeight = m_FinalImage->GetHeight();

	for (uint32_t y : m_ImageVerticalIter) {
		for (uint32_t x : m_ImageHorizontalIter) {
			ComputeMandelbrotSetAndHistogram(x, y, imageWidth, imageHeight);
			std::vector<float> hues = ComputeHues();
			glm::vec4 color = ComputeColor(x, y, hues, imageWidth);
			m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);
		}
	}

	m_FinalImage->SetData(m_ImageData);
}

void Renderer::ComputeMandelbrotSetAndHistogram(uint32_t x, uint32_t y, uint32_t imageWidth, uint32_t imageHeight)
{
	// Map pixel coordinates to complex plane coordinates
	double real = RE_START + (static_cast<double>(x) / imageWidth) * (RE_END - RE_START);
	double imag = IM_START + (static_cast<double>(y) / imageHeight) * (IM_END - IM_START);

	// Initialize complex number for Mandelbrot calculation
	auto c = std::complex<double>(real, imag);

	// Compute Mandelbrot set iteration using the provided function
	int iteration = Mandelbrot(c);
	
	values[y + m_FinalImage->GetWidth() * x] = iteration;

	if (iteration < maxIterations)
	{
		histogram[floor(iteration)]++;
	}
}

std::vector<float> Renderer::ComputeHues() {
	std::vector<float> hues;
	std::vector<int> histogramValues(maxIterations, 0);

	// Aggregate histogram values directly into the histogramValues vector
	for (const auto& pair : histogram) {
		if (pair.first < maxIterations) {
			histogramValues[pair.first] = pair.second;
		}
	}

	int total = std::accumulate(histogramValues.begin(), histogramValues.end(), 0);

	float h = 0.0f;
	for (int i = 0; i < maxIterations; ++i) {
		h += static_cast<float>(histogramValues[i]) / static_cast<float>(total);
		hues.push_back(h);
	}

	hues.push_back(1.0f); // Appending the last value

	return hues;
}

glm::vec4 Renderer::ComputeColor(int x, int y, const std::vector<float>& hues, uint32_t imageWidth)
{
	float m = values[y + imageWidth * x];
	float hue = 1.0f - (Utils::lerp(hues[static_cast<int>(floor(m))], hues[static_cast<int>(ceil(m))], m - floor(m)));
	float saturation = 1.0f; // Maximum saturation
	float value = (m < maxIterations) ? 1.0f : 0.0f; // Maximum value for colors inside the set

	 glm::vec3 color = Utils::ConvertHSVtoRGB(hue, saturation, value);
	 
	return glm::vec4(color, 1.0f);
}

int Renderer::Mandelbrot(const std::complex<double>& c) {
	std::complex<double> z = 0;
	int n = 0;
	double threshold = 4.0; // squared distance threshold (2^2)

	while (std::norm(z) <= threshold && n < maxIterations) {
		z = z * z + c;
		n++;
	}

	if (n == maxIterations) {
		return maxIterations;
	}

	return n + 1 - log(log2(std::abs(z)));
}