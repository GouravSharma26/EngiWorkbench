#include "main_window.hpp"
#include "workbench/core.hpp"
#include <QMenuBar>
#include <QStatusBar>
#include <QMessageBox>

namespace workbench {
namespace app {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    setupUi();
}

void MainWindow::setupUi() {
    setWindowTitle("PASS Science Works — Unified Engineering Workbench");
    resize(1280, 820);

    // Dark engineering theme stylesheet
    setStyleSheet(
        "QMainWindow { background-color: #181825; color: #cdd6f4; }"
        "QTabWidget::pane { border: 1px solid #313244; background-color: #1e1e2e; }"
        "QTabBar::tab { background-color: #181825; color: #a6adc8; padding: 10px 24px; font-weight: bold; border-top-left-radius: 4px; border-top-right-radius: 4px; margin-right: 2px; }"
        "QTabBar::tab:selected { background-color: #1e1e2e; color: #89b4fa; border-top: 2px solid #89b4fa; }"
        "QTabBar::tab:hover { background-color: #313244; color: #cdd6f4; }"
        "QGroupBox { font-weight: bold; border: 1px solid #313244; border-radius: 6px; margin-top: 10px; padding-top: 10px; color: #cdd6f4; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; color: #89b4fa; }"
        "QPushButton { background-color: #313244; color: #cdd6f4; border: 1px solid #45475a; border-radius: 4px; padding: 6px 12px; font-size: 11px; }"
        "QPushButton:hover { background-color: #45475a; border-color: #89b4fa; }"
        "QPushButton:pressed { background-color: #585b70; }"
        "QPushButton:disabled { background-color: #181825; color: #585b70; border-color: #313244; }"
        "QDoubleSpinBox, QSpinBox, QLineEdit { background-color: #181825; color: #cdd6f4; border: 1px solid #45475a; border-radius: 4px; padding: 4px; selection-background-color: #45475a; }"
        "QTableWidget { background-color: #181825; color: #cdd6f4; gridline-color: #313244; border: 1px solid #313244; selection-background-color: #45475a; }"
        "QHeaderView::section { background-color: #1e1e2e; color: #a6adc8; border: 1px solid #313244; padding: 4px; font-size: 11px; font-weight: bold; }"
    );

    // Menu Bar
    auto* file_menu = menuBar()->addMenu("&File");
    auto* exit_action = file_menu->addAction("E&xit");
    connect(exit_action, &QAction::triggered, this, &QWidget::close);

    auto* view_menu = menuBar()->addMenu("&Views");
    auto* act_geo = view_menu->addAction("&Geometry (STL / CSV)");
    auto* act_thermal = view_menu->addAction("&Thermal Analysis");
    auto* act_perf = view_menu->addAction("&Performance Benchmark");
    auto* act_hw = view_menu->addAction("&Hardware Mocks");
    auto* act_bp = view_menu->addAction("&Backpropagation");

    auto* help_menu = menuBar()->addMenu("&Help");
    auto* about_action = help_menu->addAction("&About Unified Engineering Workbench...");
    connect(about_action, &QAction::triggered, this, &MainWindow::onAboutTriggered);

    // Tab Widget
    tabs_ = new QTabWidget(this);
    geometry_view_ = new GeometryView(this);
    thermal_view_ = new ThermalView(this);
    performance_view_ = new PerformanceView(this);
    hardware_view_ = new HardwareView(this);
    backprop_view_ = new BackpropView(this);

    tabs_->addTab(geometry_view_, "  Geometry (STL & Point Cloud)  ");
    tabs_->addTab(thermal_view_, "  Thermal Analysis (1D FEA)  ");
    tabs_->addTab(performance_view_, "  Performance (Multicore)  ");
    tabs_->addTab(hardware_view_, "  Hardware (Simulated Mocks)  ");
    tabs_->addTab(backprop_view_, "  Backpropagation (Tanh NN)  ");

    setCentralWidget(tabs_);

    // Connect menu view navigation
    connect(act_geo, &QAction::triggered, this, [this]() { tabs_->setCurrentIndex(0); });
    connect(act_thermal, &QAction::triggered, this, [this]() { tabs_->setCurrentIndex(1); });
    connect(act_perf, &QAction::triggered, this, [this]() { tabs_->setCurrentIndex(2); });
    connect(act_hw, &QAction::triggered, this, [this]() { tabs_->setCurrentIndex(3); });
    connect(act_bp, &QAction::triggered, this, [this]() { tabs_->setCurrentIndex(4); });

    // Status Bar
    status_label_ = new QLabel(
        QString("Workbench Core: %1 | Provenance: Hardware adapters SIMULATED. Temperature CSV synthetic.")
            .arg(QString::fromStdString(core::get_version_string())),
        this
    );
    status_label_->setStyleSheet("color: #a6adc8; font-size: 11px; padding: 2px 8px;");
    statusBar()->addPermanentWidget(status_label_);
}

void MainWindow::onAboutTriggered() {
    QMessageBox::about(
        this,
        "About Unified Engineering Workbench",
        "<h3>PASS Science Works — Unified Engineering Workbench</h3>"
        "<p><b>R&D Internship 2026–2027 Engineering Assignment</b></p>"
        "<p><b>Version:</b> " + QString::fromStdString(core::get_version_string()) + "</p>"
        "<p><b>Key Capabilities:</b>"
        "<ul>"
        "<li><b>Task B:</b> ASCII STL 3D renderer and ANSYS-style nodal temperature point cloud with click inspection.</li>"
        "<li><b>Task C:</b> 1D steady-state thermal FEA solver with exact analytical comparison and mesh convergence verification.</li>"
        "<li><b>Task D:</b> Multicore ThreadPool batch runner with deterministic input ordering and <= 1e-8 °C agreement check.</li>"
        "<li><b>Task E:</b> Embedded MCU (ESP32), FPGA (Arty A7), and JTAG adapter mocks with fault recovery walkthrough.</li>"
        "<li><b>Task F:</b> Handwritten-style backpropagation with central finite-difference gradient check (<= 1e-5 error).</li>"
        "</ul></p>"
        "<p><i>Note: Device interactions are strictly simulated. Temperature CSV is synthetic.</i></p>"
    );
}

} // namespace app
} // namespace workbench

