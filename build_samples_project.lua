solution "TestingFenix" 
    location "./Samples"
    configurations {
        "Debug",
        "Release",
    }
 
    platforms {
        "x32",
        "x64",
    }

    language "C++"

project "UnitTests"
    location "./Samples/UnitTests"
    kind "ConsoleApp"

    targetdir "./Samples/UnitTests/build/$(Configuration)/$(Platform)"
    objdir "./Samples/UnitTests/obj/$(Platform)"
    debugdir "./Samples/Res"
    prebuildcommands { "call ../../scripts/compile_engine.bat $(Platform) $(Configuration) ../../FenixDK/FenixEngine.sln" }

    includedirs {
        os.getenv("VK_SDK_PATH") .. "/Include",
        "FenixDK/src",
        "External/",
        "./Samples/UnitTests/src" 
    }
    files {
        "./Samples/UnitTests/src/**"     
    }
    libdirs 
    { 
        "./FenixDK/build/$(Configuration)/$(Platform)",
    }
    links {"FenixDK"}

    configuration "Release"
        defines { "_NDEBUG" }
        flags { "OptimizeSpeed" }

    configuration "Debug" 
        defines { "_DEBUG" }
        flags {"Symbols"}


    configuration "x32"
        libdirs { 
            os.getenv("VK_SDK_PATH") .. "/Lib32",
        }
        links {"vulkan-1"}
        defines 
        {
            "Win32", 
            "VK_PROTOTYPES",
            "VK_USE_PLATFORM_WIN32_KHR",
        }

    configuration "x64"
        libdirs { 
            os.getenv("VK_SDK_PATH") .. "/Lib",
        }
        links {"vulkan-1"}      
        defines 
        {
            "x64", 
            "VK_PROTOTYPES",
            "VK_USE_PLATFORM_WIN32_KHR",
        }


project "Basic"
    location "./Samples/Basic"
    --kind "ConsoleApp"
    --In case of using WindowedApp we have to specify te entry point
    kind "WindowedApp"
    flags { "WinMain" }

    targetdir "./Samples/Basic/build/$(Configuration)/$(Platform)"
    objdir "./Samples/Basic/obj/$(Platform)"
    debugdir "./Samples/Res"
    prebuildcommands { "call ../../scripts/compile_engine.bat $(Platform) $(Configuration) ../../FenixDK/FenixEngine.sln" }

    includedirs {
        os.getenv("VK_SDK_PATH") ..  "/Include",
        "FenixDK/src",
        "External/",
        "./Samples/Basic/src" 
    }
    files {
        "./Samples/Basic/src/**"     
    }
    libdirs 
    { 
        "./FenixDK/build/$(Configuration)/$(Platform)",
    }
    links {"FenixDK"}

    configuration "Release"
        defines { "_NDEBUG" }
        flags { "OptimizeSpeed" }

    configuration "Debug" 
        defines { "_DEBUG" }
        flags {"Symbols"}


    configuration "x32"
        libdirs { 
            os.getenv("VK_SDK_PATH") .. "/Lib32",
        }
        links {"vulkan-1"}
        defines 
        {
            "Win32", 
            "VK_PROTOTYPES",
            "VK_USE_PLATFORM_WIN32_KHR",
        }

    configuration "x64"
        libdirs { 
            os.getenv("VK_SDK_PATH") .. "/Lib",
        }
        links {"vulkan-1"}      
        defines 
        {
            "x64", 
            "VK_PROTOTYPES",
            "VK_USE_PLATFORM_WIN32_KHR",
        }