#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include "workbench/backprop/model.hpp"
#include "workbench/widgets/plot_widget.hpp"

namespace workbench {
namespace app {

class BackpropView : public QWidget {
    Q_OBJECT

public:
    explicit BackpropView(QWidget* parent = nullptr);

private slots:
    void onForwardClicked();
    void onGradientsClicked();
    void onGradientCheckClicked();
    void onStepClicked();
    void onTrainClicked();
    void onResetClicked();
    void onExportTraceClicked();
    void onExportCheckClicked();

private:
    void setupUi();
    void updateParameterTable();
    void updateForwardDisplay();
    void updateLossPlot(const std::vector<backprop::TrainingStepRecord>& trace);

    backprop::Model model_{1.0, 2.0, {0.5, 0.0, 1.0, 0.0}};

    // Displays
    QLabel* forward_label_{nullptr};
    QLabel* check_status_label_{nullptr};
    QTableWidget* param_table_{nullptr};
    PlotWidget* loss_plot_{nullptr};

    // Cached results for export
    std::vector<backprop::TrainingStepRecord> current_trace_;
    std::optional<backprop::GradientCheckResult> current_check_;
};

} // namespace app
} // namespace workbench
