#include "workbench/backprop/model.hpp"
#include <cmath>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace workbench {
namespace backprop {

Model::Model(double x, double y, Parameters initial_params)
    : x_(x), y_(y), params_(initial_params) {}

ForwardOutput Model::forward() const {
    return forward(params_);
}

ForwardOutput Model::forward(const Parameters& params) const {
    ForwardOutput out;
    out.z = params.w * x_ + params.b;
    out.h = std::tanh(out.z);
    out.y_hat = params.v * out.h + params.c;
    out.delta = out.y_hat - y_;
    out.loss = 0.5 * out.delta * out.delta;
    return out;
}

Gradients Model::compute_gradients() const {
    ForwardOutput out = forward();

    // dJ/dc = delta
    // dJ/dv = delta * h
    // d(tanh(z))/dz = 1 - h^2
    // dJ/dw = delta * v * (1 - h^2) * x
    // dJ/db = delta * v * (1 - h^2)
    double sech2 = 1.0 - out.h * out.h;

    Gradients g;
    g.dc = out.delta;
    g.dv = out.delta * out.h;
    g.dw = out.delta * params_.v * sech2 * x_;
    g.db = out.delta * params_.v * sech2;

    return g;
}

Gradients Model::compute_finite_differences(double epsilon) const {
    Gradients g;

    // Check w
    Parameters pw_plus = params_; pw_plus.w += epsilon;
    Parameters pw_minus = params_; pw_minus.w -= epsilon;
    g.dw = (forward(pw_plus).loss - forward(pw_minus).loss) / (2.0 * epsilon);

    // Check b
    Parameters pb_plus = params_; pb_plus.b += epsilon;
    Parameters pb_minus = params_; pb_minus.b -= epsilon;
    g.db = (forward(pb_plus).loss - forward(pb_minus).loss) / (2.0 * epsilon);

    // Check v
    Parameters pv_plus = params_; pv_plus.v += epsilon;
    Parameters pv_minus = params_; pv_minus.v -= epsilon;
    g.dv = (forward(pv_plus).loss - forward(pv_minus).loss) / (2.0 * epsilon);

    // Check c
    Parameters pc_plus = params_; pc_plus.c += epsilon;
    Parameters pc_minus = params_; pc_minus.c -= epsilon;
    g.dc = (forward(pc_plus).loss - forward(pc_minus).loss) / (2.0 * epsilon);

    return g;
}

GradientCheckResult Model::check_gradients(double epsilon, double threshold) const {
    GradientCheckResult res;
    res.epsilon = epsilon;
    res.threshold = threshold;
    res.analytical = compute_gradients();
    res.finite_difference = compute_finite_differences(epsilon);

    res.absolute_error.dw = std::abs(res.analytical.dw - res.finite_difference.dw);
    res.absolute_error.db = std::abs(res.analytical.db - res.finite_difference.db);
    res.absolute_error.dv = std::abs(res.analytical.dv - res.finite_difference.dv);
    res.absolute_error.dc = std::abs(res.analytical.dc - res.finite_difference.dc);

    res.max_absolute_error = std::max({
        res.absolute_error.dw,
        res.absolute_error.db,
        res.absolute_error.dv,
        res.absolute_error.dc
    });

    res.passed = (res.max_absolute_error <= threshold);
    return res;
}

void Model::step(double learning_rate) {
    // Crucial: compute ALL gradients from original parameter values before updating
    Gradients g = compute_gradients();

    params_.w -= learning_rate * g.dw;
    params_.b -= learning_rate * g.db;
    params_.v -= learning_rate * g.dv;
    params_.c -= learning_rate * g.dc;
}

std::vector<TrainingStepRecord> Model::train(int num_steps, double learning_rate) {
    std::vector<TrainingStepRecord> trace;
    trace.reserve(num_steps + 1);

    // Record step 0 (initial state before updates)
    auto out0 = forward();
    auto g0 = compute_gradients();
    trace.push_back({0, out0.loss, out0.y_hat, params_, g0});

    for (int s = 1; s <= num_steps; ++s) {
        step(learning_rate);
        auto out = forward();
        auto g = compute_gradients();
        trace.push_back({s, out.loss, out.y_hat, params_, g});
    }

    return trace;
}

std::string format_training_trace_csv(const std::vector<TrainingStepRecord>& trace) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(8);
    oss << "step,loss,y_hat,w,b,v,c,dw,db,dv,dc\n";
    for (const auto& rec : trace) {
        oss << rec.step << ","
            << rec.loss << ","
            << rec.y_hat << ","
            << rec.parameters.w << ","
            << rec.parameters.b << ","
            << rec.parameters.v << ","
            << rec.parameters.c << ","
            << rec.gradients.dw << ","
            << rec.gradients.db << ","
            << rec.gradients.dv << ","
            << rec.gradients.dc << "\n";
    }
    return oss.str();
}

std::string format_gradient_check_csv(const GradientCheckResult& check) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(10);
    oss << "parameter,analytical_gradient,finite_difference_gradient,absolute_error,threshold,passed\n";
    auto add_row = [&](const char* name, double ana, double fd, double err) {
        oss << name << "," << ana << "," << fd << "," << err << ","
            << check.threshold << "," << (err <= check.threshold ? "true" : "false") << "\n";
    };
    add_row("w", check.analytical.dw, check.finite_difference.dw, check.absolute_error.dw);
    add_row("b", check.analytical.db, check.finite_difference.db, check.absolute_error.db);
    add_row("v", check.analytical.dv, check.finite_difference.dv, check.absolute_error.dv);
    add_row("c", check.analytical.dc, check.finite_difference.dc, check.absolute_error.dc);
    return oss.str();
}

} // namespace backprop
} // namespace workbench

