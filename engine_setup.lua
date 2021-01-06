
----------------------------------------------------------
-- Create aux directories

CreateIfNotExist(griffin.OutDir)
CreateIfNotExist(griffin.VSFilesDir)
CreateIfNotExist(griffin.VSOBJFilesDir)
CreateIfNotExist(griffin.WorkingDir)

----------------------------------------------------------
-- Setup the solution and configure the shared settings --
----------------------------------------------------------

workspace (griffin.EngineName)

   	configurations { "Debug", "DebugOpt", "Release"} 
  	platforms { griffin.Platforms }

	targetdir(griffin.OutPath)
	libdirs { griffin.OutPath }
   	location("Solutions/")
   	debugdir( griffin.WorkingDirPath )

	---------------------------------------------------------------   
	-- TODO: Refactor this to depend on the platform that is chosen
   	architecture "x64"
	entrypoint  ("WinMainCRTStartup")
	defines { "_PLATFORM_DIR=Win64" }
	system "Windows"
   	---------------------------------------------------------------

   	flags { "FatalWarnings" }

	filter { "configurations:Debug" }
		defines { "_DEBUG" }
		symbols "On"
		optimize "Off"
	
    filter { "configurations:DebugOpt" }
		defines { "_DEBUGOPT" }
		symbols "On"
		optimize "On"

	filter { "configurations:Release" }
		defines { "_RELEASE" }
		symbols "Off"
		optimize "Full"

	filter {}

	-- Use as --gfxapi=<API>
	newoption {
		trigger     = "gfxapi",
		value       = "<API>",
		description = "Choose a particular 3D API for rendering",
		default     = "vulkan",
		allowed = {
			{ "vulkan",    "Vulkan" },
			{ "dx12",    "Dx12" }
		}
	}

----------------------------------------------------------
-- Build the solution
----------------------------------------------------------

-- Create Engine projects
include("./Engine")
----------------------------------------------------------
