-- premake5.lua
workspace "Mandelbrot-Set-Visualization"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "Mandelbrot-App"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

group "Core"
	include "Mandelbrot-Core/Build-Core.lua"
group ""

include "Mandelbrot-App/Build-App.lua"