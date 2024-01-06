#pragma once

#include "Walnut/Image.h"
#include <complex>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/color_space.hpp> 
#include <unordered_map>

class Renderer
{
public:
    int maxIterations = 80;

    // Add zoom functionality
    float zoomFactor = 0.300;
    float centerX = -0.600;
    float centerY = 0.0;

    // Plot window
    double RE_START = -2.0;
    double RE_END = 1.0;
    double IM_START = -1.0;
    double IM_END = 1.0;
public:
    // Constructors
    Renderer() = default;

    // Main functions
    void OnResize(uint32_t width, uint32_t height);
    void Render();

    // Getters and Setters
    std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }

private:

    // Main functions
    void ComputeMandelbrotSetAndHistogram(uint32_t x, uint32_t y, uint32_t imageWidth, uint32_t imageHeight);
    std::vector<float> ComputeHues();
    glm::vec4 ComputeColor(int x, int y, const std::vector<float>& hues, uint32_t imageWidth);
    int Mandelbrot(const std::complex<double>& c);

    std::unordered_map<int, int> histogram;
    std::unordered_map<int, int> values;

    std::shared_ptr<Walnut::Image> m_FinalImage;
    std::vector<uint32_t> m_ImageHorizontalIter, m_ImageVerticalIter;
    uint32_t* m_ImageData = nullptr;
};
