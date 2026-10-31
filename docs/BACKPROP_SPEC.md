# Task F — handwritten backpropagation and C++ implementation

## 1. Required single-sample model

- Input: `x = 1`
- Target: `y = 2`
- Hidden activation: `h = tanh(w x + b)`
- Prediction: `y_hat = v h + c`
- Loss: `J = 0.5 (y_hat - y)^2`
- Initial parameters: `w=0.5`, `b=0`, `v=1`, `c=0`
- Learning rate: `η=0.1`

The candidate must handwrite the forward pass, chain-rule derivatives for all four parameters, numerical initial values and one simultaneous gradient-descent update. The scan/photo appendix must be legible and at most two pages.

## 2. Forward pass at initial values

\[
z=wx+b=0.5,\quad h=\tanh(0.5)=0.46211715726000974
\]

\[
\hat y=vh+c=0.46211715726000974
\]

\[
\delta=\hat y-y=-1.5378828427399902
\]

\[
J=\frac12\delta^2=1.1825418189970167
\]

## 3. Chain rule

Since `d(tanh(z))/dz = 1 - tanh²(z) = 1-h²`:

\[
\frac{\partial J}{\partial \hat y}=\delta,
\quad \frac{\partial \hat y}{\partial v}=h,
\quad \frac{\partial \hat y}{\partial c}=1
\]

\[
\frac{\partial h}{\partial z}=1-h^2,
\quad \frac{\partial z}{\partial w}=x,
\quad \frac{\partial z}{\partial b}=1
\]

Therefore:

\[
\boxed{\frac{\partial J}{\partial v}=\delta h}
\qquad
\boxed{\frac{\partial J}{\partial c}=\delta}
\]

\[
\boxed{\frac{\partial J}{\partial w}=\delta v(1-h^2)x}
\qquad
\boxed{\frac{\partial J}{\partial b}=\delta v(1-h^2)}
\]

## 4. Initial gradients and one simultaneous update

| Parameter | Initial gradient |
|---|---:|
| `w` | -1.2094644752400612 |
| `b` | -1.2094644752400612 |
| `v` | -0.7106820474859469 |
| `c` | -1.5378828427399902 |

Use all four gradients calculated from the original parameter set, then update together:

\[
\theta_{new}=\theta_{old}-\eta\nabla_\theta J
\]

| Parameter | Initial | Updated after one step |
|---|---:|---:|
| `w` | 0.5 | 0.6209464475240061 |
| `b` | 0 | 0.12094644752400613 |
| `v` | 1 | 1.0710682047485947 |
| `c` | 0 | 0.15378828427399904 |

Reference loss after this first update: approximately `0.6857737650448423`.

All gradients are negative initially, so subtracting `η × gradient` increases each parameter. Explain that this moves the prediction toward the larger target value for this particular initial state; do not claim that every parameter must always increase.

## 5. C++ implementation requirements

- Hand-code `tanh`, prediction, loss and all four derivatives. No automatic differentiation.
- Compute all gradients into a separate immutable gradient structure before mutating parameters.
- Implement central finite differences for each parameter:

\[
g_i^{FD}=\frac{J(\theta+\epsilon e_i)-J(\theta-\epsilon e_i)}{2\epsilon},
\quad \epsilon=10^{-6}.
\]

- Require absolute difference between analytic and finite-difference gradient `<= 1e-5` for all four initial parameters.
- Run 100 subsequent training updates with learning rate 0.1, record loss at step 0 and after each update (101 rows total), and plot loss vs. step.
- Ensure all parameters, prediction, gradients and loss remain finite.

## 6. Reference finite-difference values

With central difference epsilon `1e-6`, a representative double-precision result is:

| Parameter | Analytic gradient | Central finite difference (reference) |
|---|---:|---:|
| `w` | -1.2094644752400612 | -1.2094644753490869 |
| `b` | -1.2094644752400612 | -1.2094644753490869 |
| `v` | -0.7106820474859469 | -0.7106820474334086 |
| `c` | -1.5378828427399902 | -1.5378828426282354 |

Small final-digit differences across standard libraries/platforms are expected; the required absolute tolerance is `1e-5`.

## 7. Outputs and explanation

- `backprop_gradients.csv`: parameter, analytic gradient, finite-difference gradient, absolute difference.
- `backprop_loss.csv`: step 0–100 and loss.
- `backprop_loss.png`: loss curve with axis labels.
- Handwritten derivation scan/photo, max two pages, stored under the submission appendix path.

Explain the chain rule, why the tanh derivative is `1-h²`, why all gradients must use the same original parameter set for a simultaneous update, and what the decreasing loss shows. The reference implementation's 100-step final loss is approximately `1.7e-20`; do not hard-code this as a pass/fail criterion if harmless platform rounding differs. The key checks are correct gradients, finite values and a sensible decreasing loss history.
