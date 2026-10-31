#include "workbench/core.hpp"
#include <iostream>
#include <string>
#include <cstdlib>

int main() {
    int total_tests = 0;
    int passed_tests = 0;

    std::cout << "[Test Runner] Starting workbench_core test suite..." << std::endl;

    // Test 1: Core self-check
    total_tests++;
    if (workbench::core::self_check()) {
        std::cout << "  [PASS] Test 1: core::self_check() returned true" << std::endl;
        passed_tests++;
    } else {
        std::cerr << "  [FAIL] Test 1: core::self_check() returned false" << std::endl;
    }

    // Test 2: Core version validity
    total_tests++;
    auto v = workbench::core::get_version();
    if (v.major == 0 && v.minor == 1 && v.patch == 0 && v.release_type == "prototype") {
        std::cout << "  [PASS] Test 2: core::get_version() values match expected" << std::endl;
        passed_tests++;
    } else {
        std::cerr << "  [FAIL] Test 2: core::get_version() values mismatch" << std::endl;
    }

    // Test 3: Version string formatting
    total_tests++;
    std::string v_str = workbench::core::get_version_string();
    if (v_str == "v0.1.0-prototype") {
        std::cout << "  [PASS] Test 3: core::get_version_string() == \"v0.1.0-prototype\"" << std::endl;
        passed_tests++;
    } else {
        std::cerr << "  [FAIL] Test 3: core::get_version_string() returned " << v_str << std::endl;
    }

    std::cout << "[Test Runner] Finished. Passed " << passed_tests << " of " << total_tests << " tests." << std::endl;

    if (passed_tests == total_tests) {
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}

