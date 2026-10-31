#include "workbench/views/geometry_view.hpp"
#include "workbench/io/stl_parser.hpp"
#include "workbench/io/csv_parser.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>

namespace workbench {
namespace app {

GeometryView::GeometryView(QWidget* parent)
    : QWidget(parent) {
    setupUi();
}

void GeometryView::setupUi() {
    auto* main_layout = new QHBoxLayout(this);
    main_layout->setContentsMargins(12, 12, 12, 12);
    main_layout->setSpacing(12);

    // Left sidebar controls (320px)
    auto* sidebar = new QWidget(this);
    sidebar->setFixedWidth(330);
    auto* side_layout = new QVBoxLayout(sidebar);
    side_layout->setContentsMargins(0, 0, 0, 0);
    side_layout->setSpacing(10);

    // 1. STL Import Box
    auto* stl_group = new QGroupBox("CAD / Surface Geometry (ASCII STL)", sidebar);
    auto* stl_layout = new QVBoxLayout(stl_group);

    auto* browse_stl_btn = new QPushButton("Browse ASCII STL File...", stl_group);
    auto* example_stl_btn = new QPushButton("Load Bundled Sample Tetrahedron", stl_group);
    example_stl_btn->setStyleSheet("background-color: #2b3145; color: #89b4fa;");

    stl_layout->addWidget(browse_stl_btn);
    stl_layout->addWidget(example_stl_btn);
    side_layout->addWidget(stl_group);

    // 2. CSV Import Box
    auto* csv_group = new QGroupBox("Nodal Temperatures (ANSYS-Style CSV)", sidebar);
    auto* csv_layout = new QVBoxLayout(csv_group);

    auto* browse_csv_btn = new QPushButton("Browse Temperature CSV...", csv_group);
    auto* example_csv_btn = new QPushButton("Load Bundled Synthetic CSV", csv_group);
    example_csv_btn->setStyleSheet("background-color: #2b3145; color: #a6e3a1;");

    csv_layout->addWidget(browse_csv_btn);
    csv_layout->addWidget(example_csv_btn);
    side_layout->addWidget(csv_group);

    // 3. View Controls Box
    auto* view_group = new QGroupBox("Viewport Controls", sidebar);
    auto* view_layout = new QVBoxLayout(view_group);

    auto* fit_btn = new QPushButton("Fit Camera to Model", view_group);
    wireframe_check_ = new QCheckBox("Render Triangle Wireframes", view_group);
    wireframe_check_->setChecked(true);

    view_layout->addWidget(fit_btn);
    view_layout->addWidget(wireframe_check_);
    side_layout->addWidget(view_group);

    // 4. Inspection & Provenance Panel
    auto* info_group = new QGroupBox("Selected Node & Provenance", sidebar);
    auto* info_layout = new QVBoxLayout(info_group);

    metadata_label_ = new QLabel("No data loaded.\nUnits: STL model units | CSV: m, °C", info_group);
    metadata_label_->setWordWrap(true);
    metadata_label_->setStyleSheet("color: #bac2de; font-size: 11px;");

    node_table_ = new QTableWidget(5, 2, info_group);
    node_table_->setHorizontalHeaderLabels({"Property", "Value"});
    node_table_->verticalHeader()->setVisible(false);
    node_table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    node_table_->setItem(0, 0, new QTableWidgetItem("Node ID"));
    node_table_->setItem(1, 0, new QTableWidgetItem("X [m]"));
    node_table_->setItem(2, 0, new QTableWidgetItem("Y [m]"));
    node_table_->setItem(3, 0, new QTableWidgetItem("Z [m]"));
    node_table_->setItem(4, 0, new QTableWidgetItem("Temperature"));
    for (int r = 0; r < 5; ++r) {
        node_table_->setItem(r, 1, new QTableWidgetItem("-"));
    }
    node_table_->setFixedHeight(150);

    info_layout->addWidget(metadata_label_);
    info_layout->addWidget(node_table_);
    side_layout->addWidget(info_group);

    side_layout->addStretch();

    // Right Area: 3D Viewport
    viewport_ = new Viewport3D(this);
    main_layout->addWidget(sidebar);
    main_layout->addWidget(viewport_, 1);

    // Connect signals
    connect(browse_stl_btn, &QPushButton::clicked, this, &GeometryView::onBrowseStlClicked);
    connect(example_stl_btn, &QPushButton::clicked, this, &GeometryView::onLoadExampleStlClicked);
    connect(browse_csv_btn, &QPushButton::clicked, this, &GeometryView::onBrowseCsvClicked);
    connect(example_csv_btn, &QPushButton::clicked, this, &GeometryView::onLoadSyntheticCsvClicked);
    connect(fit_btn, &QPushButton::clicked, this, &GeometryView::onFitViewClicked);
    connect(wireframe_check_, &QCheckBox::toggled, this, &GeometryView::onWireframeToggled);
    connect(viewport_, &Viewport3D::nodeSelected, this, &GeometryView::onNodeSelected);
}

void GeometryView::onBrowseStlClicked() {
    QString path = QFileDialog::getOpenFileName(this, "Select ASCII STL File", "", "STL Files (*.stl);;All Files (*.*)");
    if (!path.isEmpty()) {
        loadStlFile(path);
    }
}

void GeometryView::onBrowseCsvClicked() {
    QString path = QFileDialog::getOpenFileName(this, "Select Nodal Temperature CSV File", "", "CSV Files (*.csv);;All Files (*.*)");
    if (!path.isEmpty()) {
        loadTemperatureCsvFile(path);
    }
}

void GeometryView::onLoadExampleStlClicked() {
    loadStlFile("data/examples/sample_tetrahedron_ascii.stl");
}

void GeometryView::onLoadSyntheticCsvClicked() {
    loadTemperatureCsvFile("data/examples/synthetic_temperature_nodes.csv");
}

void GeometryView::loadStlFile(const QString& file_path) {
    auto res = io::parse_ascii_stl_file(file_path.toStdString());
    if (res.is_ok()) {
        current_stl_path_ = file_path;
        viewport_->setMesh(*res.data);
        updateMetadataLabel();
    } else {
        QMessageBox::critical(this, "STL Import Error", 
            QString("Failed to import ASCII STL file:\n%1\nLine: %2")
                .arg(QString::fromStdString(res.error_message))
                .arg(res.line_number));
    }
}

void GeometryView::loadTemperatureCsvFile(const QString& file_path) {
    auto res = io::parse_temperature_csv_file(file_path.toStdString());
    if (res.is_ok()) {
        current_csv_path_ = file_path;
        viewport_->setPointCloud(*res.data);
        updateMetadataLabel();
    } else {
        QMessageBox::critical(this, "CSV Import Error", 
            QString("Failed to import Temperature CSV file:\n%1\nLine: %2, Column: %3")
                .arg(QString::fromStdString(res.error_message))
                .arg(res.line_number)
                .arg(res.column_number));
    }
}

void GeometryView::updateMetadataLabel() {
    QString text;
    if (!current_stl_path_.isEmpty()) {
        text += QString("<b>STL:</b> %1<br>Units: Model Units (by convention)<br>").arg(current_stl_path_);
    }
    if (!current_csv_path_.isEmpty()) {
        text += QString("<b>CSV:</b> %1<br><b>PROVENANCE:</b> <font color='#f9e2af'>Synthetic Data (Self-created)</font><br>Units: Coordinates [m], Temperature [°C]").arg(current_csv_path_);
    }
    metadata_label_->setText(text);
}

void GeometryView::onNodeSelected(const domain::TemperatureNode& node) {
    node_table_->item(0, 1)->setText(QString::number(node.node_id));
    node_table_->item(1, 1)->setText(QString::number(node.x_m, 'f', 4));
    node_table_->item(2, 1)->setText(QString::number(node.y_m, 'f', 4));
    node_table_->item(3, 1)->setText(QString::number(node.z_m, 'f', 4));
    node_table_->item(4, 1)->setText(QString("%1 °C").arg(node.temperature_C, 0, 'f', 2));
}

void GeometryView::onFitViewClicked() {
    viewport_->fitToView();
}

void GeometryView::onWireframeToggled(bool checked) {
    viewport_->setWireframeEnabled(checked);
}

} // namespace app
} // namespace workbench

