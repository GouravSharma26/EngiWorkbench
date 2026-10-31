#pragma once

#include "workbench/domain/thermal.hpp"
#include <vector>
#include <string>
#include <atomic>
#include <optional>

namespace workbench {
namespace performance {

struct BenchmarkConfig {
    unsigned int seed{42};
    size_t case_count{1000};
    int element_count{50};
    double min_conductivity_W_mK{5.0};
    double max_conductivity_W_mK{25.0};
    double min_heat_generation_W_m3{1000.0};
    double max_heat_generation_W_m3{20000.0};
    double left_temperature_C{100.0};
    double right_temperature_C{20.0};
    double length_m{1.0};
    double area_m2{1e-4};
};

struct TimingRecord {
    size_t worker_count{1};
    int repetition{1};
    double elapsed_ms{0.0};
};

struct BenchmarkSummary {
    size_t worker_count{1};
    double median_ms{0.0};
    double speedup{1.0};
    double efficiency{1.0};
};

struct AgreementResult {
    bool passed{false};
    double max_abs_difference_C{0.0};
    double threshold_C{1e-8};
    size_t evaluated_cases{0};
    size_t evaluated_nodes{0};
};

/// Generates a deterministic vector of thermal cases using std::mt19937 and the given seed.
std::vector<domain::ThermalProblem> generate_benchmark_cases(const BenchmarkConfig& config);

/// Solves a batch of thermal cases sequentially in the calling thread.
/// Stores outputs in deterministic input order: results[i] corresponds to cases[i].
std::vector<domain::ThermalSolution> solve_batch_serial(
    const std::vector<domain::ThermalProblem>& cases,
    const std::atomic<bool>* cancel_token = nullptr
);

/// Solves a batch of thermal cases in parallel across a bounded ThreadPool of worker_count threads.
/// Stores outputs in deterministic input order: results[i] corresponds to cases[i].
std::vector<domain::ThermalSolution> solve_batch_parallel(
    const std::vector<domain::ThermalProblem>& cases,
    size_t worker_count,
    const std::atomic<bool>* cancel_token = nullptr
);

/// Compares serial vs parallel solutions and validates max |T_serial - T_parallel| <= threshold_C (1e-8 C).
AgreementResult verify_agreement(
    const std::vector<domain::ThermalSolution>& serial_results,
    const std::vector<domain::ThermalSolution>& parallel_results,
    double threshold_C = 1e-8
);

/// Full benchmark execution: 1 warm-up, 5 measured repetitions per worker count, computing median/speedup/efficiency.
struct BenchmarkSuiteResult {
    std::vector<TimingRecord> raw_timings;
    std::vector<BenchmarkSummary> summaries;
    AgreementResult agreement;
};

BenchmarkSuiteResult run_benchmark(
    const std::vector<domain::ThermalProblem>& cases,
    const std::vector<size_t>& worker_counts = {1, 2, 4},
    const std::atomic<bool>* cancel_token = nullptr
);

std::string format_benchmark_cases_csv(const std::vector<domain::ThermalProblem>& cases);
std::string format_raw_timings_csv(const std::vector<TimingRecord>& timings);
std::string format_benchmark_summary_csv(const std::vector<BenchmarkSummary>& summaries);
std::string format_agreement_csv(const AgreementResult& agreement);

} // namespace performance
} // namespace workbench

