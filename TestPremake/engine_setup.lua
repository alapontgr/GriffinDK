
----------------------------------------------------------
-- Create aux directories

CreateIfNotExist(config.OutDir)
CreateIfNotExist(config.VSFilesDir)
CreateIfNotExist(config.VSOBJFilesDir)

----------------------------------------------------------
-- Setup the solution and configure the shared settings --
----------------------------------------------------------

workspace (config.EngineName)

   	configurations { "Debug", "DebugOpt", "Release", "ReleaseFinal" } 
  	platforms { config.Platforms }

	targetdir(config.OutPath)
	libdirs { config.OutPath }
   	location("Solutions/")

   	defines { "_PLATFORM_DIR=%{cfg.platform}" }

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

	filter { "configurations:Release" }
		defines { "_RELEASE_FINAL" }
		symbols "Off"
		optimize "Full"

	filter {}

----------------------------------------------------------
-- Build the solution
----------------------------------------------------------

-- Include the directories of the groups as additional include directory
for GroupName, GroupConfig in pairs(groups) do
	includedirs { config.EngineBasePath .. "/" .. GroupName}
end

-- Create the projects
for GroupName, GroupConfig in pairs(groups) do
	group(GroupName)
	for k, p in pairs(GroupConfig.Projects) do
		print(p)
		local RelPath = GroupName .. "/" .. p
		local AbsPath = config.EngineBasePath .. "/" .. RelPath
		
		include(AbsPath)

		-------------------------------------
		---------------------
		-- The scope of the project is still active

		location(config.ProjVSFilesPath .. "/" .. RelPath)
		objdir(config.ProjVSFilesPath .. "/" .. RelPath .. "/obj/")
		files { AbsPath .. "/**.*"}

		FilterPlatforms(AbsPath, config.Platforms)

		----------------------------------------------------------

	end
end

----------------------------------------------------------
