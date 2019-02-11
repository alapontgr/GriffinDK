project "QuickTesting"
	kind "WindowedApp"
	language "C++"

	links {
		"GfCore.lib",
		"GfFile.lib",
		"GfEntry.lib",
		"GfRender.lib",
	}

	SetupGraphicsSDK();