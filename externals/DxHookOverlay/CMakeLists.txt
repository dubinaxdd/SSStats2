add_library(DxHookOverlay SHARED
    src/DllMain.cpp
    src/DX9Hook.cpp
    src/DX9Hook.h
)

set_target_properties(DxHookOverlay PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_compile_definitions(DxHookOverlay PRIVATE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    UNICODE
    _UNICODE
)

target_link_libraries(DxHookOverlay PRIVATE
    d3d9
)

set_target_properties(DxHookOverlay PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY
        "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY
        "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY
        "${CMAKE_BINARY_DIR}/lib"
)
