#pragma once

#include <vector>
#include <string>
#include <cmath>
#include <limits>
#include <optional>

namespace workbench {
namespace domain {

/// Physical inputs for the 1D steady-state rod problem.
/// Governing equation: -d/dx (k * A * dT/dx) = q * A, for 0 < x < L.
struct ThermalProblem {
    /// Rod length L in metres [m]. Required default: 1.0 m.
    double length_m{1.0};

    /// Cross-sectional area A in square metres [m^2]. Required default: 1e-4 m^2.
    double area_m2{1e-4};

    /// Thermal conductivity k in Watts per metre-Kelvin [W/(m*K)]. Required default: 10.0 W/(m*K).
    double conductivity_W_mK{10.0};

    /// Volumetric heat generation rate q in Watts per cubic metre [W/m^3]. Required default: 10000.0 W/m^3.
    double heat_generation_W_m3{10000.0};

    /// Prescribed left Dirichlet temperature T(0) in degrees Celsius [deg C]. Required default: 100.0 deg C.
    double left_temperature_C{100.0};

    /// Prescribed right Dirichlet temperature T(L) in degrees Celsius [deg C]. Required default: 20.0 deg C.
    double right_temperature_C{20.0};

    /// Number of uniform two-node elements N. Required cases: 10, 20, 40, 80.
    int element_count{10};

    /// Validates all parameters: finite, positive physical dimensions, N >= 1.
    /// Returns std::nullopt on success, or an error string describing the violation.
    std::optional<std::string> validate() const {
        if (!std::isfinite(length_m) || length_m <= 0.0) {
            return "Length must be a positive finite value (in metres).";
        }
        if (!std::isfinite(area_m2) || area_m2 <= 0.0) {
            return "Cross-sectional area must be a positive finite value (in square metres).";
        }
        if (!std::isfinite(conductivity_W_mK) || conductivity_W_mK <= 0.0) {
            return "Thermal conductivity must be a positive finite value (in W/(m*K)).";
        }
        if (!std::isfinite(heat_generation_W_m3)) {
            return "Volumetric heat generation must be a finite value (in W/m^3).";
        }
        if (!std::isfinite(left_temperature_C) || !std::isfinite(right_temperature_C)) {
            return "Boundary temperatures must be finite values (in deg C).";
        }
        if (element_count < 1) {
            return "Element count N must be at least 1.";
        }
        if (element_count > 1000000) {
            return "Element count N exceeds maximum allowed limit (1,000,000).";
        }
        return std::nullopt;
    }
};

/// A single node solution record.
struct NodalSolution {
    int node_id{0};
    double x_m{0.0};
    double temperature_FE_C{0.0};
    double temperature_exact_C{0.0};
    double abs_error_C{0.0};
};

/// Midpoint evaluation inside an element.
struct MidpointErrorRecord {
    int element_id{0};
    double x_mid_m{0.0};
    double temperature_FE_interp_C{0.0};
    double temperature_exact_C{0.0};
    double abs_error_C{0.0};
};

/// Mesh convergence summary for a given mesh refinement level.
struct ConvergenceRecord {
    int element_count_N{0};
    double element_size_h_m{0.0};
    double max_abs_midpoint_error_C{0.0};
    double theoretical_midpoint_error_C{0.0};
    double error_ratio_vs_prior{0.0}; // Expected ~4.0 for second-order convergence
};

/// Full solution of the 1D thermal FE problem.
struct ThermalSolution {
    ThermalProblem problem;
    double element_size_h_m{0.0};
    std::vector<NodalSolution> nodes;
    std::vector<MidpointErrorRecord> midpoints;
    double max_abs_nodal_error_C{0.0};
    double max_abs_midpoint_error_C{0.0};
    double theoretical_max_midpoint_error_C{0.0};
};

} // namespace domain
} // namespace workbench

