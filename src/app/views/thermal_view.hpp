#pragma once

#include <QWidget>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include "workbench/domain/thermal.hpp"
#include "workbench/widgets/plot_widget.hpp"

namespace workbench {
namespace app {

class ThermalView : public QWidget {
    Q_OBJECT

public:
    explicit ThermalView(QWidget* parent = nullptr);

private slots:
    void onSolveClicked();
    void onConvergenceStudyClicked();
    void onExportNodalCsvClicked();
    void onExportMidpointCsvClicked();
    void onExportConvergenceCsvClicked();

private:
    void setupUi();
    domain::ThermalProblem readInputs() const;
    void updateTemperaturePlot(const domain::ThermalSolution& sol);
    void updateConvergencePlot(const std::vector<domain::ConvergenceRecord>& records);

    // Inputs
    QDoubleSpinBox* length_spin_{nullptr};
    QDoubleSpinBox* area_spin_{nullptr};
    QDoubleSpinBox* k_spin_{nullptr};
    QDoubleSpinBox* q_spin_{nullptr};
    QDoubleSpinBox* t0_spin_{nullptr};
    QDoubleSpinBox* tl_spin_{nullptr};
    QSpinBox* n_spin_{nullptr};

    // Plots
    PlotWidget* temp_plot_{nullptr};
    PlotWidget* conv_plot_{nullptr};

    // Table & Results
    QTableWidget* convergence_table_{nullptr};
    QLabel* summary_label_{nullptr};

    // Cached results for export
    std::optional<domain::ThermalSolution> current_solution_;
    std::vector<domain::ThermalSolution> study_solutions_;
    std::vector<domain::ConvergenceRecord> convergence_records_;
};

} // namespace app
} // namespace workbench

