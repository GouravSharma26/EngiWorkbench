#include "workbench/views/performance_view.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <fstream>

namespace workbench {
namespace app {

PerformanceView::PerformanceView(QWidget* parent)
    : QWidget(parent) {
    setupUi();

    // Register meta types for thread signal
    qRegisterMetaType<workbench::performance::BenchmarkSuiteResult>("workbench::performance::BenchmarkSuiteResult");
    connect(this, &PerformanceView::benchmarkCompleted, this, &PerformanceView::onBenchmarkCompleted);
    connect(this, &PerformanceView::benchmarkCancelled, this, &PerformanceView::onBenchmarkCancelled);
    connect(this, &PerformanceView::benchmarkFailed, this, &PerformanceView::onBenchmarkFailed);
}

PerformanceView::~PerformanceView() {
    cancel_flag_.store(true);
    if (worker_thread_.joinable()) {
        worker_thread_.join();
    }
}

void PerformanceView::setupUi() {
    auto* main_layout = new QHBoxLayout(this);
    main_layout->setContentsMargins(12, 12, 12, 12);
    main_layout->setSpacing(12);

    // Left controls column (340px)
    auto* sidebar = new QWidget(this);
    sidebar->setFixedWidth(340);
    auto* side_layout = new QVBoxLayout(sidebar);
    side_layout->setContentsMargins(0, 0, 0, 0);
    side_layout->setSpacing(10);

    // 1. Benchmark Workload Config
    auto* cfg_group = new QGroupBox("Reproducible Workload Configuration", sidebar);
    auto* form = new QFormLayout(cfg_group);

    seed_spin_ = new QSpinBox(cfg_group);
    seed_spin_->setRange(1, 999999);
    seed_spin_->setValue(42);

    cases_spin_ = new QSpinBox(cfg_group);
    cases_spin_->setRange(10, 50000);
    cases_spin_->setValue(1000);

    elements_spin_ = new QSpinBox(cfg_group);
    elements_spin_->setRange(10, 500);
    elements_spin_->setValue(50);

    form->addRow("Fixed RNG Seed:", seed_spin_);
    form->addRow("Case Count:", cases_spin_);
    form->addRow("Elements / Case:", elements_spin_);

    auto* workers_layout = new QHBoxLayout();
    worker_1_check_ = new QCheckBox("1 Worker", cfg_group); worker_1_check_->setChecked(true);
    worker_2_check_ = new QCheckBox("2 Workers", cfg_group); worker_2_check_->setChecked(true);
    worker_4_check_ = new QCheckBox("4 Workers", cfg_group); worker_4_check_->setChecked(true);
    workers_layout->addWidget(worker_1_check_);
    workers_layout->addWidget(worker_2_check_);
    workers_layout->addWidget(worker_4_check_);
    form->addRow("Worker Pool:", workers_layout);

    side_layout->addWidget(cfg_group);

    // 2. Action Buttons & Progress
    auto* act_group = new QGroupBox("Execution & State Controller", sidebar);
    auto* act_layout = new QVBoxLayout(act_group);

    auto* btn_row = new QHBoxLayout();
    run_btn_ = new QPushButton("Run Benchmark", act_group);
    run_btn_->setStyleSheet("background-color: #2b3145; color: #a6e3a1; font-weight: bold;");
    cancel_btn_ = new QPushButton("Cancel", act_group);
    cancel_btn_->setEnabled(false);
    cancel_btn_->setStyleSheet("background-color: #2b3145; color: #f38ba8;");
    btn_row->addWidget(run_btn_);
    btn_row->addWidget(cancel_btn_);

    progress_bar_ = new QProgressBar(act_group);
    progress_bar_->setRange(0, 0); // Indeterminate during execution
    progress_bar_->setVisible(false);

    status_label_ = new QLabel("Status: Idle", act_group);
    status_label_->setStyleSheet("color: #bac2de; font-size: 11px;");

    agreement_label_ = new QLabel("Agreement (Serial vs Parallel): Pending", act_group);
    agreement_label_->setStyleSheet("color: #fab387; font-size: 11px; font-weight: bold;");

    act_layout->addLayout(btn_row);
    act_layout->addWidget(progress_bar_);
    act_layout->addWidget(status_label_);
    act_layout->addWidget(agreement_label_);
    side_layout->addWidget(act_group);

    // 3. Export Group
    auto* exp_group = new QGroupBox("Export Benchmark Data", sidebar);
    auto* exp_layout = new QVBoxLayout(exp_group);
    auto* exp_cases_btn = new QPushButton("Export Generated Cases CSV...", exp_group);
    auto* exp_raw_btn = new QPushButton("Export Raw Timings (5 Reps) CSV...", exp_group);
    auto* exp_summary_btn = new QPushButton("Export Summary CSV...", exp_group);
    auto* exp_agree_btn = new QPushButton("Export Agreement CSV...", exp_group);

    exp_layout->addWidget(exp_cases_btn);
    exp_layout->addWidget(exp_raw_btn);
    exp_layout->addWidget(exp_summary_btn);
    exp_layout->addWidget(exp_agree_btn);
    side_layout->addWidget(exp_group);

    side_layout->addStretch();

    // Right Area: Tables and Plot
    auto* right_widget = new QWidget(this);
    auto* right_layout = new QVBoxLayout(right_widget);
    right_layout->setContentsMargins(0, 0, 0, 0);
    right_layout->setSpacing(10);

    // Tables Row
    auto* tables_row = new QHBoxLayout();
    
    // Summary Table
    summary_table_ = new QTableWidget(3, 4, right_widget);
    summary_table_->setHorizontalHeaderLabels({"Workers (p)", "Median [ms]", "Speedup (T1/Tp)", "Efficiency"});
    summary_table_->verticalHeader()->setVisible(false);
    summary_table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    summary_table_->setFixedHeight(130);
    tables_row->addWidget(summary_table_, 1);

    // Raw Timings Table
    raw_table_ = new QTableWidget(5, 3, right_widget);
    raw_table_->setHorizontalHeaderLabels({"Workers", "Repetition", "Time [ms]"});
    raw_table_->verticalHeader()->setVisible(false);
    raw_table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    raw_table_->setFixedHeight(130);
    tables_row->addWidget(raw_table_, 1);

    right_layout->addLayout(tables_row);

    // Speedup Plot
    speedup_plot_ = new PlotWidget(right_widget);
    speedup_plot_->setTitles("Multicore Speedup: Measured vs Ideal Scaling", "Worker Threads (p)", "Speedup Factor S_p = T_1 / T_p");
    right_layout->addWidget(speedup_plot_, 1);

    main_layout->addWidget(sidebar);
    main_layout->addWidget(right_widget, 1);

    // Connect user interactions
    connect(run_btn_, &QPushButton::clicked, this, &PerformanceView::onRunBenchmarkClicked);
    connect(cancel_btn_, &QPushButton::clicked, this, &PerformanceView::onCancelClicked);
    connect(exp_cases_btn, &QPushButton::clicked, this, &PerformanceView::onExportCasesClicked);
    connect(exp_raw_btn, &QPushButton::clicked, this, &PerformanceView::onExportRawTimingsClicked);
    connect(exp_summary_btn, &QPushButton::clicked, this, &PerformanceView::onExportSummaryClicked);
    connect(exp_agree_btn, &QPushButton::clicked, this, &PerformanceView::onExportAgreementClicked);
}

void PerformanceView::onRunBenchmarkClicked() {
    if (worker_thread_.joinable()) {
        worker_thread_.join();
    }

    std::vector<size_t> workers;
    if (worker_1_check_->isChecked()) workers.push_back(1);
    if (worker_2_check_->isChecked()) workers.push_back(2);
    if (worker_4_check_->isChecked()) workers.push_back(4);

    if (workers.empty()) {
        QMessageBox::warning(this, "Configuration", "Please select at least one worker count.");
        return;
    }

    performance::BenchmarkConfig config;
    config.seed = static_cast<unsigned int>(seed_spin_->value());
    config.case_count = static_cast<size_t>(cases_spin_->value());
    config.element_count = elements_spin_->value();

    current_cases_ = performance::generate_benchmark_cases(config);

    cancel_flag_.store(false);
    run_btn_->setEnabled(false);
    cancel_btn_->setEnabled(true);
    progress_bar_->setVisible(true);
    status_label_->setText("Status: Running (1 warm-up + 5 measured reps per worker)...");
    agreement_label_->setText("Agreement: Verifying...");

    worker_thread_ = std::thread([this, cases = current_cases_, workers]() {
        try {
            auto suite = performance::run_benchmark(cases, workers, &this->cancel_flag_);
            if (this->cancel_flag_.load()) {
                emit this->benchmarkCancelled();
            } else {
                emit this->benchmarkCompleted(suite);
            }
        } catch (const std::exception& ex) {
            emit this->benchmarkFailed(QString::fromStdString(ex.what()));
        }
    });
}

void PerformanceView::onCancelClicked() {
    cancel_flag_.store(true);
    status_label_->setText("Status: Cancelling...");
    cancel_btn_->setEnabled(false);
}

void PerformanceView::onBenchmarkCompleted(const workbench::performance::BenchmarkSuiteResult& result) {
    run_btn_->setEnabled(true);
    cancel_btn_->setEnabled(false);
    progress_bar_->setVisible(false);
    status_label_->setText("Status: Completed successfully.");

    current_suite_ = result;

    // Populate summary table
    summary_table_->setRowCount(static_cast<int>(result.summaries.size()));
    for (size_t i = 0; i < result.summaries.size(); ++i) {
        const auto& s = result.summaries[i];
        summary_table_->setItem(static_cast<int>(i), 0, new QTableWidgetItem(QString::number(s.worker_count)));
        summary_table_->setItem(static_cast<int>(i), 1, new QTableWidgetItem(QString::number(s.median_ms, 'f', 2)));
        summary_table_->setItem(static_cast<int>(i), 2, new QTableWidgetItem(QString::number(s.speedup, 'f', 2) + "x"));
        summary_table_->setItem(static_cast<int>(i), 3, new QTableWidgetItem(QString::number(s.efficiency * 100.0, 'f', 1) + "%"));
    }

    // Populate raw timings table
    raw_table_->setRowCount(static_cast<int>(result.raw_timings.size()));
    for (size_t i = 0; i < result.raw_timings.size(); ++i) {
        const auto& t = result.raw_timings[i];
        raw_table_->setItem(static_cast<int>(i), 0, new QTableWidgetItem(QString::number(t.worker_count)));
        raw_table_->setItem(static_cast<int>(i), 1, new QTableWidgetItem(QString("Rep %1").arg(t.repetition)));
        raw_table_->setItem(static_cast<int>(i), 2, new QTableWidgetItem(QString::number(t.elapsed_ms, 'f', 2)));
    }

    // Agreement display
    if (result.agreement.passed) {
        agreement_label_->setText(
            QString("Agreement: <font color='#a6e3a1'>PASSED</font> (Max Diff: %1 °C <= 1e-8 °C)")
                .arg(result.agreement.max_abs_difference_C, 0, 'e', 2)
        );
    } else {
        agreement_label_->setText(
            QString("Agreement: <font color='#f38ba8'>FAILED</font> (Max Diff: %1 °C > 1e-8 °C)")
                .arg(result.agreement.max_abs_difference_C, 0, 'e', 2)
        );
    }

    updatePlots(result);
}

void PerformanceView::onBenchmarkCancelled() {
    run_btn_->setEnabled(true);
    cancel_btn_->setEnabled(false);
    progress_bar_->setVisible(false);
    status_label_->setText("Status: Cancelled. Previous complete results preserved.");
    agreement_label_->setText("Agreement: Run aborted.");
}

void PerformanceView::onBenchmarkFailed(const QString& error) {
    run_btn_->setEnabled(true);
    cancel_btn_->setEnabled(false);
    progress_bar_->setVisible(false);
    status_label_->setText("Status: Failed.");
    QMessageBox::critical(this, "Benchmark Error", error);
}

void PerformanceView::updatePlots(const workbench::performance::BenchmarkSuiteResult& suite) {
    speedup_plot_->clear();

    if (suite.summaries.empty()) return;

    // Ideal linear speedup line
    PlotSeries ideal_series;
    ideal_series.title = "Ideal Linear Speedup";
    ideal_series.color = QColor(108, 112, 134); // Grey/Muted
    ideal_series.line_width = 1;
    ideal_series.show_points = false;

    // Measured Speedup bar chart
    PlotSeries measured_series;
    measured_series.title = "Measured Speedup (T1/Tp)";
    measured_series.color = QColor(137, 180, 250); // Blue
    measured_series.is_bar_chart = true;

    for (const auto& s : suite.summaries) {
        ideal_series.points.emplace_back(static_cast<double>(s.worker_count), static_cast<double>(s.worker_count));
        measured_series.points.emplace_back(static_cast<double>(s.worker_count), s.speedup);
    }

    speedup_plot_->addSeries(ideal_series);
    speedup_plot_->addSeries(measured_series);
}

void PerformanceView::onExportCasesClicked() {
    if (current_cases_.empty()) {
        QMessageBox::information(this, "Export", "Please generate or run a benchmark first.");
        return;
    }
    QString path = QFileDialog::getSaveFileName(this, "Export Benchmark Cases CSV", "benchmark_cases.csv", "CSV Files (*.csv)");
    if (!path.isEmpty()) {
        std::ofstream out(path.toStdString());
        out << performance::format_benchmark_cases_csv(current_cases_);
        QMessageBox::information(this, "Export Successful", "Cases exported to:\n" + path);
    }
}

void PerformanceView::onExportRawTimingsClicked() {
    if (!current_suite_.has_value()) {
        QMessageBox::information(this, "Export", "Please run a benchmark first.");
        return;
    }
    QString path = QFileDialog::getSaveFileName(this, "Export Raw Timings CSV", "benchmark_raw_timings.csv", "CSV Files (*.csv)");
    if (!path.isEmpty()) {
        std::ofstream out(path.toStdString());
        out << performance::format_raw_timings_csv(current_suite_->raw_timings);
        QMessageBox::information(this, "Export Successful", "Raw timings exported to:\n" + path);
    }
}

void PerformanceView::onExportSummaryClicked() {
    if (!current_suite_.has_value()) {
        QMessageBox::information(this, "Export", "Please run a benchmark first.");
        return;
    }
    QString path = QFileDialog::getSaveFileName(this, "Export Benchmark Summary CSV", "benchmark_summary.csv", "CSV Files (*.csv)");
    if (!path.isEmpty()) {
        std::ofstream out(path.toStdString());
        out << performance::format_benchmark_summary_csv(current_suite_->summaries);
        QMessageBox::information(this, "Export Successful", "Benchmark summary exported to:\n" + path);
    }
}

void PerformanceView::onExportAgreementClicked() {
    if (!current_suite_.has_value()) {
        QMessageBox::information(this, "Export", "Please run a benchmark first.");
        return;
    }
    QString path = QFileDialog::getSaveFileName(this, "Export Agreement CSV", "benchmark_agreement.csv", "CSV Files (*.csv)");
    if (!path.isEmpty()) {
        std::ofstream out(path.toStdString());
        out << performance::format_agreement_csv(current_suite_->agreement);
        QMessageBox::information(this, "Export Successful", "Agreement results exported to:\n" + path);
    }
}

} // namespace app
} // namespace workbench
