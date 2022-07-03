workspace "Orange"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Distribution"
	}

	startproject "Orange"

outputdir = "%{cfg.buildcfg}/%{cfg.system}/%{cfg.architecture}"

project "Orange"
	location "Generated"
	kind "WindowedApp"
	language "C++"
	cppdialect "C++20"
	warnings "Extra"

	targetdir ("Generated/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("Generated/bin/intermediate/" .. outputdir .. "/%{prj.name}")

	files
	{
		"./Source/**.h",
		"./Source/**.cpp",
		"./Source/Shaders/**.hlsl"
	}

	includedirs
	{
		"./ThirdParty/FreeType/inc",
		"./ThirdParty/FreeType/src",
		"./ThirdParty/DirectX11/inc",
		"./ThirdParty/DirectX11/src/%{cfg.architecture}/%{cfg.buildcfg}"
	}

	links
	{
		"./ThirdParty/FreeType/src/freetype.lib",
		"./ThirdParty/DirectX11/src/%{cfg.architecture}/%{cfg.buildcfg}/DirectXTK.lib"
	}

	-- HLSL shader filters --
	filter "files:**.hlsl"
		shadermodel "5.0"
	
	filter "files:**_VS.hlsl"
		shadertype "Vertex"
	
	filter "files:**_GS.hlsl"
		shadertype "Geometry"
	
	filter "files:**_PS.hlsl"
		shadertype "Pixel"

	-- System filters --
	filter "system:Windows"
		systemversion "latest"

		defines
		{
			"OG_WINDOWS"
		}

		postbuildcommands
		{
			("{COPY} ../ThirdParty/FreeType/freetype.dll ./bin/" .. outputdir .. "/%{prj.name}")
		}

	-- Configuration filters --
	filter "configurations:Debug"
		defines "OG_DEBUG"
		symbols "On"
		
	filter "configurations:Release"
		defines "OG_RELEASE"
		optimize "On"

	filter "configurations:Distribution"
		defines "OG_DISTRIBUTION"
		optimize "On"