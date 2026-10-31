#include "workbench/thermal/solver.hpp"
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>

namespace workbench {
namespace thermal {

double exact_temperature(const domain::ThermalProblem& problem, double x_m) {
    const double L = problem.length_m;
    const double k = problem.conductivity_W_mK;
    const double q = problem.heat_generation_W_m3;
    const double T0 = problem.left_temperature_C;
    const double TL = problem.right_temperature_C;

    // T_exact(x) = T0 + (TL - T0)*(x/L) + (q / (2*k)) * x * (L - x)
    return T0 + (TL - T0) * (x_m / L) + (q / (2.0 * k)) * x_m * (L - x_m);
}

domain::ThermalSolution solve_thermal_1d(const domain::ThermalProblem& problem) {
    auto err = problem.validate();
    if (err) {
        throw std::invalid_argument("ThermalProblem validation failed: " + *err);
    }

    const int N = problem.element_count;
    const double L = problem.length_m;
    const double A = problem.area_m2;
    const double k = problem.conductivity_W_mK;
    const double q = problem.heat_generation_W_m3;
    const double T0 = problem.left_temperature_C;
    const double TL = problem.right_temperature_C;

    const double h = L / static_cast<double>(N);
    const double c_k = (k * A) / h;      // Element stiffness factor
    const double f_interior = q * A * h; // Consistent interior load contribution (qAh/2 + qAh/2)

    domain::ThermalSolution solution;
    solution.problem = problem;
    solution.element_size_h_m = h;
    solution.theoretical_max_midpoint_error_C = (q * h * h) / (8.0 * k);

    std::vector<double> T_all(N + 1, 0.0);
    T_all[0] = T0;
    T_all[N] = TL;

    const int M = N - 1; // Number of interior unknowns: nodes 1 to N-1

    if (M > 0) {
        // Form the interior tridiagonal system: K_II * T_I = F_I - K_IB * T_B
        std::vector<double> diag(M, 2.0 * c_k);
        std::vector<double> sub(M, -c_k);
        std::vector<double> super(M, -c_k);
        std::vector<double> rhs(M, f_interior);

        // Account for known Dirichlet boundaries in the RHS
        rhs[0] += c_k * T0;
        rhs[M - 1] += c_k * TL;

        // Solve via Thomas algorithm
        // Forward sweep
        for (int i = 1; i < M; ++i) {
            if (std::abs(diag[i - 1]) < 1e-14) {
                throw std::runtime_error("Zero or near-zero pivot encountered in thermal FEA solver.");
            }
            double w = sub[i] / diag[i - 1];
            diag[i] -= w * super[i - 1];
            rhs[i] -= w * rhs[i - 1];
        }

        if (std::abs(diag[M - 1]) < 1e-14) {
            throw std::runtime_error("Zero or near-zero pivot encountered in thermal FEA back-substitution.");
        }

        // Back-substitution
        std::vector<double> T_interior(M, 0.0);
        T_interior[M - 1] = rhs[M - 1] / diag[M - 1];
        for (int i = M - 2; i >= 0; --i) {
            T_interior[i] = (rhs[i] - super[i] * T_interior[i + 1]) / diag[i];
        }

        // Store interior nodal temperatures
        for (int i = 0; i < M; ++i) {
            T_all[i + 1] = T_interior[i];
        }
    }

    // Populate nodal solution and calculate nodal errors
    double max_nodal_err = 0.0;
    solution.nodes.resize(N + 1);
    for (int i = 0; i <= N; ++i) {
        double x = static_cast<double>(i) * h;
        double T_fe = T_all[i];
        double T_ex = exact_temperature(problem, x);
        double err_i = std::abs(T_fe - T_ex);

        solution.nodes[i] = {i, x, T_fe, T_ex, err_i};
        max_nodal_err = std::max(max_nodal_err, err_i);
    }
    solution.max_abs_nodal_error_C = max_nodal_err;

    // Evaluate linear interpolation and error at element midpoints
    double max_mid_err = 0.0;
    solution.midpoints.resize(N);
    for (int e = 0; e < N; ++e) {
        double x_mid = (static_cast<double>(e) + 0.5) * h;
        double T_interp = 0.5 * (T_all[e] + T_all[e + 1]);
        double T_ex_mid = exact_temperature(problem, x_mid);
        double err_mid = std::abs(T_interp - T_ex_mid);

        solution.midpoints[e] = {e, x_mid, T_interp, T_ex_mid, err_mid};
        max_mid_err = std::max(max_mid_err, err_mid);
    }
    solution.max_abs_midpoint_error_C = max_mid_err;

    return solution;
}

std::vector<domain::ConvergenceRecord> run_convergence_study(
    domain::ThermalProblem base_problem, 
    const std::vector<int>& element_counts
) {
    std::vector<domain::ConvergenceRecord> results;
    results.reserve(element_counts.size());

    double prior_error = 0.0;

    for (int N : element_counts) {
        base_problem.element_count = N;
        auto sol = solve_thermal_1d(base_problem);

        double ratio = (prior_error > 0.0) ? (prior_error / sol.max_abs_midpoint_error_C) : 0.0;
        prior_error = sol.max_abs_midpoint_error_C;

        results.push_back({
            N,
            sol.element_size_h_m,
            sol.max_abs_midpoint_error_C,
            sol.theoretical_max_midpoint_error_C,
            ratio
        });
    }

    return results;
}

std::string format_nodal_solution_csv(const domain::ThermalSolution& solution) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(8);
    oss << "node_id,x_m,T_FE_C,T_exact_C,abs_error_C\n";
    for (const auto& node : solution.nodes) {
        oss << node.node_id << ","
            << node.x_m << ","
            << node.temperature_FE_C << ","
            << node.temperature_exact_C << ","
            << node.abs_error_C << "\n";
    }
    return oss.str();
}

std::string format_midpoint_errors_csv(const std::vector<domain::ThermalSolution>& solutions) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(8);
    oss << "N,element_id,x_mid_m,T_FE_interp_C,T_exact_C,abs_error_C\n";
    for (const auto& sol : solutions) {
        int N = sol.problem.element_count;
        for (const auto& mid : sol.midpoints) {
            oss << N << ","
                << mid.element_id << ","
                << mid.x_mid_m << ","
                << mid.temperature_FE_interp_C << ","
                << mid.temperature_exact_C << ","
                << mid.abs_error_C << "\n";
        }
    }
    return oss.str();
}

std::string format_convergence_csv(const std::vector<domain::ConvergenceRecord>& convergence) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(8);
    oss << "N,h_m,max_abs_midpoint_error_C,theoretical_midpoint_error_C,error_ratio_vs_prior\n";
    for (const auto& rec : convergence) {
        oss << rec.element_count_N << ","
            << rec.element_size_h_m << ","
            << rec.max_abs_midpoint_error_C << ","
            << rec.theoretical_midpoint_error_C << ","
            << rec.error_ratio_vs_prior << "\n";
    }
    return oss.str();
}

} // namespace thermal
} // namespace workbench

