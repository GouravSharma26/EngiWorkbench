#include "workbench/widgets/plot_widget.hpp"
#include <QPainter>
#include <QFontMetrics>
#include <cmath>
#include <algorithm>

namespace workbench {
namespace app {

PlotWidget::PlotWidget(QWidget* parent)
    : QWidget(parent) {
    setMinimumSize(320, 220);
    setStyleSheet("background-color: #1e1e2e;");
}

void PlotWidget::setTitles(const QString& title, const QString& x_label, const QString& y_label) {
    title_ = title;
    x_label_ = x_label;
    y_label_ = y_label;
    update();
}

void PlotWidget::addSeries(const PlotSeries& series) {
    series_list_.push_back(series);
    computeBounds();
    update();
}

void PlotWidget::clear() {
    series_list_.clear();
    x_min_ = 0.0; x_max_ = 1.0;
    y_min_ = 0.0; y_max_ = 1.0;
    update();
}

void PlotWidget::setLogY(bool enable) {
    log_y_ = enable;
    computeBounds();
    update();
}

void PlotWidget::computeBounds() {
    if (series_list_.empty()) {
        x_min_ = 0.0; x_max_ = 1.0;
        y_min_ = 0.0; y_max_ = 1.0;
        return;
    }

    double x_min = 1e12, x_max = -1e12;
    double y_min = 1e12, y_max = -1e12;
    bool has_pts = false;

    for (const auto& s : series_list_) {
        for (const auto& pt : s.points) {
            has_pts = true;
            x_min = std::min(x_min, pt.first);
            x_max = std::max(x_max, pt.first);

            double y = pt.second;
            if (log_y_) {
                y = std::log10(std::max(y, 1e-12));
            }
            y_min = std::min(y_min, y);
            y_max = std::max(y_max, y);
        }
    }

    if (!has_pts) {
        x_min_ = 0.0; x_max_ = 1.0;
        y_min_ = 0.0; y_max_ = 1.0;
        return;
    }

    // Add 8% margin
    double dx = x_max - x_min;
    if (dx <= 1e-9) dx = 1.0;
    x_min_ = x_min - 0.04 * dx;
    x_max_ = x_max + 0.04 * dx;

    double dy = y_max - y_min;
    if (dy <= 1e-9) dy = 1.0;
    y_min_ = y_min - 0.08 * dy;
    y_max_ = y_max + 0.08 * dy;
}

void PlotWidget::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Background
    painter.fillRect(rect(), QColor(24, 25, 36));

    // Plot margin boundaries
    int margin_left = 65;
    int margin_right = 25;
    int margin_top = 40;
    int margin_bottom = 45;

    QRect plot_area(margin_left, margin_top, 
                    width() - margin_left - margin_right, 
                    height() - margin_top - margin_bottom);

    if (plot_area.width() <= 10 || plot_area.height() <= 10) return;

    // Draw grid background
    painter.fillRect(plot_area, QColor(16, 17, 24));
    painter.setPen(QPen(QColor(45, 48, 65), 1));
    painter.drawRect(plot_area);

    // Gridlines and Ticks
    int grid_x_steps = 5;
    int grid_y_steps = 5;

    painter.setFont(QFont("Segoe UI", 8));
    QFontMetrics fm = painter.fontMetrics();

    // Vertical grid & X ticks
    for (int i = 0; i <= grid_x_steps; ++i) {
        double r = static_cast<double>(i) / grid_x_steps;
        int px = plot_area.left() + static_cast<int>(r * plot_area.width());
        painter.setPen(QPen(QColor(38, 41, 58), 1, Qt::DashLine));
        painter.drawLine(px, plot_area.top(), px, plot_area.bottom());

        double val = x_min_ + r * (x_max_ - x_min_);
        QString val_str = QString::number(val, 'g', 3);
        int text_w = fm.horizontalAdvance(val_str);
        painter.setPen(QColor(150, 155, 175));
        painter.drawText(px - text_w / 2, plot_area.bottom() + 18, val_str);
    }

