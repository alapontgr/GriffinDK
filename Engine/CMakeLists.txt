# Add some external libraries
AddSubmodule(External/glslang)

# Add SpirvCross
add_subdirectory(External/spirv-cross)

############################
## Setup Engine library
############################

#file(GLOB_RECURSE GF_Srcs ${CMAKE_CURRENT_SOURCE_DIR}/Src/Common)
file(GLOB_RECURSE GF_CommonSrcs  
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Common/*.*)
# Add srcs from external libraries
set(GF_ExternalSrcDirs
    glm
    vma
    stb
    json
)
set(GF_ExternalSrcs "")
foreach(externalDir ${GF_ExternalSrcDirs})
    file(GLOB_RECURSE tmp_list ${CMAKE_CURRENT_SOURCE_DIR}/External/${externalDir}/*.*)
    set(GF_ExternalSrcs ${GF_ExternalSrcs} ${tmp_list})
endforeach()

# Add platform files (TODO make this cross platform)
file(GLOB_RECURSE GF_PlatformSrcs  
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Win64/*.*) 
# Add Gfx files (TODO make this flexible with different Gfx APIs)
if(UseVulkan)
    file(GLOB_RECURSE GF_GfxSrcs  
        ${CMAKE_CURRENT_SOURCE_DIR}/Src/Vulkan/*.*) 
endif()

# Set library files
set(GF_Srcs
    ${GF_CommonSrcs}
    ${GF_PlatformSrcs}
    ${GF_ExternalSrcs}
    ${GF_GfxSrcs}
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${GF_Srcs})

# Create library
add_library(${engineName} ${GF_Srcs})

# Add include dirs
set(GF_Includes 
    ${CMAKE_CURRENT_SOURCE_DIR}/Src
    ${CMAKE_CURRENT_SOURCE_DIR}/External
)
target_include_directories(${engineName} PUBLIC ${GF_Includes})
target_link_libraries(${engineName} glslang SPIRV GeSpirvCross)
target_compile_definitions(${engineName} PUBLIC _PLATFORM_DIR=Win64)

# Setup Gfx dependencies
SetGfxSdk(${engineName})