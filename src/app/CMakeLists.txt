find_package(Qt6 COMPONENTS Core Widgets OpenGLWidgets QUIET)

if(Qt6_FOUND)
    message(STATUS "Qt6 found (${Qt6_VERSION}). Building Qt presentation target workbench_app.")
    add_executable(workbench_app
        main.cpp
    )
    target_compile_definitions(workbench_app PRIVATE WORKBENCH_HAS_QT=1)
    target_link_libraries(workbench_app
        PRIVATE
            workbench_core
            Qt6::Core
            Qt6::Widgets
            Qt6::OpenGLWidgets
    )
    set_target_properties(workbench_app PROPERTIES
        AUTOMOC ON
        AUTOUIC ON
        AUTORCC ON
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
    )
else()
    message(STATUS "Qt6 (Widgets, OpenGLWidgets) not found in CMake prefix path.")
    message(STATUS "Building workbench_app in console placeholder mode until Qt 6 is configured.")
    add_executable(workbench_app
        main.cpp
    )
    target_link_libraries(workbench_app
        PRIVATE
            workbench_core
    )
    set_target_properties(workbench_app PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
    )
endif()

