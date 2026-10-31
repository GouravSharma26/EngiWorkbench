#pragma once

#include <vector>
#include <string>

namespace workbench {
namespace backprop {

/// Network parameters for the one-sample model:
/// h = tanh(w * x + b)
/// y_hat = v * h + c
struct Parameters {
    double w{0.5};
    double b{0.0};
    double v{1.0};
    double c{0.0};
};

/// Analytical parameter gradients.
struct Gradients {
    double dw{0.0};
    double db{0.0};
    double dv{0.0};
    double dc{0.0};
};

/// Forward pass evaluation results.
struct ForwardOutput {
    double z{0.0};       // z = w*x + b
    double h{0.0};       // h = tanh(z)
    double y_hat{0.0};   // y_hat = v*h + c
    double delta{0.0};   // delta = y_hat - y
    double loss{0.0};    // J = 0.5 * delta^2
};

/// Numerical gradient check verification comparing analytical vs central finite differences.
struct GradientCheckResult {
    bool passed{false};
    double epsilon{1e-6};
    double max_absolute_error{0.0};
    double threshold{1e-5};
    Gradients analytical;
    Gradients finite_difference;
    Gradients absolute_error;
};

/// A single step in the training history trace.
struct TrainingStepRecord {
    int step{0};
    double loss{0.0};
    double y_hat{0.0};
    Parameters parameters;
    Gradients gradients;
};

/// Single-sample regression training model.
class Model {
public:
    Model(double x = 1.0, double y = 2.0, Parameters initial_params = Parameters{0.5, 0.0, 1.0, 0.0});

    /// Computes the forward pass.
    ForwardOutput forward() const;

    /// Computes forward pass with arbitrary parameters.
    ForwardOutput forward(const Parameters& params) const;

    /// Computes analytical gradients from current parameters.
    Gradients compute_gradients() const;

    /// Computes central finite differences for all parameters with step size epsilon.
    Gradients compute_finite_differences(double epsilon = 1e-6) const;

    /// Verifies analytical gradients against central finite differences.
    GradientCheckResult check_gradients(double epsilon = 1e-6, double threshold = 1e-5) const;

    /// Performs one simultaneous gradient descent update step: theta <- theta - learning_rate * grad.
    void step(double learning_rate = 0.1);

    /// Trains the model for num_steps and returns the complete history of (num_steps + 1) records.
    std::vector<TrainingStepRecord> train(int num_steps = 100, double learning_rate = 0.1);

    const Parameters& parameters() const noexcept {
        return params_;
    }

    double input_x() const noexcept {
        return x_;
    }

    double target_y() const noexcept {
        return y_;
    }

private:
    double x_{1.0};
    double y_{2.0};
    Parameters params_;
};

/// Formats the training history trace into CSV: "step,loss,y_hat,w,b,v,c,dw,db,dv,dc"
std::string format_training_trace_csv(const std::vector<TrainingStepRecord>& trace);

/// Formats gradient check results into CSV.
std::string format_gradient_check_csv(const GradientCheckResult& check);

} // namespace backprop
} // namespace workbench

