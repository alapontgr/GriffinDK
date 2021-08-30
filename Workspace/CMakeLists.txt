add_subdirectory(QuickTesting)
add_subdirectory(TestShaderCompiler)

set_target_properties (QuickTesting PROPERTIES
    FOLDER Tests
)
set_target_properties (TestShaderCompiler PROPERTIES
    FOLDER Tests
)