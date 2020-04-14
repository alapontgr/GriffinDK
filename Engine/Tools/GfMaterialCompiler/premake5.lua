project "GfMaterialCompiler"
	kind "StaticLib"
	language "C++"
	
	kind "None"
	filter {"architecture:x86_64"}
    	kind "StaticLib"
    
    	libdirs { griffin.EngineBasePath .. "/" .. "External/dxc/lib" }
    	links { "dxcompiler.dll" }

    filter {}