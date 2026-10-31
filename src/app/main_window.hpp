#pragma once

#include <QMainWindow>
#include <QTabWidget>
#include <QLabel>
#include "views/geometry_view.hpp"
#include "views/thermal_view.hpp"
#include "views/performance_view.hpp"
#include "views/hardware_view.hpp"
#include "views/backprop_view.hpp"

namespace workbench {
namespace app {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onAboutTriggered();

private:
    void setupUi();

    QTabWidget* tabs_{nullptr};
    GeometryView* geometry_view_{nullptr};
    ThermalView* thermal_view_{nullptr};
    PerformanceView* performance_view_{nullptr};
    HardwareView* hardware_view_{nullptr};
    BackpropView* backprop_view_{nullptr};
    QLabel* status_label_{nullptr};
};

} // namespace app
} // namespace workbench

