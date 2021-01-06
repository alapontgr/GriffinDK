---------------------------------------------------------
-- Context vars
---------------------------------------------------------

ProjectDeps = {}
DepsToResolve = {}
TargetDirs = {}

---------------------------------------------------------

-- Helpers

function ExistsFile(file)
   local ok, err, code = os.rename(file, file)
   if not ok then
      if code == 13 then
         -- Permission denied, but it exists
         return true
      end
   end
   return ok, err
end


function CopyCMD(_From, _To)
   dirName = path.getdirectory(_From)
   fileName = path.getname(_From)
   return "(robocopy \"" .. dirName .. "\" \"" .. _To .. "\" " .. fileName .. ") ^& IF %ERRORLEVEL% LEQ 4 exit /B 0"
end

---------------------------------------------------------
-- General Utilities
---------------------------------------------------------

--- Check if a directory exists in this path
function IsDir(path)
   -- "/" works on both Unix and Windows
   return ExistsFile(path.."/")
end

-- Creates the directory "Dir" if it does not exist
function CreateIfNotExist(Dir)
   if not IsDir(Dir) then
      os.execute("mkdir " .. Dir)
   end
   
end

---------------------------------------------------------

function FilterPlatforms(AbsPath, Platforms)

   for kP, platform in pairs(Platforms) do
      filter {"platforms:" .. platform}
         for fkP, FilteredPlatform in pairs(Platforms) do
            if platform ~= FilteredPlatform then
               removefiles {AbsPath .. "/" .. FilteredPlatform .. "/**"}         
            end
         end
      filter {}
   end
end

---------------------------------------------------------

function SetupProjectSettings(_Project, _RelPath, _AbsPath)
   location(TargetDirs[_project])
   objdir(griffin.ProjVSFilesPath .. "/" .. _RelPath .. "/obj/")
   files { _AbsPath .. "/**.*"}
   FilterPlatforms(_AbsPath, griffin.Platforms)
end

---------------------------------------------------------

function GroupPostProcess(_Project, _RelPath, _AbsPath)
   project(_Project)
   if DepsToResolve[_Project] then
      commands = {}
      for index, projDep in ipairs(DepsToResolve[_Project]) do
         if ProjectDeps[projDep] then
            for index2, resDep in ipairs(ProjectDeps[projDep]) do
               dirName = path.getdirectory(resDep)
               fileName = path.getname(resDep)
               dstDir = TargetDirs[_Project]
               table.insert(commands, CopyCMD(resDep, "../" .. dstDir))
            end
         end
      end
      postbuildcommands (commands)
   end
end

---------------------------------------------------------

function SetupGroup(_Group, _GroupName, _AbsPath, _RelPath, Filter)
   local GroupFilter = Filter .. "/" .. _GroupName
   group(GroupFilter)
   -- Setup _projects
   if _Group.Projects then

      for _v, _project in pairs(_Group.Projects) do      
         local ProjRelPath = _RelPath .. "/" .. _project
         local ProjAbsPath = _AbsPath .. "/" .. ProjRelPath

         TargetDirs[_project] = griffin.OutPath
         include(ProjAbsPath)
         SetupProjectSettings(_project, ProjRelPath, ProjAbsPath)
      end
   end

   -- Setup sub-groups
   if _Group._groups then
      for _v, _g in pairs(_Group._groups) do
         local GroupAbsPath = _AbsPath .. _g.Name
         local GroupRelPath = _RelPath .. _g.Name
         SetupGroup(_g, _v, GroupAbsPath, GroupRelPath, GroupFilter)
      end
   end
end

---------------------------------------------------------

function DoGroupPostProcess(_Group, _GroupName, _AbsPath, _RelPath)
   if _Group.Projects then
      for _v, _project in pairs(_Group.Projects) do      
         local ProjRelPath = _RelPath .. "/" .. _project
         local ProjAbsPath = _AbsPath .. "/" .. ProjRelPath

         GroupPostProcess(_project, ProjRelPath, ProjAbsPath)
      end
   end

   -- Setup sub-groups
   if _Group._groups then
      for _v, _g in pairs(_Group._groups) do
         local GroupAbsPath = _AbsPath .. _g.Name
         local GroupRelPath = _RelPath .. _g.Name
         DoGroupPostProcess(_g, _v, GroupAbsPath, GroupRelPath)
      end
   end
end

---------------------------------------------------------
-- Premake Utilities
---------------------------------------------------------

-- Use this within the scope of a project to do the setup of the graphics SDK
-- Includes and links the current active (Premake) project with the correct graphics SDKs 
function SetupGraphicsSDK()
   -- Setup Vulkan SDK
   configuration "vulkan"
 
   includedirs {
        os.getenv("VK_SDK_PATH") .. "/Include",
   }
   libdirs { 
      os.getenv("VK_SDK_PATH") .. "/Lib",
   }
   links {"vulkan-1"}      
   defines 
   {
      "VK_PROTOTYPES",
      "VK_USE_PLATFORM_WIN32_KHR",
   }

   filter {}

end

function GfRegisterExtDep(_Project, _Dep)
   if not ProjectDeps[_Project] then
      ProjectDeps[_Project] = {}
   end
   table.insert(ProjectDeps[_Project], path.getabsolute(_Dep))
end

-- Files registered in _DepProject with GfRegisterExtDep will be copied to the _Project output directoy
function GfResolveDep(_Project, _DepProject)
   if not DepsToResolve[_Project] then
      DepsToResolve[_Project] = {}
   end
   table.insert(DepsToResolve[_Project], _DepProject)
end