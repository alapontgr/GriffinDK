project "QuickTesting"
	kind "WindowedApp"
	language "C++"

	links {
		"GfCore",
		"GfInput",
		"GfFile",
		"GfEntry",
		"GfRender",
	}

	SetupGraphicsSDK();