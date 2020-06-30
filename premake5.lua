workspace "Traum"
	configurations {"Debug", "Release"}
	location "build"
	filter "language:C"
		toolset "gcc"
		buildoptions {"-std=c11 -pedantic -Wall"}
	filter "system:windows"
		links {"mingw32"}
	filter "configurations:Debug"
		defines {"DEBUG"}
		symbols "On"
	filter "configurations:Test"
		defines {"DEBUG", "TEST"}
		symbols "On"
	filter "configurations:Release"
		defines {"NDEBUG"}
		vectorextensions "Default"
		optimize "Speed"
	project "maya"
		language "C"
		kind "StaticLib"
		includedirs {"public"}
		sysincludedirs {"headers"}
		postbuildcommands {"cd .. && python copy.py maya"}
		files {
			"maya/**.h",
			"maya/**.c",
		}
		filter {}
	project "kroptr"
		language "C"
		kind "StaticLib"
		includedirs {"kroptr/public"}
		sysincludedirs {"headers"}
		postbuildcommands {"cd .. && python copy.py kroptr"}
		files {
			"kroptr/**.h",
			"kroptr/**.c",
		}
		filter {}
	project "system"
		language "C"
		kind "StaticLib"
		includedirs {"public"}
		sysincludedirs {"headers"}
		postbuildcommands {"cd .. && python copy.py system"}
		files {
			"system/**.h",
			"system/**.c",
		}
		--Only used on the dreamcast
		removefiles "system/kos.c"
		filter {}
		links {"maya"}
	project "engine"
		language "C"
		kind "StaticLib"
		includedirs {"public"}
		sysincludedirs {"headers"}
		postbuildcommands {"cd .. && python copy.py engine"}
		files {
			"engine/**.h",
			"engine/**.c",
		}
		filter {}
		links {"system", "maya"}
	project "traum"
		language "C"
		kind "ConsoleApp"
		includedirs {"public"}
		sysincludedirs {"headers"}
		postbuildcommands {"cd .. && python copy.py traum"}
		files {
			"traum/**.h",
			"traum/**.c",
		}
		filter {}
		links {"sdl2main", "sdl2", "engine", "system", "maya"}
	project "punk"
		language "C"
		kind "ConsoleApp"
		includedirs {"public"}
		sysincludedirs {"headers"}
		postbuildcommands {"cd .. && python copy.py punk"}
		files {
			"punk/**.h",
			"punk/**.c",
		}
		filter {}
		links {"sdl2main", "sdl2", "engine", "system", "maya"}
