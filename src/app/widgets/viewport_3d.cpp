#include "widgets/viewport_3d.hpp"
#include <QPainter>
#include <QPainterPath>
#include <cmath>
#include <algorithm>

namespace workbench {
namespace app {

namespace {

constexpr double kPi = 3.14159265358979323846;

double degToRad(double deg) {
    return deg * kPi / 180.0;
}

} // namespace

Viewport3D::Viewport3D(QWidget* parent)
    : QWidget(parent) {
    setMinimumSize(400, 300);
    setMouseTracking(true);
    setStyleSheet("background-color: #1e1e2e;");
}

void Viewport3D::setMesh(const domain::MeshData& mesh) {
    mesh_ = mesh;
    updateBounds();
    fitToView();
    update();
}

void Viewport3D::setPointCloud(const domain::TemperatureField& field) {
    point_cloud_ = field;
    updateBounds();
    fitToView();
    update();
}

void Viewport3D::clear() {
    mesh_.reset();
    point_cloud_.reset();
    selected_node_.reset();
    updateBounds();
    update();
}

void Viewport3D::fitToView() {
    pitch_deg_ = 30.0;
    yaw_deg_ = -45.0;
    pan_x_ = 0.0;
    pan_y_ = 0.0;
    zoom_ = 1.0;
    update();
}

void Viewport3D::setWireframeEnabled(bool enabled) {
    wireframe_enabled_ = enabled;
    update();
}

void Viewport3D::updateBounds() {
    bool has_data = false;
    domain::Point3D b_min{1e9, 1e9, 1e9};
    domain::Point3D b_max{-1e9, -1e9, -1e9};

    if (mesh_.has_value() && !mesh_->triangles.empty()) {
        has_data = true;
        b_min.x = std::min(b_min.x, mesh_->min_bounds.x);
        b_min.y = std::min(b_min.y, mesh_->min_bounds.y);
        b_min.z = std::min(b_min.z, mesh_->min_bounds.z);
        b_max.x = std::max(b_max.x, mesh_->max_bounds.x);
        b_max.y = std::max(b_max.y, mesh_->max_bounds.y);
        b_max.z = std::max(b_max.z, mesh_->max_bounds.z);
    }

    if (point_cloud_.has_value() && !point_cloud_->nodes.empty()) {
        has_data = true;
        b_min.x = std::min(b_min.x, point_cloud_->min_bounds.x);
        b_min.y = std::min(b_min.y, point_cloud_->min_bounds.y);
        b_min.z = std::min(b_min.z, point_cloud_->min_bounds.z);
        b_max.x = std::max(b_max.x, point_cloud_->max_bounds.x);
        b_max.y = std::max(b_max.y, point_cloud_->max_bounds.y);
        b_max.z = std::max(b_max.z, point_cloud_->max_bounds.z);
    }

    if (has_data) {
        bounds_min_ = b_min;
        bounds_max_ = b_max;
        center_ = {
            0.5 * (bounds_min_.x + bounds_max_.x),
            0.5 * (bounds_min_.y + bounds_max_.y),
            0.5 * (bounds_min_.z + bounds_max_.z)
        };
        double dx = bounds_max_.x - bounds_min_.x;
        double dy = bounds_max_.y - bounds_min_.y;
        double dz = bounds_max_.z - bounds_min_.z;
        radius_ = std::max({dx, dy, dz, 0.001}) * 0.5;
    } else {
        bounds_min_ = {-1.0, -1.0, -1.0};
        bounds_max_ = {1.0, 1.0, 1.0};
        center_ = {0.0, 0.0, 0.0};
        radius_ = 1.0;
    }
}

Viewport3D::ProjectedPoint Viewport3D::project(const domain::Point3D& pt) const {
    // Center point
    double cx = pt.x - center_.x;
    double cy = pt.y - center_.y;
    double cz = pt.z - center_.z;

    // Yaw rotation around Y axis
    double yaw = degToRad(yaw_deg_);
    double cos_y = std::cos(yaw);
    double sin_y = std::sin(yaw);
    double x1 = cos_y * cx + sin_y * cz;
    double y1 = cy;
    double z1 = -sin_y * cx + cos_y * cz;

    // Pitch rotation around X axis
    double pitch = degToRad(pitch_deg_);
    double cos_p = std::cos(pitch);
    double sin_p = std::sin(pitch);
    double x2 = x1;
    double y2 = cos_p * y1 - sin_p * z1;
    double z2 = sin_p * y1 + cos_p * z1;

    // Viewport scaling
    double w = width();
    double h = height();
    double scale = 0.45 * std::min(w, h) * zoom_ / radius_;

    ProjectedPoint proj;
    proj.screen_x = 0.5 * w + pan_x_ + x2 * scale;
    proj.screen_y = 0.5 * h + pan_y_ - y2 * scale; // Invert Y for screen coordinates
    proj.depth = z2;
    return proj;
}

QColor Viewport3D::temperatureToColor(double temp_C) const {
    if (!point_cloud_.has_value()) {
        return QColor(100, 200, 255);
    }
    double t_min = point_cloud_->min_temperature_C;
    double t_max = point_cloud_->max_temperature_C;

    double ratio = 0.5;
    if (std::abs(t_max - t_min) > 1e-6) {
        ratio = (temp_C - t_min) / (t_max - t_min);
    }
    ratio = std::clamp(ratio, 0.0, 1.0);

    // Map ratio to Hue: 240 deg (Blue, cold) down to 0 deg (Red, hot)
    int hue = static_cast<int>((1.0 - ratio) * 240.0);
    return QColor::fromHsv(hue, 220, 240);
}

void Viewport3D::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Background gradient
    QLinearGradient bg(0, 0, 0, height());
    bg.setColorAt(0.0, QColor(25, 27, 38));
    bg.setColorAt(1.0, QColor(15, 16, 24));
    painter.fillRect(rect(), bg);

