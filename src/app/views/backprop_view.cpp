#include "workbench/views/backprop_view.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <fstream>

namespace workbench {
namespace app {

BackpropView::BackpropView(QWidget* parent)
    : QWidget(parent) {
    setupUi();
    onForwardClicked();
}

void BackpropView::setupUi() {
    auto* main_layout = new QHBoxLayout(this);
    main_layout->setContentsMargins(12, 12, 12, 12);
    main_layout->setSpacing(12);

    // Left sidebar controls (360px)
    auto* sidebar = new QWidget(this);
    sidebar->setFixedWidth(360);
    auto* side_layout = new QVBoxLayout(sidebar);
    side_layout->setContentsMargins(0, 0, 0, 0);
    side_layout->setSpacing(10);

    // 1. Model Formulation Box
    auto* form_group = new QGroupBox("One-Sample Tanh Regression Model", sidebar);
    auto* form_layout = new QVBoxLayout(form_group);

    auto* eq_label = new QLabel(
        "<b>Input:</b> x = 1.0, <b>Target:</b> y = 2.0<br>"
        "<b>Hidden:</b> h = tanh(w·x + b)<br>"
        "<b>Prediction:</b> ŷ = v·h + c<br>"
        "<b>Loss:</b> J = ½(ŷ - y)²<br>"
        "<b>Learning Rate:</b> η = 0.1",
        form_group
    );
    eq_label->setStyleSheet("color: #bac2de; font-size: 11px;");
    form_layout->addWidget(eq_label);
    side_layout->addWidget(form_group);

    // 2. Action Buttons Box
    auto* act_group = new QGroupBox("Backpropagation Actions", sidebar);
    auto* act_layout = new QVBoxLayout(act_group);

    auto* fwd_btn = new QPushButton("Evaluate Forward Pass", act_group);
    auto* grad_btn = new QPushButton("Compute Analytical Gradients", act_group);
    auto* check_btn = new QPushButton("Run Finite-Diff Check (ε=1e-6, ≤ 1e-5)", act_group);
    check_btn->setStyleSheet("background-color: #2b3145; color: #a6e3a1; font-weight: bold;");

    auto* step_btn = new QPushButton("Apply 1 Simultaneous Update Step", act_group);
    auto* train_btn = new QPushButton("Run 100-Step Training Trace", act_group);
    train_btn->setStyleSheet("background-color: #2b3145; color: #89b4fa; font-weight: bold;");

    auto* reset_btn = new QPushButton("Reset to Initial Parameters", act_group);
    reset_btn->setStyleSheet("background-color: #2b3145; color: #f38ba8;");

    act_layout->addWidget(fwd_btn);
    act_layout->addWidget(grad_btn);
    act_layout->addWidget(check_btn);
    act_layout->addWidget(step_btn);
    act_layout->addWidget(train_btn);
    act_layout->addWidget(reset_btn);
    side_layout->addWidget(act_group);

    // 3. Forward Pass & Check Output Display
    auto* out_group = new QGroupBox("Forward & Gradient Status", sidebar);
    auto* out_layout = new QVBoxLayout(out_group);

    forward_label_ = new QLabel("Forward pass: pending.", out_group);
    forward_label_->setWordWrap(true);
    forward_label_->setStyleSheet("background-color: #181825; color: #89b4fa; padding: 6px; border-radius: 4px; font-family: monospace; font-size: 11px;");

    check_status_label_ = new QLabel("Gradient check: Not run.", out_group);
    check_status_label_->setStyleSheet("color: #bac2de; font-size: 11px; font-weight: bold;");

    out_layout->addWidget(forward_label_);
    out_layout->addWidget(check_status_label_);
    side_layout->addWidget(out_group);

    // 4. Export Box
    auto* exp_group = new QGroupBox("Export Results", sidebar);
    auto* exp_layout = new QVBoxLayout(exp_group);
    auto* exp_trace_btn = new QPushButton("Export 100-Step Training Trace CSV...", exp_group);
    auto* exp_check_btn = new QPushButton("Export Gradient Check CSV...", exp_group);

    exp_layout->addWidget(exp_trace_btn);
    exp_layout->addWidget(exp_check_btn);
    side_layout->addWidget(exp_group);

    side_layout->addStretch();

    // Right Area: Parameter Table & Loss Curve Plot
    auto* right_widget = new QWidget(this);
    auto* right_layout = new QVBoxLayout(right_widget);
    right_layout->setContentsMargins(0, 0, 0, 0);
    right_layout->setSpacing(10);

    // Parameter Table
    param_table_ = new QTableWidget(4, 5, right_widget);
    param_table_->setHorizontalHeaderLabels({"Parameter", "Value θ", "Analytic ∇J", "Finite Diff g_FD", "Abs Error"});
    param_table_->verticalHeader()->setVisible(false);
    param_table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    param_table_->setFixedHeight(145);
    right_layout->addWidget(param_table_);

    // 2D Loss Curve Plot
    loss_plot_ = new PlotWidget(right_widget);
    loss_plot_->setTitles("Training Convergence: Loss J vs Optimization Step", "Optimization Step (0 to 100)", "Loss J = 0.5(ŷ - y)²");
    right_layout->addWidget(loss_plot_, 1);

    main_layout->addWidget(sidebar);
    main_layout->addWidget(right_widget, 1);

    // Connections
    connect(fwd_btn, &QPushButton::clicked, this, &BackpropView::onForwardClicked);
    connect(grad_btn, &QPushButton::clicked, this, &BackpropView::onGradientsClicked);
    connect(check_btn, &QPushButton::clicked, this, &BackpropView::onGradientCheckClicked);
    connect(step_btn, &QPushButton::clicked, this, &BackpropView::onStepClicked);
    connect(train_btn, &QPushButton::clicked, this, &BackpropView::onTrainClicked);
    connect(reset_btn, &QPushButton::clicked, this, &BackpropView::onResetClicked);
    connect(exp_trace_btn, &QPushButton::clicked, this, &BackpropView::onExportTraceClicked);
    connect(exp_check_btn, &QPushButton::clicked, this, &BackpropView::onExportCheckClicked);
}

void BackpropView::updateForwardDisplay() {
    auto f = model_.forward();
    forward_label_->setText(
        QString("z = %1 | h = tanh(z) = %2\n"
                "Prediction ŷ = %3 | Target y = %4\n"
                "Error δ = %5 | Loss J = %6")
            .arg(f.z, 0, 'f', 6)
            .arg(f.h, 0, 'f', 6)
            .arg(f.y_hat, 0, 'f', 6)
            .arg(model_.target_y(), 0, 'f', 1)
            .arg(f.delta, 0, 'f', 6)
            .arg(f.loss, 0, 'f', 6)
    );
}

void BackpropView::updateParameterTable() {
    auto p = model_.parameters();
    auto g = model_.compute_gradients();
    auto fd = model_.compute_finite_differences(1e-6);

    auto set_row = [&](int row, const char* name, double val, double grad, double fd_val) {
        param_table_->setItem(row, 0, new QTableWidgetItem(name));
        param_table_->setItem(row, 1, new QTableWidgetItem(QString::number(val, 'f', 6)));
        param_table_->setItem(row, 2, new QTableWidgetItem(QString::number(grad, 'f', 6)));
        param_table_->setItem(row, 3, new QTableWidgetItem(QString::number(fd_val, 'f', 6)));
        double err = std::abs(grad - fd_val);
        param_table_->setItem(row, 4, new QTableWidgetItem(QString::number(err, 'e', 2)));
    };

    set_row(0, "w", p.w, g.dw, fd.dw);
    set_row(1, "b", p.b, g.db, fd.db);
    set_row(2, "v", p.v, g.dv, fd.dv);
    set_row(3, "c", p.c, g.dc, fd.dc);
}

void BackpropView::onForwardClicked() {
    updateForwardDisplay();
    updateParameterTable();
}

void BackpropView::onGradientsClicked() {
    updateForwardDisplay();
    updateParameterTable();
}

void BackpropView::onGradientCheckClicked() {
    auto check = model_.check_gradients(1e-6, 1e-5);
    current_check_ = check;
    updateForwardDisplay();
    updateParameterTable();

    if (check.passed) {
        check_status_label_->setText(
            QString("Check: <font color='#a6e3a1'>PASSED (Max Error: %1 <= 1e-5)</font>")
                .arg(check.max_absolute_error, 0, 'e', 2)
        );
    } else {
        check_status_label_->setText(
            QString("Check: <font color='#f38ba8'>FAILED (Max Error: %1 > 1e-5)</font>")
                .arg(check.max_absolute_error, 0, 'e', 2)
        );
    }
}

void BackpropView::onStepClicked() {
    model_.step(0.1);
    updateForwardDisplay();
    updateParameterTable();
}

void BackpropView::onTrainClicked() {
    // Fresh training run from current parameters
    current_trace_ = model_.train(100, 0.1);
    updateForwardDisplay();
    updateParameterTable();
    updateLossPlot(current_trace_);
}

void BackpropView::onResetClicked() {
    model_ = backprop::Model(1.0, 2.0, {0.5, 0.0, 1.0, 0.0});
    current_trace_.clear();
    loss_plot_->clear();
    check_status_label_->setText("Gradient check: Reset.");
    updateForwardDisplay();
    updateParameterTable();
}

void BackpropView::updateLossPlot(const std::vector<backprop::TrainingStepRecord>& trace) {
    loss_plot_->clear();

    PlotSeries s;
    s.title = "Training Loss J";
    s.color = QColor(166, 227, 161); // Green
    s.line_width = 2;
    s.show_points = false;

    for (const auto& rec : trace) {
        s.points.emplace_back(static_cast<double>(rec.step), rec.loss);
    }

    loss_plot_->addSeries(s);
}

void BackpropView::onExportTraceClicked() {
    if (current_trace_.empty()) {
        QMessageBox::information(this, "Export", "Please run a 100-step training trace first.");
        return;
    }
    QString path = QFileDialog::getSaveFileName(this, "Export Training Trace CSV", "backprop_loss_trace.csv", "CSV Files (*.csv)");
    if (!path.isEmpty()) {
        std::ofstream out(path.toStdString());
        out << backprop::format_training_trace_csv(current_trace_);
        QMessageBox::information(this, "Export Successful", "Training trace exported to:\n" + path);
    }
}

void BackpropView::onExportCheckClicked() {
    if (!current_check_.has_value()) {
        current_check_ = model_.check_gradients(1e-6, 1e-5);
    }
    QString path = QFileDialog::getSaveFileName(this, "Export Gradient Check CSV", "backprop_gradient_check.csv", "CSV Files (*.csv)");
    if (!path.isEmpty()) {
        std::ofstream out(path.toStdString());
        out << backprop::format_gradient_check_csv(*current_check_);
        QMessageBox::information(this, "Export Successful", "Gradient check exported to:\n" + path);
    }
}

} // namespace app
} // namespace workbench
