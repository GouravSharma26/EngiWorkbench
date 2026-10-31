#include "workbench/backprop/model.hpp"
#include <iostream>
#include <cmath>
#include <cstdlib>

int main() {
    int total = 0;
    int passed = 0;

    std::cout << "[Test Suite: Handwritten-style Backpropagation]" << std::endl;

    workbench::backprop::Model model(1.0, 2.0, {0.5, 0.0, 1.0, 0.0});

    // Test 1: Forward pass at initial parameters
    total++;
    auto f0 = model.forward();
    if (std::abs(f0.z - 0.5) < 1e-10 &&
        std::abs(f0.h - 0.46211715726000974) < 1e-10 &&
        std::abs(f0.y_hat - 0.46211715726000974) < 1e-10 &&
        std::abs(f0.delta - (-1.5378828427399902)) < 1e-10 &&
        std::abs(f0.loss - 1.1825418189970167) < 1e-10) {
        std::cout << "  [PASS] Test 1: Forward pass matches reference values exactly (J=1.182542, y_hat=0.462117)" << std::endl;
        passed++;
    } else {
        std::cerr << "  [FAIL] Test 1: Forward pass values mismatch: loss=" << f0.loss << ", y_hat=" << f0.y_hat << std::endl;
    }

    // Test 2: Initial analytical gradients
    total++;
    auto g0 = model.compute_gradients();
    if (std::abs(g0.dw - (-1.2094644752400612)) < 1e-10 &&
        std::abs(g0.db - (-1.2094644752400612)) < 1e-10 &&
        std::abs(g0.dv - (-0.7106820474859469)) < 1e-10 &&
        std::abs(g0.dc - (-1.5378828427399902)) < 1e-10) {
        std::cout << "  [PASS] Test 2: Analytical gradients match reference values (dw=-1.209464, dv=-0.710682, dc=-1.537883)" << std::endl;
        passed++;
    } else {
        std::cerr << "  [FAIL] Test 2: Analytical gradients mismatch: dw=" << g0.dw << ", dv=" << g0.dv << std::endl;
    }

    // Test 3: Central finite difference check (epsilon=1e-6, threshold <= 1e-5)
    total++;
    auto check = model.check_gradients(1e-6, 1e-5);
    if (check.passed) {
        std::cout << "  [PASS] Test 3: Central finite differences match analytical gradients within <= 1e-5 (Max error: "
                  << check.max_absolute_error << ")" << std::endl;
        passed++;
    } else {
        std::cerr << "  [FAIL] Test 3: Gradient check failed! Max error: " << check.max_absolute_error << std::endl;
    }

    // Test 4: Simultaneous parameter update after one step (lr = 0.1)
    total++;
    model.step(0.1);
    auto p1 = model.parameters();
    auto f1 = model.forward();
    if (std::abs(p1.w - 0.6209464475240061) < 1e-10 &&
        std::abs(p1.b - 0.12094644752400613) < 1e-10 &&
        std::abs(p1.v - 1.0710682047485947) < 1e-10 &&
        std::abs(p1.c - 0.15378828427399904) < 1e-10 &&
        std::abs(f1.loss - 0.6857737650448423) < 1e-8) {
        std::cout << "  [PASS] Test 4: One-step simultaneous update matches reference parameters (J_1=0.685774)" << std::endl;
        passed++;
    } else {
        std::cerr << "  [FAIL] Test 4: Updated parameter values mismatch" << std::endl;
    }

    // Test 5: 100-step training trace (101 points, monotonic loss reduction)
    total++;
    workbench::backprop::Model train_model(1.0, 2.0, {0.5, 0.0, 1.0, 0.0});
    auto trace = train_model.train(100, 0.1);
    bool trace_ok = (trace.size() == 101);
    if (trace_ok) {
        // Verify loss monotonically decreases
        for (size_t i = 1; i < trace.size(); ++i) {
            if (trace[i].loss > trace[i - 1].loss) {
                trace_ok = false;
                std::cerr << "  [FAIL] Step " << i << " loss increased: " << trace[i].loss << " > " << trace[i-1].loss << std::endl;
                break;
            }
        }
        // Verify final prediction is close to target 2.0 and loss is small
        if (trace.back().loss > 0.01 || std::abs(trace.back().y_hat - 2.0) > 0.15) {
            trace_ok = false;
        }
    }

    if (trace_ok) {
        std::cout << "  [PASS] Test 5: 100-step training trace converges monotonically (J_0=1.1825 -> J_100=" 
                  << trace.back().loss << ", y_hat=" << trace.back().y_hat << ")" << std::endl;
        passed++;
    } else {
        std::cerr << "  [FAIL] Test 5: Training trace failed convergence check" << std::endl;
    }

    // Test 6: CSV formatting of training trace and gradient check
    total++;
    auto trace_csv = workbench::backprop::format_training_trace_csv(trace);
    auto check_csv = workbench::backprop::format_gradient_check_csv(check);
    if (!trace_csv.empty() && !check_csv.empty() && 
        trace_csv.find("step,loss") != std::string::npos &&
        check_csv.find("analytical_gradient") != std::string::npos) {
        std::cout << "  [PASS] Test 6: Training trace and gradient check CSV formatted correctly" << std::endl;
        passed++;
    } else {
        std::cerr << "  [FAIL] Test 6: CSV formatting failed" << std::endl;
    }

    std::cout << "[Test Suite: Handwritten-style Backpropagation] Passed " << passed << " of " << total << " tests." << std::endl;
    return (passed == total) ? EXIT_SUCCESS : EXIT_FAILURE;
}