    // 1. Draw Mesh Triangles if loaded
    if (mesh_.has_value()) {
        struct RenderTriangle {
            QPolygonF poly;
            double depth;
            QColor color;
        };
        std::vector<RenderTriangle> render_tris;
        render_tris.reserve(mesh_->triangles.size());

        for (const auto& tri : mesh_->triangles) {
            auto p1 = project(tri.v1);
            auto p2 = project(tri.v2);
            auto p3 = project(tri.v3);

            double avg_depth = (p1.depth + p2.depth + p3.depth) / 3.0;

            // Simple directional shading based on normal
            double nx = tri.normal.x;
            double ny = tri.normal.y;
            double nz = tri.normal.z;
            double len = std::sqrt(nx*nx + ny*ny + nz*nz);
            if (len > 1e-6) { nx /= len; ny /= len; nz /= len; }

            // Directional light from (0.5, 0.8, 0.3)
            double dot = std::max(0.15, 0.5*nx + 0.8*ny + 0.3*nz);
            int intensity = static_cast<int>(std::clamp(dot * 220.0 + 35.0, 0.0, 255.0));
            QColor face_color(intensity * 7 / 10, intensity * 8 / 10, intensity);

            QPolygonF poly;
            poly << QPointF(p1.screen_x, p1.screen_y)
                 << QPointF(p2.screen_x, p2.screen_y)
                 << QPointF(p3.screen_x, p3.screen_y);

            render_tris.push_back({poly, avg_depth, face_color});
        }

        // Painter's algorithm: sort triangles from back to front
        std::sort(render_tris.begin(), render_tris.end(), [](const auto& a, const auto& b) {
            return a.depth < b.depth;
        });

        for (const auto& r_tri : render_tris) {
            painter.setBrush(r_tri.color);
            painter.setPen(wireframe_enabled_ ? QPen(QColor(80, 100, 130, 140), 1) : Qt::NoPen);
            painter.drawPolygon(r_tri.poly);
        }
    }

