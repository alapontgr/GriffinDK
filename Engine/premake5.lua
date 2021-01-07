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

location(ProjectFilesDir)
targetdir(griffin.OutPath)
objdir(griffin.ObjPath .. "/Griffin")
-------------------
-- Add include dirs
includedirs { "./Src", "./External" }

-------------------
-- Add files

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

SetupGraphicsSDK()