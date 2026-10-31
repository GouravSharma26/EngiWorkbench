find_package(Qt6 COMPONENTS Core Widgets OpenGLWidgets QUIET)

if(Qt6_FOUND)
    message(STATUS "Qt6 found (${Qt6_VERSION}). Building Qt presentation target workbench_app.")
    add_executable(workbench_app
        main.cpp
        main_window.hpp
        main_window.cpp
        widgets/viewport_3d.hpp
        widgets/viewport_3d.cpp
        widgets/plot_widget.hpp
        widgets/plot_widget.cpp
        views/geometry_view.hpp
        views/geometry_view.cpp
        views/thermal_view.hpp
        views/thermal_view.cpp
        views/performance_view.hpp
        views/performance_view.cpp
        views/hardware_view.hpp
        views/hardware_view.cpp
        views/backprop_view.hpp
        views/backprop_view.cpp
    )

    target_include_directories(workbench_app PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/widgets
        ${CMAKE_CURRENT_SOURCE_DIR}/views
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
