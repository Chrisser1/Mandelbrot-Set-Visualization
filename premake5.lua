-- premake5.lua
workspace "Mandelbrot-Set"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "Mandelbrot-Set"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
include "Walnut/WalnutExternal.lua"

include "Mandelbrot-Set"