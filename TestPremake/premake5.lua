----------------------------------------------------------
-- Griffin Engine
----------------------------------------------------------

griffin = {}
griffin.EngineName = "GriffinEngine"
griffin.Platforms = { "x64(Dx12)", "x64(Vulkan)" }
griffin.EngineBasePath = "Engine"
griffin.OutDir = "bin"
griffin.VSFilesDir = "build"
griffin.WorkingDir = "data"
griffin.VSOBJFilesDir = griffin.VSFilesDir .. "/" .. "obj"
griffin.OutPath = griffin.OutDir .. "/" .. "%{cfg.platform}" .. "/" .. "%{cfg.buildcfg}"
griffin.ProjVSFilesPath = griffin.VSFilesDir .. "/" .. "%{prj.platform}" .. "/" .. "%{prj.buildcfg}"
griffin.ProjVSOBJFilesPath = griffin.VSOBJFilesDir .. "/" .. "%{prj.platform}" .. "/" .. "%{prj.buildcfg}"
griffin.WorkingDirPath = griffin.WorkingDir .. "/" .. "%{prj.platform}" .. "/"

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
		"GfEntry",
		"GfRender",
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