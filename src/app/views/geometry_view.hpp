#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QTableWidget>
#include "workbench/widgets/viewport_3d.hpp"

namespace workbench {
namespace app {

class GeometryView : public QWidget {
    Q_OBJECT

public:
    explicit GeometryView(QWidget* parent = nullptr);

    /// Loads an ASCII STL file from disk.
    void loadStlFile(const QString& file_path);

    /// Loads a nodal temperature CSV file from disk.
    void loadTemperatureCsvFile(const QString& file_path);

private slots:
    void onBrowseStlClicked();
    void onBrowseCsvClicked();
    void onLoadExampleStlClicked();
    void onLoadSyntheticCsvClicked();
    void onNodeSelected(const domain::TemperatureNode& node);
    void onFitViewClicked();
    void onWireframeToggled(bool checked);

private:
    void setupUi();
    void updateMetadataLabel();

    Viewport3D* viewport_{nullptr};
    QLabel* status_label_{nullptr};
    QLabel* metadata_label_{nullptr};
    QTableWidget* node_table_{nullptr};
    QCheckBox* wireframe_check_{nullptr};

    QString current_stl_path_;
    QString current_csv_path_;
};

} // namespace app
} // namespace workbench

