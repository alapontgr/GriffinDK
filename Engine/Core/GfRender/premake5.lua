project "GfRender"
	kind "StaticLib"
	language "C++"

	links {
		"GfCore",
		"GfInput",
	}

	SetupGraphicsSDK();