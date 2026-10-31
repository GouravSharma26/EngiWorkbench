#include "workbench/performance/benchmark.hpp"
#include "workbench/performance/thread_pool.hpp"
#include "workbench/thermal/solver.hpp"
#include <random>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>

namespace workbench {
namespace performance {

std::vector<domain::ThermalProblem> generate_benchmark_cases(const BenchmarkConfig& config) {
    std::mt19937 rng(config.seed);
    std::uniform_real_distribution<double> dist_k(config.min_conductivity_W_mK, config.max_conductivity_W_mK);
    std::uniform_real_distribution<double> dist_q(config.min_heat_generation_W_m3, config.max_heat_generation_W_m3);

    std::vector<domain::ThermalProblem> cases;
    cases.reserve(config.case_count);

    for (size_t i = 0; i < config.case_count; ++i) {
        domain::ThermalProblem p;
        p.length_m = config.length_m;
        p.area_m2 = config.area_m2;
        p.conductivity_W_mK = dist_k(rng);
        p.heat_generation_W_m3 = dist_q(rng);
        p.left_temperature_C = config.left_temperature_C;
        p.right_temperature_C = config.right_temperature_C;
        p.element_count = config.element_count;
        cases.push_back(p);
    }

    return cases;
}

std::vector<domain::ThermalSolution> solve_batch_serial(
    const std::vector<domain::ThermalProblem>& cases,
    const std::atomic<bool>* cancel_token
) {
    std::vector<domain::ThermalSolution> results(cases.size());

    for (size_t i = 0; i < cases.size(); ++i) {
        if (cancel_token && cancel_token->load(std::memory_order_relaxed)) {
            return {}; // Cancellation returns empty to avoid partial publishing
        }
        results[i] = thermal::solve_thermal_1d(cases[i]);
    }

    return results;
}

std::vector<domain::ThermalSolution> solve_batch_parallel(
    const std::vector<domain::ThermalProblem>& cases,
    size_t worker_count,
    const std::atomic<bool>* cancel_token
) {
    std::vector<domain::ThermalSolution> results(cases.size());
    ThreadPool pool(worker_count);

    pool.parallel_for(0, cases.size(), [&](size_t i) {
        results[i] = thermal::solve_thermal_1d(cases[i]);
    }, cancel_token);

    if (cancel_token && cancel_token->load(std::memory_order_relaxed)) {
        return {}; // Cancellation returns empty to avoid partial publishing
    }

    return results;
}

AgreementResult verify_agreement(
    const std::vector<domain::ThermalSolution>& serial_results,
    const std::vector<domain::ThermalSolution>& parallel_results,
    double threshold_C
) {
    AgreementResult res;
    res.threshold_C = threshold_C;
    res.evaluated_cases = std::min(serial_results.size(), parallel_results.size());
    res.max_abs_difference_C = 0.0;
    res.passed = true;

    if (serial_results.size() != parallel_results.size() || serial_results.empty()) {
        res.passed = false;
        return res;
    }

    for (size_t c = 0; c < serial_results.size(); ++c) {
        const auto& s_nodes = serial_results[c].nodes;
        const auto& p_nodes = parallel_results[c].nodes;

        if (s_nodes.size() != p_nodes.size()) {
            res.passed = false;
            return res;
        }

        for (size_t n = 0; n < s_nodes.size(); ++n) {
            double diff = std::abs(s_nodes[n].temperature_FE_C - p_nodes[n].temperature_FE_C);
            res.max_abs_difference_C = std::max(res.max_abs_difference_C, diff);
            ++res.evaluated_nodes;
        }
    }

    res.passed = (res.max_abs_difference_C <= threshold_C);
    return res;
}

BenchmarkSuiteResult run_benchmark(
    const std::vector<domain::ThermalProblem>& cases,
    const std::vector<size_t>& worker_counts,
    const std::atomic<bool>* cancel_token
) {
    BenchmarkSuiteResult suite;

    if (cases.empty()) {
        return suite;
    }

    // Baseline serial run for agreement check
    auto serial_baseline = solve_batch_serial(cases, cancel_token);
    if (cancel_token && cancel_token->load(std::memory_order_relaxed)) {
        return suite;
    }

    std::vector<domain::ThermalSolution> latest_parallel;

    for (size_t p : worker_counts) {
        if (p < 1) continue;
        if (cancel_token && cancel_token->load(std::memory_order_relaxed)) {
            return suite;
        }

        // 1. Unmeasured warm-up
        if (p == 1) {
            solve_batch_serial(cases, cancel_token);
        } else {
            solve_batch_parallel(cases, p, cancel_token);
        }

        // 2. Five measured repetitions
        std::vector<double> run_times_ms;
        run_times_ms.reserve(5);

        for (int rep = 1; rep <= 5; ++rep) {
            if (cancel_token && cancel_token->load(std::memory_order_relaxed)) {
                return suite;
            }

            auto start = std::chrono::steady_clock::now();
            if (p == 1) {
                solve_batch_serial(cases, cancel_token);
            } else {
                latest_parallel = solve_batch_parallel(cases, p, cancel_token);
            }
            auto end = std::chrono::steady_clock::now();

            double elapsed_ms = std::chrono::duration<double, std::milli>(end - start).count();
            suite.raw_timings.push_back({p, rep, elapsed_ms});
            run_times_ms.push_back(elapsed_ms);
        }

        // Compute median time
        std::sort(run_times_ms.begin(), run_times_ms.end());
        double median_ms = run_times_ms[2]; // Median of 5 elements is the 3rd element (index 2)

        suite.summaries.push_back({p, median_ms, 1.0, 1.0});
    }

    // Compute speedup and efficiency relative to p = 1 baseline
    if (!suite.summaries.empty()) {
        double T1_median = suite.summaries.front().median_ms;
        for (auto& s : suite.summaries) {
            if (s.median_ms > 0.0) {
                s.speedup = T1_median / s.median_ms;
                s.efficiency = s.speedup / static_cast<double>(s.worker_count);
            }
        }
    }

    // Agreement check
    if (!latest_parallel.empty() && !serial_baseline.empty()) {
        suite.agreement = verify_agreement(serial_baseline, latest_parallel);
    } else if (!serial_baseline.empty()) {
        // Fallback for 1-worker suite
        suite.agreement = verify_agreement(serial_baseline, serial_baseline);
    }

    return suite;
}

std::string format_benchmark_cases_csv(const std::vector<domain::ThermalProblem>& cases) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6);
    oss << "case_id,L_m,A_m2,k_W_mK,q_W_m3,T_left_C,T_right_C,N\n";
    for (size_t i = 0; i < cases.size(); ++i) {
        const auto& c = cases[i];
        oss << i << "," << c.length_m << "," << c.area_m2 << ","
            << c.conductivity_W_mK << "," << c.heat_generation_W_m3 << ","
            << c.left_temperature_C << "," << c.right_temperature_C << ","
            << c.element_count << "\n";
    }
    return oss.str();
}

std::string format_raw_timings_csv(const std::vector<TimingRecord>& timings) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(4);
    oss << "worker_count,repetition,elapsed_ms\n";
    for (const auto& t : timings) {
        oss << t.worker_count << "," << t.repetition << "," << t.elapsed_ms << "\n";
    }
    return oss.str();
}

std::string format_benchmark_summary_csv(const std::vector<BenchmarkSummary>& summaries) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(4);
    oss << "worker_count,median_ms,speedup,efficiency\n";
    for (const auto& s : summaries) {
        oss << s.worker_count << "," << s.median_ms << "," << s.speedup << "," << s.efficiency << "\n";
    }
    return oss.str();
}

std::string format_agreement_csv(const AgreementResult& agreement) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(12);
    oss << "passed,max_abs_difference_C,threshold_C,evaluated_cases,evaluated_nodes\n";
    oss << (agreement.passed ? "true" : "false") << ","
        << agreement.max_abs_difference_C << ","
        << agreement.threshold_C << ","
        << agreement.evaluated_cases << ","
        << agreement.evaluated_nodes << "\n";
    return oss.str();
}

} // namespace performance
} // namespace workbench

