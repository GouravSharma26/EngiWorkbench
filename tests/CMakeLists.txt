add_executable(workbench_core_tests
    test_main.cpp
)

target_link_libraries(workbench_core_tests
    PRIVATE
        workbench_core
)

target_compile_features(workbench_core_tests PRIVATE cxx_std_17)

add_test(
    NAME workbench_core_tests
    COMMAND workbench_core_tests
)

add_executable(workbench_thermal_tests
    test_thermal.cpp
)

target_link_libraries(workbench_thermal_tests
    PRIVATE
        workbench_core
)

target_compile_features(workbench_thermal_tests PRIVATE cxx_std_17)

add_test(
    NAME workbench_thermal_tests
    COMMAND workbench_thermal_tests
)

add_executable(workbench_parser_tests
    test_parsers.cpp
)

target_link_libraries(workbench_parser_tests
    PRIVATE
        workbench_core
)

target_compile_features(workbench_parser_tests PRIVATE cxx_std_17)

add_test(
    NAME workbench_parser_tests
    COMMAND workbench_parser_tests
)

add_executable(workbench_performance_tests
    test_performance.cpp
)

target_link_libraries(workbench_performance_tests
    PRIVATE
        workbench_core
)

target_compile_features(workbench_performance_tests PRIVATE cxx_std_17)

add_test(
    NAME workbench_performance_tests
    COMMAND workbench_performance_tests
)

add_executable(workbench_embedded_tests
    test_embedded.cpp
)

target_link_libraries(workbench_embedded_tests
    PRIVATE
        workbench_core
)

target_compile_features(workbench_embedded_tests PRIVATE cxx_std_17)

add_test(
    NAME workbench_embedded_tests
    COMMAND workbench_embedded_tests
)

add_executable(workbench_backprop_tests
    test_backprop.cpp
)

target_link_libraries(workbench_backprop_tests
    PRIVATE
        workbench_core
)

target_compile_features(workbench_backprop_tests PRIVATE cxx_std_17)

add_test(
    NAME workbench_backprop_tests
    COMMAND workbench_backprop_tests
)

set_tests_properties(
    workbench_core_tests
    workbench_thermal_tests
    workbench_parser_tests
    workbench_performance_tests
    workbench_embedded_tests
    workbench_backprop_tests
    PROPERTIES WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)
