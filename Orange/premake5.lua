workspace "Orange"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Distribution"
	}

outputdir = "%{cfg.buildcfg}/%{cfg.system}/%{cfg.architecture}"

project "Orange"
	location "Generated"
	kind "WindowedApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin/intermediate/" .. outputdir .. "/%{prj.name}")

	files
	{
		"./Source/**.h",
		"./Source/**.cpp"
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

	filter "system:Windows"
		cppdialect "C++20"
		systemversion "latest"

		defines
		{
			"OG_WINDOWS"
		}

	filter "configurations:Debug"
		defines "OG_DEBUG"
		symbols "On"
		
	filter "configurations:Release"
		defines "OG_RELEASE"
		optimize "On"

	filter "configurations:Distribution"
		defines "OG_DISTRIBUTION"
		optimize "On"