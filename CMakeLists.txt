cmake_minimum_required(VERSION 3.20)

project(EngiWorkbench
    VERSION 0.1.0
    DESCRIPTION "PASS Science Works - Unified Engineering Workbench"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable testing with CTest
include(CTest)
enable_testing()

# 1. Qt-independent computational and domain core
add_subdirectory(src/core)

# 2. Automated test suite for core
add_subdirectory(tests)

# 3. Presentation / application layer
add_subdirectory(src/app)

