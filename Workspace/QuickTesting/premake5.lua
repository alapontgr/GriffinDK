project "QuickTesting"
	kind "WindowedApp"
	language "C++"

	links {
		"GfCore.lib",
		"GfEntry.lib",
		"GfRender.lib",
	}

	SetupGraphicsSDK();