    // 2. Draw Point Cloud Nodes if loaded
    if (point_cloud_.has_value()) {
        struct RenderNode {
            QPointF pos;
            double depth;
            QColor color;
            int id;
            double temp_C;
            bool is_selected;
        };
        std::vector<RenderNode> render_nodes;
        render_nodes.reserve(point_cloud_->nodes.size());

        for (const auto& node : point_cloud_->nodes) {
            domain::Point3D pt{node.x_m, node.y_m, node.z_m};
            auto proj = project(pt);
            QColor col = temperatureToColor(node.temperature_C);
            bool is_sel = selected_node_.has_value() && (selected_node_->node_id == node.node_id);

            render_nodes.push_back({
                QPointF(proj.screen_x, proj.screen_y),
                proj.depth,
                col,
                node.node_id,
                node.temperature_C,
                is_sel
            });
        }

        // Sort nodes back to front
        std::sort(render_nodes.begin(), render_nodes.end(), [](const auto& a, const auto& b) {
            return a.depth < b.depth;
        });

        for (const auto& r_node : render_nodes) {
            if (r_node.is_selected) {
                // Highlight ring
                painter.setPen(QPen(Qt::yellow, 2));
                painter.setBrush(r_node.color);
                painter.drawEllipse(r_node.pos, 8, 8);
                // Label
                painter.setPen(Qt::white);
                painter.drawText(r_node.pos + QPointF(10, -5), 
                    QString("ID: %1 (%2 °C)").arg(r_node.id).arg(r_node.temp_C, 0, 'f', 1));
            } else {
                painter.setPen(QPen(QColor(0, 0, 0, 120), 1));
                painter.setBrush(r_node.color);
                painter.drawEllipse(r_node.pos, 5, 5);
            }
        }

        // 3. Draw Temperature Legend in Top-Right
        int legend_w = 20;
        int legend_h = 140;
        int legend_x = width() - 70;
        int legend_y = 25;

        QLinearGradient grad(legend_x, legend_y + legend_h, legend_x, legend_y);
        grad.setColorAt(0.0, QColor::fromHsv(240, 220, 240)); // Blue
        grad.setColorAt(0.5, QColor::fromHsv(120, 220, 240)); // Green
        grad.setColorAt(1.0, QColor::fromHsv(0, 220, 240));   // Red

        painter.fillRect(QRect(legend_x, legend_y, legend_w, legend_h), grad);
        painter.setPen(QPen(Qt::white, 1));
        painter.drawRect(QRect(legend_x, legend_y, legend_w, legend_h));

        painter.setFont(QFont("Segoe UI", 8));
        painter.drawText(legend_x - 45, legend_y + 12, 
            QString("%1°C").arg(point_cloud_->max_temperature_C, 0, 'f', 0));
        painter.drawText(legend_x - 45, legend_y + legend_h, 
            QString("%1°C").arg(point_cloud_->min_temperature_C, 0, 'f', 0));
    }

    // 4. On-screen Controls Overlay & Coordinate Axes
    painter.setPen(QColor(180, 180, 200, 160));
    painter.setFont(QFont("Segoe UI", 9));
    painter.drawText(15, 25, "3D Viewport: Orbit (Left drag) | Pan (Right/Middle drag) | Zoom (Wheel)");

    if (!mesh_.has_value() && !point_cloud_.has_value()) {
        painter.setFont(QFont("Segoe UI", 12));
        painter.setPen(QColor(150, 150, 170));
        painter.drawText(rect(), Qt::AlignCenter, "No 3D Model or Point Cloud Loaded.\nImport an ASCII STL or Temperature CSV to begin.");
    }
}

void Viewport3D::mousePressEvent(QMouseEvent* event) {
    last_mouse_pos_ = event->pos();
    if (event->button() == Qt::LeftButton) {
        // Proximity picking test on nodes
        if (point_cloud_.has_value()) {
            double best_dist = 14.0; // Picking radius
            std::optional<domain::TemperatureNode> clicked;

            for (const auto& node : point_cloud_->nodes) {
                auto proj = project({node.x_m, node.y_m, node.z_m});
                double dx = proj.screen_x - event->pos().x();
                double dy = proj.screen_y - event->pos().y();
                double dist = std::sqrt(dx*dx + dy*dy);
                if (dist < best_dist) {
                    best_dist = dist;
                    clicked = node;
                }
            }

            if (clicked.has_value()) {
                selected_node_ = clicked;
                emit nodeSelected(*selected_node_);
                update();
                return;
            }
        }
        is_orbiting_ = true;
    } else if (event->button() == Qt::RightButton || event->button() == Qt::MiddleButton) {
        is_panning_ = true;
    }
}

void Viewport3D::mouseMoveEvent(QMouseEvent* event) {
    QPoint delta = event->pos() - last_mouse_pos_;
    last_mouse_pos_ = event->pos();

    if (is_orbiting_) {
        yaw_deg_ += delta.x() * 0.5;
        pitch_deg_ -= delta.y() * 0.5;
        pitch_deg_ = std::clamp(pitch_deg_, -89.0, 89.0);
        update();
    } else if (is_panning_) {
        pan_x_ += delta.x();
        pan_y_ += delta.y();
        update();
    }
}

void Viewport3D::mouseReleaseEvent(QMouseEvent* /*event*/) {
    is_orbiting_ = false;
    is_panning_ = false;
}

void Viewport3D::wheelEvent(QWheelEvent* event) {
    double num_degrees = event->angleDelta().y() / 8.0;
    double factor = std::pow(1.1, num_degrees / 15.0);
    zoom_ = std::clamp(zoom_ * factor, 0.05, 50.0);
    update();
}

} // namespace app
} // namespace workbench

