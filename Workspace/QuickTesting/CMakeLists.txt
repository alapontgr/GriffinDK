#file(GLOB_RECURSE GF_Srcs ${CMAKE_CURRENT_SOURCE_DIR}/Src/Common)
file(GLOB_RECURSE Srcs  
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.*)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Srcs})

# Create library
add_executable(QuickTesting ${Srcs})

# Add include dirs
set(includeDirs
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/Engine/Src
    ${PROJECT_SOURCE_DIR}/Engine/External
)
target_include_directories(QuickTesting PUBLIC ${includeDirs})

set_property(TARGET QuickTesting PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assets")

# Link against engine
target_link_libraries(QuickTesting ${engineName})