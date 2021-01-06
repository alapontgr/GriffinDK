project "Griffin"

GriffinDirs = 
	{
		"GfCore",
		"GfMemory",
		"GfEntry",
		"GfRender",
		"GfInput",
		"GfFile",
	}

ExternalProjects = 
	{
		"glm",
		"vma",
		"stb",
	}

kind "StaticLib"
language "C++"

location(griffin.OutPath)
objdir(griffin.ProjVSFilesPath .. "/Griffin/obj/")

-------------------
-- Add include dirs
includedirs { "./Src", "./External" }

-------------------
-- Add files TODO

fileDirs = {"./Src/Common/**.*"}

-- External files
for k, Name in pairs(ExternalProjects) do
	table.insert(fileDirs, "./External/" .. Name .. "/**.*")
end
-- Add per-platform files
table.insert(fileDirs, "./Src/Win64/**.*")

-- Add per Gfx API
table.insert(fileDirs, "./Src/Vulkan/**.*")

files {fileDirs}

-----------------------
-- Setup Gfx libraries

if _OPTIONS["gfxapi"] == "vulkan" then

	defines { "_GFX_API=Vulkan" }
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

end