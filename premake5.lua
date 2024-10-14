-- premake5.lua
workspace "BasicRayTracer"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "BasicRayTracer"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
include "Walnut/WalnutExternal.lua"

include "BasicRayTracer"