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
