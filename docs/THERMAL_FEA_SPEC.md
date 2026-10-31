# Task C — 1D steady-state thermal finite-element specification

## 1. Problem definition

Solve the steady 1D rod problem:

\[
-\frac{d}{dx}\left(kA\frac{dT}{dx}\right)=qA, \quad 0<x<L
\]

with insulated lateral surfaces and prescribed end temperatures:

| Parameter | Required value | Unit |
|---|---:|---|
| Rod length `L` | 1 | m |
| Cross-sectional area `A` | `1e-4` | m² |
| Conductivity `k` | 10 | W/(m·K) |
| Volumetric heat generation `q` | 10,000 | W/m³ |
| Left temperature `T(0)` | 100 | °C |
| Right temperature `T(L)` | 20 | °C |
| Meshes | N = 10, 20, 40, 80 | uniform two-node elements |

Assume constant properties, no lateral heat loss, and uniform volumetric heat generation. The quantity `q` is volumetric heat generation in W/m³; the element source vector uses `q A`.

## 2. Element formulation

For a two-node linear element of length `h = L/N`, the element stiffness matrix is

\[
\mathbf{k}^{(e)} = \frac{kA}{h}
\begin{bmatrix}1&-1\\-1&1\end{bmatrix}
\]

and the consistent element load vector for uniform volumetric generation is

\[
\mathbf{f}^{(e)} = \frac{qAh}{2}
\begin{bmatrix}1\\1\end{bmatrix}.
\]

Assemble contributions into the global system `K T = F`. The matrix for a 1D two-node mesh is tridiagonal.

## 3. Correct Dirichlet treatment

Both end temperatures are prescribed. Remove the known end degrees of freedom and solve only for interior nodal temperatures:

\[
\mathbf K_{II}\mathbf T_I = \mathbf F_I - \mathbf K_{IB}\mathbf T_B.
\]

This is the preferred implementation because it naturally accounts for both known boundary values. Do not merely overwrite the first/last matrix rows while leaving inconsistent columns/RHS entries. A Thomas tridiagonal solver is suitable for the reduced system; guard against zero/non-finite pivots.

## 4. Analytical solution

For constant `k`, `A` cancels from the governing equation and the exact field is

\[
T_{exact}(x)=100-80\frac{x}{L}+\frac{q}{2k}x(L-x).
\]

For `L=1`, `k=10`, `q=10,000`:

\[
T_{exact}(x)=100-80x+500x(1-x).
\]

The end values must be 100 °C and 20 °C. The maximum occurs at x = 0.42 m and is 188.2 °C (useful as a sanity check).

## 5. Required midpoint error

Evaluate the FE field by **linear interpolation inside each element**, not by comparing exact values only at nodes. At each element midpoint:

\[
T_{FE}(x_m)=\frac{T_i+T_{i+1}}{2},
\quad e_m=|T_{FE}(x_m)-T_{exact}(x_m)|.
\]

For this constant-source, uniform mesh case, the theoretical maximum midpoint error is

\[
e_{max,mid}=\frac{q h^2}{8k}=\frac{125}{N^2}\;^\circ C.
\]

| Elements N | Element size h (m) | Expected max midpoint error (°C) | Expected error ratio vs previous mesh |
|---:|---:|---:|---:|
| 10 | 0.1 | 1.25 | — |
| 20 | 0.05 | 0.3125 | 4 |
| 40 | 0.025 | 0.078125 | 4 |
| 80 | 0.0125 | 0.01953125 | 4 |

The FE nodal values should agree with the exact solution at the mesh nodes up to floating-point/solver tolerance for this uniform constant-coefficient case. The midpoint errors are non-zero and should reduce by approximately four each time h is halved (second-order convergence).

## 6. Outputs and plots

Required exports:

- `thermal_nodes_N10.csv`, `thermal_nodes_N20.csv`, `thermal_nodes_N40.csv`, `thermal_nodes_N80.csv` with `node_id,x_m,T_FE_C,T_exact_C,abs_error_C`.
- `thermal_midpoint_errors.csv` with `N,element_id,x_mid_m,T_FE_interp_C,T_exact_C,abs_error_C`.
- `thermal_convergence.csv` with `N,h_m,max_abs_midpoint_error_C`.

Required plots:
1. FE temperature and exact temperature versus x, with x in m and T in °C. Show all mesh sizes or provide one curve per N.
2. Maximum midpoint absolute error versus element count or h; show units and mesh convergence.

## 7. Numerical tests

- Validate all required physical values are finite; `L`, `A`, `k` and N are positive; N is a positive integer; temperatures and q are finite.
- Check endpoint values match prescribed temperatures.
- Compare node values to `T_exact(x_i)` with a tight tolerance (document compiler/platform tolerance).
- Compare maximum midpoint errors with the table above; do not substitute a nodal error metric.
- Check midpoint error decreases by about a factor of four for each mesh refinement.
- Test zero heat generation and nonzero/negative finite q in the general solver if supported. For the assignment default, use positive q.
- Test invalid `k<=0`, `A<=0`, `L<=0`, NaN/Inf and cancellation.

## 8. Debugging insight to explain in report

A strong example is the Dirichlet boundary-condition issue: replacing a boundary row without eliminating the associated column contribution changes the interior right-hand side and can produce an incorrect temperature field. The reduced-system equation above is an auditable way to preserve both prescribed endpoint values.

## 9. Explanation to prepare for interview

Explain that the weak form turns the second-order differential equation into a matrix system; each linear element approximates T with shape functions; the stiffness matrix penalizes temperature gradients; the source vector represents generated heat; and finer meshes reduce interpolation error. State assumptions, units, and why insulated lateral surfaces mean no side-surface heat-loss term.
