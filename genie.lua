solution "FenixDK"
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
project "Core"
    --kind "ConsoleApp"
    --In case of using WindowedApp we have to specify te entry point
    kind "WindowedApp"
    flags { "WinMain" }

    includedirs {
        "C:/VulkanSDK/1.0.39.1/Include",
        "Core/src",
        "External/",
    }
    files {
        "Core/src/**",
        "./External/**",     
    }

    targetdir "./Core/build/$(Configuration)/$(Platform)"
    objdir "./Core/obj/$(Platform)"
    debugdir "./Res"

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