    // Horizontal grid & Y ticks
    for (int i = 0; i <= grid_y_steps; ++i) {
        double r = static_cast<double>(i) / grid_y_steps;
        int py = plot_area.bottom() - static_cast<int>(r * plot_area.height());
        painter.setPen(QPen(QColor(38, 41, 58), 1, Qt::DashLine));
        painter.drawLine(plot_area.left(), py, plot_area.right(), py);

        double val = y_min_ + r * (y_max_ - y_min_);
        QString val_str;
        if (log_y_) {
            val_str = QString("1e%1").arg(val, 0, 'f', 1);
        } else {
            val_str = QString::number(val, 'g', 3);
        }
        int text_w = fm.horizontalAdvance(val_str);
        painter.setPen(QColor(150, 155, 175));
        painter.drawText(plot_area.left() - text_w - 8, py + 4, val_str);
    }

    // Coordinate transforms
    auto map_pt = [&](double x, double y) -> QPointF {
        if (log_y_) {
            y = std::log10(std::max(y, 1e-12));
        }
        double rx = (x - x_min_) / (x_max_ - x_min_);
        double ry = (y - y_min_) / (y_max_ - y_min_);
        double px = plot_area.left() + rx * plot_area.width();
        double py = plot_area.bottom() - ry * plot_area.height();
        return QPointF(px, py);
    };

    // Draw Series
    painter.setClipRect(plot_area);

    for (const auto& s : series_list_) {
        if (s.points.empty()) continue;

        if (s.is_bar_chart) {
            // Draw bars
            double bar_width = std::max(6.0, static_cast<double>(plot_area.width()) / (s.points.size() * 3));
            for (const auto& pt : s.points) {
                QPointF top = map_pt(pt.first, pt.second);
                QPointF base = map_pt(pt.first, 0.0);
                QRectF bar_rect(top.x() - bar_width * 0.5, top.y(), bar_width, base.y() - top.y());
                painter.fillRect(bar_rect, s.color);
                painter.setPen(QPen(s.color.lighter(130), 1));
                painter.drawRect(bar_rect);
            }
        } else {
            // Draw lines
            QPolygonF poly;
            for (const auto& pt : s.points) {
                poly << map_pt(pt.first, pt.second);
            }
            painter.setPen(QPen(s.color, s.line_width));
            painter.drawPolyline(poly);

            // Draw points
            if (s.show_points) {
                painter.setBrush(s.color);
                painter.setPen(QPen(Qt::white, 1));
                for (int i = 0; i < poly.size(); ++i) {
                    painter.drawEllipse(poly[i], 3, 3);
                }
            }
        }
    }

    painter.setClipping(false);

    // Title & Axis Labels
    painter.setFont(QFont("Segoe UI", 10, QFont::Bold));
    painter.setPen(QColor(220, 225, 240));
    painter.drawText(plot_area.left(), margin_top - 12, title_);

    painter.setFont(QFont("Segoe UI", 8));
    painter.setPen(QColor(170, 175, 195));
    // X label
    painter.drawText(plot_area.left() + plot_area.width() / 2 - fm.horizontalAdvance(x_label_) / 2, 
                     height() - 8, x_label_);

    // Y label (rotated)
    painter.save();
    painter.translate(14, plot_area.top() + plot_area.height() / 2 + fm.horizontalAdvance(y_label_) / 2);
    painter.rotate(-90);
    painter.drawText(0, 0, y_label_);
    painter.restore();

    // Legend (top-right of plot area)
    if (!series_list_.empty()) {
        int leg_x = plot_area.right() - 140;
        int leg_y = plot_area.top() + 8;
        int leg_h = static_cast<int>(series_list_.size() * 18 + 8);
        painter.fillRect(QRect(leg_x, leg_y, 132, leg_h), QColor(20, 22, 32, 220));
        painter.setPen(QPen(QColor(60, 65, 85), 1));
        painter.drawRect(QRect(leg_x, leg_y, 132, leg_h));

        painter.setFont(QFont("Segoe UI", 8));
        for (size_t i = 0; i < series_list_.size(); ++i) {
            int item_y = leg_y + static_cast<int>(i * 18) + 14;
            painter.fillRect(QRect(leg_x + 8, item_y - 6, 12, 4), series_list_[i].color);
            painter.setPen(QColor(210, 215, 230));
            painter.drawText(leg_x + 26, item_y, series_list_[i].title);
        }
    }
}

} // namespace app
} // namespace workbench
