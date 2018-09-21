project "QuickTesting"
	kind "WindowedApp"
	language "C++"

	links {
		"GfCore",
		"GfEntry",
		"GfRender",
	}

	SetupGraphicsSDK();