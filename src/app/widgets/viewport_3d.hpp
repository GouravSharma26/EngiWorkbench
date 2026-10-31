#pragma once

#include <QWidget>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPaintEvent>
#include <optional>
#include <vector>
#include "workbench/domain/geometry.hpp"

namespace workbench {
namespace app {

/// 3D Viewport widget providing camera orbit, pan, zoom, fit-to-view,
/// triangle mesh rendering with face normals, and colored temperature point-cloud
/// with node picking inspection.
class Viewport3D : public QWidget {
    Q_OBJECT

public:
    explicit Viewport3D(QWidget* parent = nullptr);

    /// Sets the triangle mesh to render.
    void setMesh(const domain::MeshData& mesh);

    /// Sets the temperature point cloud to render.
    void setPointCloud(const domain::TemperatureField& field);

    /// Clears mesh and point cloud data.
    void clear();

    /// Resets camera orientation and fits view to active bounding box.
    void fitToView();

    /// Sets whether to render triangle wireframes.
    void setWireframeEnabled(bool enabled);

    /// Returns selected node, if any.
    std::optional<domain::TemperatureNode> selectedNode() const noexcept {
        return selected_node_;
    }

signals:
    /// Emitted when a node in the point cloud is clicked/picked.
    void nodeSelected(const domain::TemperatureNode& node);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    struct Vector3 {
        double x{0.0};
        double y{0.0};
        double z{0.0};
    };

    struct ProjectedPoint {
        double screen_x{0.0};
        double screen_y{0.0};
        double depth{0.0};
    };

    ProjectedPoint project(const domain::Point3D& pt) const;
    QColor temperatureToColor(double temp_C) const;
    void updateBounds();

    std::optional<domain::MeshData> mesh_;
    std::optional<domain::TemperatureField> point_cloud_;
    std::optional<domain::TemperatureNode> selected_node_;

    // Camera parameters
    double pitch_deg_{30.0};
    double yaw_deg_{-45.0};
    double zoom_{1.0};
    double pan_x_{0.0};
    double pan_y_{0.0};

    // Bounding box of all loaded data
    domain::Point3D bounds_min_{-1.0, -1.0, -1.0};
    domain::Point3D bounds_max_{1.0, 1.0, 1.0};
    domain::Point3D center_{0.0, 0.0, 0.0};
    double radius_{1.0};

    // Interaction state
    QPoint last_mouse_pos_;
    bool is_orbiting_{false};
    bool is_panning_{false};
    bool wireframe_enabled_{true};
};

} // namespace app
} // namespace workbench
