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
-- Premake Utilities
---------------------------------------------------------

-- Use this within the scope of a project to do the setup of the graphics SDK
-- Includes and links the current active (Premake) project with the correct graphics SDKs 
function SetupGraphicsSDK()

   -- Setup Vulkan SDK
   filter {"platforms:x64(Vulkan)"}

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

   -- Setup Dx12 SDK
   filter {"platforms:x64(Dx12)"}

   filter {}

end