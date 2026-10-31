#pragma once

#include "workbench/domain/thermal.hpp"
#include <vector>
#include <string>
#include <stdexcept>

namespace workbench {
namespace thermal {

/// Evaluates the analytical exact temperature field at position x_m in [0, L].
/// Formula: T_exact(x) = T0 + (TL - T0)*(x/L) + (q / (2*k)) * x * (L - x)
double exact_temperature(const domain::ThermalProblem& problem, double x_m);

/// Solves the 1D steady-state heat conduction problem using uniform 2-node linear elements.
/// Assembles stiffness and consistent load vectors, imposes Dirichlet boundary conditions
/// by interior reduction: K_II * T_I = F_I - K_IB * T_B, and solves the resulting
/// tridiagonal system via Thomas algorithm.
/// Throws std::invalid_argument if problem parameters are invalid.
domain::ThermalSolution solve_thermal_1d(const domain::ThermalProblem& problem);

/// Runs a systematic mesh convergence study over a set of element counts (e.g. N = 10, 20, 40, 80).
/// Computes midpoint errors and verifies error ratios (expected ~4.0 for second-order convergence).
std::vector<domain::ConvergenceRecord> run_convergence_study(
    domain::ThermalProblem base_problem, 
    const std::vector<int>& element_counts = {10, 20, 40, 80}
);

/// Exports nodal temperature solution to CSV format:
/// "node_id,x_m,T_FE_C,T_exact_C,abs_error_C"
std::string format_nodal_solution_csv(const domain::ThermalSolution& solution);

/// Exports midpoint errors table to CSV format:
/// "N,element_id,x_mid_m,T_FE_interp_C,T_exact_C,abs_error_C"
std::string format_midpoint_errors_csv(const std::vector<domain::ThermalSolution>& solutions);

/// Exports mesh convergence study summary to CSV format:
/// "N,h_m,max_abs_midpoint_error_C,theoretical_midpoint_error_C,error_ratio"
std::string format_convergence_csv(const std::vector<domain::ConvergenceRecord>& convergence);

} // namespace thermal
} // namespace workbench

