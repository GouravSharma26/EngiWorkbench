#include "workbench/performance/thread_pool.hpp"
#include "workbench/performance/benchmark.hpp"
#include <iostream>
#include <cmath>
#include <cstdlib>

int main() {
    int total = 0;
    int passed = 0;

    std::cout << "[Test Suite: Multicore Performance Benchmark]" << std::endl;

    // Test 1: Seeded generator repeatability and valid parameter bounds
    total++;
    workbench::performance::BenchmarkConfig config;
    config.seed = 42;
    config.case_count = 50;
    config.element_count = 20;

    auto cases_1 = workbench::performance::generate_benchmark_cases(config);
    auto cases_2 = workbench::performance::generate_benchmark_cases(config);

    bool gen_ok = (cases_1.size() == 50 && cases_2.size() == 50);
    for (size_t i = 0; i < cases_1.size() && gen_ok; ++i) {
        if (cases_1[i].conductivity_W_mK != cases_2[i].conductivity_W_mK ||
            cases_1[i].heat_generation_W_m3 != cases_2[i].heat_generation_W_m3) {
            gen_ok = false;
        }
        if (cases_1[i].conductivity_W_mK < 5.0 || cases_1[i].conductivity_W_mK > 25.0 ||
            cases_1[i].heat_generation_W_m3 < 1000.0 || cases_1[i].heat_generation_W_m3 > 20000.0) {
            gen_ok = false;
        }
    }

    if (gen_ok) {
        std::cout << "  [PASS] Test 1: Seeded case generator is deterministic and enforces physical bounds" << std::endl;
        passed++;
    } else {
        std::cerr << "  [FAIL] Test 1: Generator non-deterministic or bounds violated" << std::endl;
    }

    // Test 2: Serial vs Parallel Agreement (<= 1e-8 C difference)
    total++;
    auto serial_res = workbench::performance::solve_batch_serial(cases_1);
    auto parallel_res_2 = workbench::performance::solve_batch_parallel(cases_1, 2);
    auto parallel_res_4 = workbench::performance::solve_batch_parallel(cases_1, 4);

    auto agree_2 = workbench::performance::verify_agreement(serial_res, parallel_res_2, 1e-8);
    auto agree_4 = workbench::performance::verify_agreement(serial_res, parallel_res_4, 1e-8);

    if (agree_2.passed && agree_4.passed) {
        std::cout << "  [PASS] Test 2: Serial vs parallel outputs agree within <= 1e-8 C (Max diff: "
                  << std::max(agree_2.max_abs_difference_C, agree_4.max_abs_difference_C) << " C)" << std::endl;
        passed++;
    } else {
        std::cerr << "  [FAIL] Test 2: Agreement check failed! Diff2: " 
                  << agree_2.max_abs_difference_C << ", Diff4: " << agree_4.max_abs_difference_C << std::endl;
    }

    // Test 3: Deterministic Input-Order Indexing
    total++;
    bool order_ok = (serial_res.size() == cases_1.size() && parallel_res_4.size() == cases_1.size());
    for (size_t i = 0; i < cases_1.size() && order_ok; ++i) {
        // First node is T_left, last node is T_right, middle temperature varies deterministically with k and q
        double T_s = serial_res[i].nodes[10].temperature_FE_C;
        double T_p = parallel_res_4[i].nodes[10].temperature_FE_C;
        if (std::abs(T_s - T_p) > 1e-8) {
            order_ok = false;
        }
    }
    if (order_ok) {
        std::cout << "  [PASS] Test 3: Result indexing strictly preserves deterministic input case order" << std::endl;
        passed++;
    } else {
        std::cerr << "  [FAIL] Test 3: Result indexing mismatch between serial and parallel" << std::endl;
    }

    // Test 4: Cancellation safety
    total++;
    std::atomic<bool> cancelled{true};
    auto cancel_res = workbench::performance::solve_batch_parallel(cases_1, 2, &cancelled);
    if (cancel_res.empty()) {
        std::cout << "  [PASS] Test 4: Cancellation safely halts execution and returns empty vector" << std::endl;
        passed++;
    } else {
        std::cerr << "  [FAIL] Test 4: Cancellation did not return empty result" << std::endl;
    }

    // Test 5: Benchmark suite execution (warm-up + 5 repetitions + speedup / efficiency)
    total++;
    auto suite = workbench::performance::run_benchmark(cases_1, {1, 2});
    bool suite_ok = (suite.raw_timings.size() == 10); // 2 worker configs * 5 reps = 10 timings
    if (suite.summaries.size() == 2 && suite.agreement.passed) {
        if (suite.summaries[0].worker_count == 1 && suite.summaries[0].speedup == 1.0 &&
            suite.summaries[1].worker_count == 2 && suite.summaries[1].speedup > 0.0) {
            std::cout << "  [PASS] Test 5: Benchmark suite (1 warm-up, 5 reps, median, speedup, efficiency) completed successfully" << std::endl;
            passed++;
        } else {
            std::cerr << "  [FAIL] Test 5: Benchmark summary values incorrect" << std::endl;
        }
    } else {
        std::cerr << "  [FAIL] Test 5: Suite failed to record expected timings or agreement" << std::endl;
    }

    std::cout << "[Test Suite: Multicore Performance Benchmark] Passed " << passed << " of " << total << " tests." << std::endl;
    return (passed == total) ? EXIT_SUCCESS : EXIT_FAILURE;
}
