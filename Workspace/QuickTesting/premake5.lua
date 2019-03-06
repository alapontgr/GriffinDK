project "QuickTesting"
	kind "WindowedApp"
	language "C++"

	links {
		"GfCore",
		"GfFile",
		"GfEntry",
		"GfRender",
	}

	SetupGraphicsSDK();