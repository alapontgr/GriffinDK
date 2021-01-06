----------------------------------------------------------
-- Griffin Engine
----------------------------------------------------------

griffin = {}
griffin.EngineName = "GriffinEngine"
griffin.Platforms = { "Win64" }
griffin.GfxApis = { "Vulkan" }
griffin.EngineBasePath = "Engine"
griffin.OutDir = "bin"
griffin.VSFilesDir = "build"
griffin.WorkingDir = "data"
griffin.VSOBJFilesDir = griffin.VSFilesDir .. "/" .. "obj"
griffin.OutPath = griffin.OutDir .. "/" .. "%{prj.platform}" .. "/" .. "%{prj.buildcfg}"
griffin.ProjVSFilesPath = griffin.VSFilesDir .. "/" .. "%{prj.platform}" .. "/" .. "%{prj.buildcfg}"
griffin.ProjVSOBJFilesPath = griffin.VSOBJFilesDir .. "/" .. "%{prj.platform}" .. "/" .. "%{prj.buildcfg}"
griffin.WorkingDirPath = griffin.WorkingDir .. "/" .. "%{prj.platform}" .. "/"
griffin.GriffinSrcDir = griffin.EngineBasePath .. "/Src"

----------------------------------------------------------
-- Solution layout
----------------------------------------------------------

groups = {}

-- Group definition example:
--groups["TestGroup"] = 
--{
--	--Path = griffin.EngineBasePath,
--	Path = "./",
--	Projects = 
--	{
--		"TestProject1"
--	},
--	_groups = 
--	{
--		{
--			Name = "TestChild",
--			Projects = 
--			{
--				"TestProject1"
--			},
--		}
--	},
--}

groups["External"] = 
{
	Path = griffin.EngineBasePath,
	Projects = 
	{
		"glm",
		"vma",
		"stb",
		"dxc",
	}
}

groups["Tools"] = 
{
	Path = griffin.EngineBasePath,
	_groups =
	{
		Libraries = 
		{
			Projects = 
			{
				"GfMaterialCompiler",
			},
		}
	}
}

groups["Workspace"] = 
{
	--Path = griffin.EngineBasePath,
	Path = "./",
	Projects = 
	{
		"QuickTesting",
		"RTExperiments"
	},
}

----------------------------------------------------------
-- Build the solution
----------------------------------------------------------

dofile("./scripts/utils.lua")
dofile("./engine_setup.lua")