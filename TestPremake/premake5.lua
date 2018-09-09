----------------------------------------------------------
-- Griffin Engine
----------------------------------------------------------

config = {}
config.EngineName = "GriffinEngine"
config.Platforms = { "x64(Vulkan)", "x64(Dx12)" }
config.EngineBasePath = "Engine"
config.OutDir = "bin"
config.VSFilesDir = "build"
config.VSOBJFilesDir = config.VSFilesDir .. "/" .. "obj"
config.OutPath = config.OutDir .. "/" .. "%{cfg.platform}" .. "/" .. "%{cfg.buildcfg}"
config.ProjVSFilesPath = config.VSFilesDir .. "/" .. "%{prj.platform}" .. "/" .. "%{prj.buildcfg}"
config.ProjVSOBJFilesPath = config.VSOBJFilesDir .. "/" .. "%{prj.platform}" .. "/" .. "%{prj.buildcfg}"

----------------------------------------------------------
-- Solution layout
----------------------------------------------------------

groups = {}

-- Group definition example:
-- groups["Core"] = 
-- {
--	Projects = 
--	{
--		"GfCore"
--	}
--
--}
-- The example above creates a project GfCore within the group Core. At creation the script will execute the file "premake5.lua" used to describe the project

groups["External"] = 
{
	Projects = 
	{
		"glm"
	}
}

groups["Core"] = 
{
	Projects = 
	{
		"GfCore",
		"GfMaths"
	}

}

groups["Testing"] = 
{
	Projects = 
	{
		"QuickTesting"
	}

}

----------------------------------------------------------
-- Build the solution
----------------------------------------------------------

dofile("./scripts/utils.lua")
dofile("./engine_setup.lua")