
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

   	configurations { "Debug", "DebugOpt", "Release", "ReleaseFinal" } 
  	platforms { griffin.Platforms }

	targetdir(griffin.OutPath)
	libdirs { griffin.OutPath }
   	location("Solutions/")
   	debugdir( griffin.WorkingDirPath )

   	-- TODO: Refactor this to depend on the platform that is chosen
   	architecture "x86_64"
   	entrypoint  ("WinMainCRTStartup")
   	---------------------------------------------------------------

   	defines { "_PLATFORM_DIR=%{cfg.platform}" }
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
	includedirs { GroupConfig.Path .. "/" .. GroupName}
end

-- Projects main pass
for GroupName, GroupConfig in pairs(groups) do
	group(GroupName)

	if GroupConfig.Projects then
		for k, p in pairs(GroupConfig.Projects) do
			--print(p)
			local RelPath = GroupName .. "/" .. p
			local AbsPath = GroupConfig.Path .. "/" .. RelPath
			
			include(AbsPath)			
			SetupProjectSettings(p, RelPath, AbsPath)

			----------------------------------------------------------

		end
	end

	-- Sub groups
	if GroupConfig._groups then
		for _v, _g in pairs(GroupConfig._groups) do
			SetupGroup(_g, _v, GroupConfig.Path, GroupName, GroupName)
		end
	end
end

-- Projects post-processing
for GroupName, GroupConfig in pairs(groups) do
	if GroupConfig.Projects then
		for k, p in pairs(GroupConfig.Projects) do
			--print(p)
			local RelPath = GroupName .. "/" .. p
			local AbsPath = GroupConfig.Path .. "/" .. RelPath		
			GroupPostProcess(p, RelPath, AbsPath)
		end
	end

	-- Sub groups
	if GroupConfig._groups then
		for _v, _g in pairs(GroupConfig._groups) do
			DoGroupPostProcess(_g, _v, GroupConfig.Path, GroupName)
		end
	end
end

----------------------------------------------------------
