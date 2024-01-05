project "Mandelbrot-Core"
   kind "StaticLib"
   language "C++"
   cppdialect "C++20"
   targetdir "Binaries/%{cfg.buildcfg}"
   staticruntime "off"

   files { "Source/**.h", "Source/**.cpp" }

   -- Add include directories for Vulkan, GLFW, and GLM
   includedirs
   {
       "Source",
       "C:/VulkanSDK/1.3.250.1/Include", -- Vulkan include directory
       "../Mandelbrot-Core/Source/Libraries/glfw-3.3.9.bin.WIN64/include", -- GLFW include directory
       "../Mandelbrot-Core/Source/Libraries/glm", -- GLM include directory
   }

   -- Add library directories for Vulkan and GLFW
   libdirs
   {
       "C:/VulkanSDK/1.3.250.1/Lib", -- Vulkan library directory
       "../Mandelbrot-Core/Source/Libraries/glfw-3.3.9.bin.WIN64/lib-vc2019", -- GLFW library directory (adjust as needed)
   }

   -- Add dependencies for Vulkan and GLFW
   links
   {
       "vulkan-1", -- Vulkan library name (adjust as needed)
       "glfw3",    -- GLFW library name (adjust as needed)
   }

   targetdir ("../Binaries/" .. OutputDir .. "/%{prj.name}")
   objdir ("../Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")

   filter "system:windows"
       systemversion "latest"
       defines { }

   filter "configurations:Debug"
       defines { "DEBUG" }
       runtime "Debug"
       symbols "On"

   filter "configurations:Release"
       defines { "RELEASE" }
       runtime "Release"
       optimize "On"
       symbols "On"

   filter "configurations:Dist"
       defines { "DIST" }
       runtime "Release"
       optimize "On"
       symbols "Off"