----------------------------------------------------------
-- Griffin Engine
----------------------------------------------------------

griffin = {}
griffin.EngineName = "GriffinEngine"
griffin.Platforms = { "Win64" }
griffin.GfxApis = { "Vulkan" }

griffin.EngineBasePath = os.realpath("./Engine")

griffin.OutDir = os.realpath("./_bin")
griffin.ObjDir = os.realpath("./_obj")
griffin.WorkingDir = os.realpath("./data")
griffin.GriffinLibDir = griffin.OutDir .. "/Griffin"
griffin.GriffinSrcDir = griffin.EngineBasePath .. "/Src"
griffin.ExternalSrcDir = griffin.EngineBasePath .. "/External"
griffin.ProjectFilesDir = os.realpath("./_vs")

griffin.WorkingDirPath = griffin.WorkingDir .. "/" .. "%{prj.platform}" .. "/"
griffin.OutPath = griffin.OutDir .. "/" .. "%{prj.platform}" .. "/" .. "%{prj.buildcfg}"
griffin.ObjPath = griffin.ObjDir .. "/" .. "%{prj.platform}" .. "/" .. "%{prj.buildcfg}"

----------------------------------------------------------

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

-- Use as --add_tests={0,1}
newoption {
	trigger     = "add_tests",
	value       = "{0,1}",
	description = "Include the tests in the solution",
	default     = "1",
	allowed = {
		{ "1",    "Enabled" },
		{ "0",    "Disabled" }
	}
}

----------------------------------------------------------
-- Build the solution
----------------------------------------------------------

-- Add utilities
dofile("./scripts/utils.lua")

----------------------------------------------------------
-- Setup the solution and configure the shared settings --
----------------------------------------------------------

workspace (griffin.EngineName)

   	configurations { "Debug", "DebugOpt", "Release"} 
  	platforms { griffin.Platforms }

	targetdir(griffin.OutPath)
	libdirs { griffin.OutPath }
   	location( griffin.ProjectFilesDir)
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

----------------------------------------------------------
-- Build the solution
----------------------------------------------------------

-- Create Engine projects
include("./Engine")

if _OPTIONS["add_tests"] == "1" then
	include("./Workspace")
end
----------------------------------------------------------
