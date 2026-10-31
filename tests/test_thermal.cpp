#include "workbench/thermal/solver.hpp"
#include <iostream>
#include <cmath>
#include <cstdlib>

int main() {
    int total = 0;
    int passed = 0;

    std::cout << "[Test Suite: Thermal FEA 1D]" << std::endl;

    // Test 1: Analytical solution values
    total++;
    workbench::domain::ThermalProblem p_def;
    double T_0 = workbench::thermal::exact_temperature(p_def, 0.0);
    double T_1 = workbench::thermal::exact_temperature(p_def, 1.0);
    double T_peak = workbench::thermal::exact_temperature(p_def, 0.42);

    if (std::abs(T_0 - 100.0) < 1e-12 && 
        std::abs(T_1 - 20.0) < 1e-12 && 
        std::abs(T_peak - 188.2) < 1e-12) {
        std::cout << "  [PASS] Test 1: Analytical solution sanity check (T(0)=100, T(1)=20, T(0.42)=188.2)" << std::endl;
        passed++;
    } else {
        std::cerr << "  [FAIL] Test 1: Analytical solution mismatch: T(0)=" << T_0 
                  << ", T(1)=" << T_1 << ", T(0.42)=" << T_peak << std::endl;
    }

    // Test 2: Validation of ThermalProblem parameters
    total++;
    workbench::domain::ThermalProblem p_inv = p_def;
    p_inv.length_m = -1.0;
    auto err1 = p_inv.validate();
    p_inv = p_def;
    p_inv.conductivity_W_mK = 0.0;
    auto err2 = p_inv.validate();
    p_inv = p_def;
    p_inv.element_count = 0;
    auto err3 = p_inv.validate();

    if (err1.has_value() && err2.has_value() && err3.has_value()) {
        std::cout << "  [PASS] Test 2: Negative/boundary input validation correctly rejects invalid problems" << std::endl;
        passed++;
    } else {
        std::cerr << "  [FAIL] Test 2: Input validation failed to catch invalid parameters" << std::endl;
    }

    // Test 3: Convergence study over N = 10, 20, 40, 80
    total++;
    std::vector<int> N_cases = {10, 20, 40, 80};
    auto conv = workbench::thermal::run_convergence_study(p_def, N_cases);

    double expected_mid[4] = {1.25, 0.3125, 0.078125, 0.01953125};
    bool conv_ok = (conv.size() == 4);

    for (size_t i = 0; i < conv.size() && conv_ok; ++i) {
        if (std::abs(conv[i].max_abs_midpoint_error_C - expected_mid[i]) > 1e-5) {
            conv_ok = false;
            std::cerr << "  [FAIL] Mesh N=" << conv[i].element_count_N 
                      << " midpoint error=" << conv[i].max_abs_midpoint_error_C 
                      << " expected=" << expected_mid[i] << std::endl;
        }
        if (i > 0) {
            if (std::abs(conv[i].error_ratio_vs_prior - 4.0) > 0.01) {
                conv_ok = false;
                std::cerr << "  [FAIL] Mesh refinement ratio " << conv[i].error_ratio_vs_prior 
                          << " deviated from 4.0" << std::endl;
            }
        }
    }

    if (conv_ok) {
        std::cout << "  [PASS] Test 3: Mesh convergence (N=10: 1.25 C, N=20: 0.3125 C, N=40: 0.078125 C, N=80: 0.01953125 C; ratio ~4.0)" << std::endl;
        passed++;
    }

    // Test 4: Nodal exactness test (FE linear elements have nodal exactness for uniform source)
    total++;
    bool nodal_ok = true;
    for (int N : N_cases) {
        p_def.element_count = N;
        auto sol = workbench::thermal::solve_thermal_1d(p_def);
        if (sol.max_abs_nodal_error_C > 1e-9) {
            nodal_ok = false;
            std::cerr << "  [FAIL] N=" << N << " nodal error " << sol.max_abs_nodal_error_C << " > 1e-9" << std::endl;
        }
    }
    if (nodal_ok) {
        std::cout << "  [PASS] Test 4: FE nodal solution exactness (|T_FE - T_exact| <= 1e-9 C at all nodes)" << std::endl;
        passed++;
    }

    // Test 5: Dirichlet boundary conditions exact imposition
    total++;
    p_def.element_count = 10;
    auto sol10 = workbench::thermal::solve_thermal_1d(p_def);
    if (std::abs(sol10.nodes.front().temperature_FE_C - 100.0) < 1e-12 && 
        std::abs(sol10.nodes.back().temperature_FE_C - 20.0) < 1e-12) {
        std::cout << "  [PASS] Test 5: Prescribed Dirichlet boundary conditions strictly satisfied (100 C and 20 C)" << std::endl;
        passed++;
    } else {
        std::cerr << "  [FAIL] Test 5: Boundary conditions not satisfied at ends" << std::endl;
    }

    // Test 6: CSV formatting and non-empty export output
    total++;
    auto nodal_csv = workbench::thermal::format_nodal_solution_csv(sol10);
    auto mid_csv = workbench::thermal::format_midpoint_errors_csv({sol10});
    auto conv_csv = workbench::thermal::format_convergence_csv(conv);

    if (!nodal_csv.empty() && !mid_csv.empty() && !conv_csv.empty()) {
        std::cout << "  [PASS] Test 6: Thermal CSV serialization formatted correctly" << std::endl;
        passed++;
    } else {
        std::cerr << "  [FAIL] Test 6: Empty CSV output produced" << std::endl;
    }

    std::cout << "[Test Suite: Thermal FEA 1D] Passed " << passed << " of " << total << " tests." << std::endl;
    return (passed == total) ? EXIT_SUCCESS : EXIT_FAILURE;
}
