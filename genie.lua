solution "FenixEngine"
    location "./FenixDK"
    configurations {
        "Debug",
        "Release",
    }
 
    platforms {
        "x32",
        "x64",
    }

    language "C++"

--Engine project that will be used as a static library
project "FenixDK"
    location "./FenixDK"
    --kind "ConsoleApp"
    --In case of using WindowedApp we have to specify te entry point
    kind "StaticLib"
    flags { "WinMain" }

    targetdir "./FenixDK/build/$(Configuration)/$(Platform)"
    objdir "./FenixDK/obj/$(Platform)"
    debugdir "./Res"

    includedirs {
        "C:/VulkanSDK/1.0.39.1/Include",
        "FenixDK/src",
        "External/",
    }
    files {
        "FenixDK/src/**",
        "./External/**",     
    }

    configuration "Release"
        defines { "_NDEBUG" }
        flags { "OptimizeSpeed" }

    configuration "Debug" 
        debugdir "./Res"
        defines { "_DEBUG" }
        flags {"Symbols"}

    configuration "x32"
        libdirs { 
            "C:/VulkanSDK/1.0.39.1/Bin32",
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
            "C:/VulkanSDK/1.0.39.1/Bin",
        }
        links {"vulkan-1"}      
        defines 
        {
            "x64", 
            "VK_PROTOTYPES",
            "VK_USE_PLATFORM_WIN32_KHR",
        }

solution "TestingFenix" 
    location "./Tests"
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
    location "./Tests"
    --kind "ConsoleApp"
    --In case of using WindowedApp we have to specify te entry point
    kind "WindowedApp"
    flags { "WinMain" }

    targetdir "./Tests/build/$(Configuration)/$(Platform)"
    objdir "./Tests/obj/$(Platform)"
    debugdir "./Res"
    prebuildcommands { "call ../scripts/compile_engine.bat $(Platform) $(Configuration)" }

    includedirs {
        "C:/VulkanSDK/1.0.39.1/Include",
        "FenixDK/src",
        "External/",
        "Tests/src" 
    }
    files {
        "Tests/src/**"     
    }

    configuration "Release"
        defines { "_NDEBUG" }
        flags { "OptimizeSpeed" }

    configuration "Debug" 
        debugdir "./Res"
        defines { "_DEBUG" }
        flags {"Symbols"}

        libdirs { 
            "./FenixDK/build/$(Configuration)/$(Platform)",
        }
        links {"FenixDK"}


    configuration "x32"
        libdirs { 
            "C:/VulkanSDK/1.0.39.1/Bin32",
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
            "C:/VulkanSDK/1.0.39.1/Bin",
        }
        links {"vulkan-1"}      
        defines 
        {
            "x64", 
            "VK_PROTOTYPES",
            "VK_USE_PLATFORM_WIN32_KHR",
        }
