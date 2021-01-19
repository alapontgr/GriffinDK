project "QuickTesting"
	kind "ConsoleApp"
	language "C++"

	links {
		"Griffin",
	}

	location(ProjectFilesDir)
	targetdir(griffin.OutPath .. "/QuickTesting")
	objdir(griffin.ObjPath .. "/QuickTesting/obj/")

	-------------------
	-- Add include dirs
	includedirs { "./", griffin.GriffinSrcDir, griffin.ExternalSrcDir }

	-------------------
	-- Add lib dirs
	libdirs { griffin.GriffinLibDir }

	-------------------
	-- Add files
	files {"./src/**.*"}

	files {griffin.ExternalSrcDir .. "/tinygltf/**.*"}

	SetupGraphicsSDK()