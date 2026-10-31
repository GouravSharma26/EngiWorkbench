add_library(workbench_core STATIC
    src/core.cpp
    src/thermal/solver.cpp
)

target_include_directories(workbench_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(workbench_core PUBLIC cxx_std_17)

# Enforce strictly that workbench_core has no Qt dependencies
set_target_properties(workbench_core PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
