#pragma once

#include <vector>
#include <string>
#include <cmath>
#include <limits>
#include <optional>

namespace workbench {
namespace domain {

/// 3D point in Cartesian coordinates. Units specified by context (metres by convention).
struct Point3D {
    double x{0.0};
    double y{0.0};
    double z{0.0};

    bool is_finite() const noexcept {
        return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
    }
};

/// 3D triangle facet with normal vector and three vertices.
struct Triangle {
    Point3D normal;
    Point3D v1;
    Point3D v2;
    Point3D v3;

    bool is_valid() const noexcept {
        return normal.is_finite() && v1.is_finite() && v2.is_finite() && v3.is_finite();
    }
};

/// Complete surface mesh representation parsed from ASCII STL.
struct MeshData {
    std::string solid_name;
    std::vector<Triangle> triangles;
    Point3D min_bounds;
    Point3D max_bounds;

    size_t facet_count() const noexcept {
        return triangles.size();
    }
};

/// A single node record matching ANSYS synthetic CSV export schema:
/// (NodeNumber, X, Y, Z, Temperature)
struct TemperatureNode {
    int node_id{0};
    double x_m{0.0};
    double y_m{0.0};
    double z_m{0.0};
    double temperature_C{0.0};

    bool is_valid() const noexcept {
        return node_id > 0 && std::isfinite(x_m) && std::isfinite(y_m) && 
               std::isfinite(z_m) && std::isfinite(temperature_C);
    }
};

/// Temperature point cloud with bounds, statistics, and provenance metadata.
struct TemperatureField {
    std::vector<TemperatureNode> nodes;
    Point3D min_bounds;
    Point3D max_bounds;
    double min_temperature_C{0.0};
    double max_temperature_C{0.0};
    bool is_synthetic{true};
    std::string source_description;

    size_t node_count() const noexcept {
        return nodes.size();
    }
};

} // namespace domain
} // namespace workbench

