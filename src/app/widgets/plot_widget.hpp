#pragma once

#include <QWidget>
#include <QPaintEvent>
#include <vector>
#include <string>
#include <QString>
#include <QColor>

namespace workbench {
namespace app {

struct PlotSeries {
    QString title;
    std::vector<std::pair<double, double>> points;
    QColor color{0, 160, 255};
    int line_width{2};
    bool show_points{false};
    bool is_bar_chart{false};
};

/// 2D scientific plotting widget using QPainter.
class PlotWidget : public QWidget {
    Q_OBJECT

public:
    explicit PlotWidget(QWidget* parent = nullptr);

    /// Sets the plot title and axis titles with units.
    void setTitles(const QString& title, const QString& x_label, const QString& y_label);

    /// Adds or updates a curve series.
    void addSeries(const PlotSeries& series);

    /// Clears all series.
    void clear();

    /// Sets whether to use logarithmic scaling for Y axis (useful for convergence/loss plots).
    void setLogY(bool enable);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QString title_{"Plot"};
    QString x_label_{"X"};
    QString y_label_{"Y"};
    std::vector<PlotSeries> series_list_;
    bool log_y_{false};

    double x_min_{0.0};
    double x_max_{1.0};
    double y_min_{0.0};
    double y_max_{1.0};

    void computeBounds();
};

} // namespace app
} // namespace workbench

