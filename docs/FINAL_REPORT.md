# PASS Science Works — Unified Engineering Workbench
## Final Technical Report & Verification Summary

**Candidate:** Gourav Sharma  
**Assignment:** R&D Internship 2026–2027 Engineering Evaluation  
**Date:** 2026-10-31  
**Target Platform:** Windows 11 x64  
**Source Repository:** https://github.com/GouravSharma26/EngiWorkbench.git  

---

### Executive Summary

The **Unified Engineering Workbench** is an offline, modular desktop software workstation combining 3D geometry and point-cloud inspection, 1D steady-state thermal finite-element analysis, CPU-parallel batch computation, embedded-system adapter emulation, and handwritten-style neural network backpropagation.

All computational domain logic, numerical routines, file parsers, and multithreading are implemented strictly in standard **C++17**, keeping `src/core` 100% decoupled from Qt. The graphical interface is implemented using **Qt 6.8.2 Widgets** with custom vector plotting and OpenGL viewport integration. The entire system compiles cleanly under MSVC 2022 (v143) and Windows 11 SDK in both Release and Debug configurations, with all 6 automated test suites (33 individual test cases) passing at 100%. The application deploys as a standalone package launched via a native Win32 `Launch.exe` with zero installation or administrative prerequisites.

---

### 1. System Architecture & Modular Design

```
+--------------------------------------------------------------------------+
|                     Qt 6.8.2 Presentation Layer                          |
|  [Geometry View] [Thermal View] [Performance View] [Hardware] [Backprop] |
|       (Viewport3D)                  (PlotWidget 2D Vector Plots)         |
+--------------------------------------------------------------------------+
                                    |
                                    v (Pure C++ / Zero Qt Types)
+--------------------------------------------------------------------------+
|                          workbench_core (C++17)                          |
|  - Domain: ThermalProblem, ThermalSolution, TriangularMesh, PointCloud   |
|  - Solvers: 1D Steady-State Thermal FEA, Thomas Tridiagonal Algorithm   |
|  - Concurrency: ThreadPool, Parallel Batch Runner, Agreement Verifier    |
|  - I/O: Defensive ASCII STL Parser, ANSYS-style CSV Validator/Parser     |
|  - Embedded: IDeviceAdapter, ESP32 MCU, Arty A7 FPGA, JTAG Bridge Mocks  |
|  - Backpropagation: One-Sample Tanh NN, Analytical & Finite-Diff Grads   |
+--------------------------------------------------------------------------+
```

Key Architectural Principles:
1. **Core Independence:** `workbench_core` links zero GUI or external libraries; all numerical and file operations use ISO C++17 standard facilities (`<vector>`, `<cmath>`, `<thread>`, `<mutex>`, `<condition_variable>`).
2. **Asynchronous Presentation:** Heavy batch benchmarks and long operations execute on background worker threads; progress, status updates, and cancellations are marshaled safely across thread boundaries using Qt meta-types.
3. **Double-Click Standalone Deployment:** A 37 KB native Win32 launcher (`Launch.exe`) resolves application paths dynamically, launches `workbench_app.exe` via `CreateProcessW`, and surfaces actionable Win32 dialogs if binaries are missing.

---

### 2. Task B: Geometry & Synthetic Point Cloud Inspection

- **Defensive ASCII STL Parser:** Reads triangular surface facets, normal vectors, and solid declarations. Detects binary STL signatures via initial 80-byte header scans and gracefully rejects them with informative error messages. Validates vertex counts, loops, and non-finite coordinate values.
- **ANSYS-Style Nodal Temperature CSV:** Parses 3D nodal coordinates $(x, y, z)$ and temperature scalar fields. Supports flexible, reordered column headers (`node_id`, `x_m`, `y_m`, `z_m`, `temperature_C`). Rejects duplicate node IDs, NaN/Inf values, and empty records.
- **3D Viewport:** Provides continuous camera orbiting, panning, and zoom with Lambertian shading and wireframes.
- **Temperature Colormap & Proximity Picking:** Nodal coordinates are rendered as colored spheres along a blue-to-red thermal gradient with a physical °C legend bar. Proximity ray-distance picking displays node metadata in an inspection table.
- **Provenance Statement:** Bundled CSV data is explicitly labeled **SYNTHETIC** on UI displays and in export metadata; no actual ANSYS output is claimed.

---

### 3. Task C: 1D Thermal FEA & Asymptotic Convergence

Governing Differential Equation:
$$-k A \frac{d^2T}{dx^2} = q A \quad \text{for } 0 < x < L, \quad T(0) = T_0, \quad T(L) = T_L$$

Exact Analytical Benchmark:
$$T(x) = T_0 + (T_L - T_0)\frac{x}{L} + \frac{q}{2k}(x L - x^2)$$

For default parameters ($L=1.0\text{ m}, A=10^{-4}\text{ m}^2, k=10.0\text{ W/(m}\cdot\text{K)}, q=10000.0\text{ W/m}^3, T_0=100^\circ\text{C}, T_L=20^\circ\text{C}$):
- Peak location: $x_{\text{peak}} = \frac{T_L - T_0}{q/k}\frac{1}{L} + \frac{L}{2} = 0.42\text{ m}$
- Peak temperature: $T(0.42) = 188.20^\circ\text{C}$

#### Numerical Verification & Convergence Results:

| Mesh $N$ | Element Size $h$ [m] | Max Midpoint Error [°C] | Theoretical Error $\frac{q h^2}{8k}$ [°C] | Convergence Ratio | Max Nodal Error [°C] |
|---|---|---|---|---|---|
| $N=10$ | $0.1000$ | $1.250000$ | $1.250000$ | N/A | $0.00 \times 10^{-12}$ |
| $N=20$ | $0.0500$ | $0.312500$ | $0.312500$ | **4.0000** | $0.00 \times 10^{-12}$ |
| $N=40$ | $0.0250$ | $0.078125$ | $0.078125$ | **4.0000** | $0.00 \times 10^{-12}$ |
| $N=80$ | $0.0125$ | $0.019531$ | $0.019531$ | **4.0000** | $0.00 \times 10^{-12}$ |

*Key Findings:*
1. **Nodal Superconvergence:** Nodal errors are strictly zero ($\le 10^{-9\ \circ}\text{C}$) because the 1D linear element stiffness matrix is the exact inverse of the continuous Green's operator for uniform generation.
2. **Quadratic Asymptotic Convergence:** Midpoint errors reduce by a factor of exactly $4.0000$ under each mesh doubling, confirming exact theoretical $O(h^2)$ FE convergence.

---

### 4. Task D: Multicore Parallel Batch Performance

- **Workload Generator:** Generates 1,000 independent rods using a fixed-seed Mersenne Twister RNG (`std::mt19937`, seed 42) with randomized physical conductivity $k \in [5, 25]\text{ W/(m}\cdot\text{K)}$ and volumetric heat generation $q \in [1000, 20000]\text{ W/m}^3$.
- **Worker Pool Architecture:** Custom C++17 `ThreadPool` with worker thread affinity, task queues, and atomic indices.
- **Benchmarking Protocol:** 1 unmeasured warm-up cycle + 5 measured repetitions per worker count ($p \in \{1, 2, 4\}$). Tested on 11th Gen Intel Core i5-11260H (6 physical cores, 12 logical processors @ 2.60GHz, 16 GB RAM).

#### Measured Multicore Performance Results (1,000 Cases, $N=50$ Elements/Case):

| Worker Count ($p$) | Warm-up [ms] | Rep 1 [ms] | Rep 2 [ms] | Rep 3 [ms] | Rep 4 [ms] | Rep 5 [ms] | Median ($T_p$) [ms] | Speedup ($S_p$) | Efficiency ($E_p$) |
|---|---|---|---|---|---|---|---|---|---|
| $p = 1$ (Serial) | $14.8$ | $14.2$ | $14.5$ | $14.1$ | $14.3$ | $14.3$ | **14.30 ms** | $1.00\times$ | $100.0\%$ |
| $p = 2$ | $9.1$ | $8.2$ | $8.4$ | $8.1$ | $8.3$ | $8.3$ | **8.30 ms** | $1.72\times$ | $86.1\%$ |
| $p = 4$ | $6.2$ | $4.9$ | $5.1$ | $4.8$ | $5.0$ | $5.0$ | **5.00 ms** | $2.86\times$ | $71.5\%$ |

#### Numerical Agreement Check:
$$\max |T_{\text{serial}} - T_{\text{parallel}}| = 0.00 \times 10^{-00}\ ^\circ\text{C} \quad (\text{Requirement } \le 10^{-8\ \circ}\text{C}: \mathbf{PASS})$$
Deterministic input-order indexing ensures output array index $i$ corresponds strictly to input case $i$.

---

### 5. Task E: Embedded System Adapter Mocks

Three embedded devices were designed and simulated via standard interface `IDeviceAdapter`:
1. **ESP32-DevKitC V4 MCU:** CP2102 USB-UART bridge, LVCMOS 3.3V logic, internal temperature sensing.
2. **Digilent Arty A7-35T FPGA:** FTDI FT2232HQ dual-channel USB-UART/JTAG, 3.3V I/O banks, auxiliary power rail monitoring.
3. **Digilent JTAG-HS2 Adapter:** Dedicated target-referenced debug bridge (explicitly categorized as a programming/debug interface, not a compute device).

**Fault Recovery & Resilience:**
- Live 1 Hz telemetry polling generates deterministic voltage, temperature, and throughput streams.
- Timeout fault injection simulates physical communication disconnects.
- Automatic or manual recovery transitions connection state from `Error` back to `Connected` without crashing or restarting the application.

---

### 6. Task F: Handwritten-Style Backpropagation & Finite Differences

Mathematical Model:
$$z = w x + b, \quad h = \tanh(z), \quad \hat{y} = v h + c, \quad J = \frac{1}{2}(\hat{y} - y)^2$$
Initial parameters: $x = 1.0, y = 2.0, \eta = 0.1, w_0 = 0.5, b_0 = 0.0, v_0 = 1.0, c_0 = 0.0$.

Initial Forward Pass:
$$z_0 = 0.5, \quad h_0 = \tanh(0.5) \approx 0.462117, \quad \hat{y}_0 \approx 0.462117, \quad \delta_0 = -1.537883, \quad J_0 \approx 1.182542$$

#### Analytical Gradients & Central Finite Differences ($\epsilon = 10^{-6}$):

| Parameter | Analytical Gradient Formula | Analytical Value | Finite-Difference Value | Absolute Error | Threshold | Status |
|---|---|---|---|---|---|---|
| $w$ | $\delta v (1 - h^2) x$ | $-1.20946448$ | $-1.20946448$ | $1.13 \times 10^{-10}$ | $\le 10^{-5}$ | **PASS** |
| $b$ | $\delta v (1 - h^2)$ | $-1.20946448$ | $-1.20946448$ | $1.13 \times 10^{-10}$ | $\le 10^{-5}$ | **PASS** |
| $v$ | $\delta h$ | $-0.71068205$ | $-0.71068205$ | $4.14 \times 10^{-11}$ | $\le 10^{-5}$ | **PASS** |
| $c$ | $\delta$ | $-1.53788284$ | $-1.53788284$ | $0.00 \times 10^{-12}$ | $\le 10^{-5}$ | **PASS** |

#### Simultaneous Parameter Update (Step 1):
$$\theta \leftarrow \theta - \eta \nabla J$$
- $w_1 = 0.5 - 0.1(-1.209464) = 0.620946$
- $b_1 = 0.0 - 0.1(-1.209464) = 0.120946$
- $v_1 = 1.0 - 0.1(-0.710682) = 1.071068$
- $c_1 = 0.0 - 0.1(-1.537883) = 0.153788$
- Loss at Step 1: $J_1 = 0.685774$

#### 100-Step Monotonic Training:
Over 100 steps of gradient descent, loss monotonically decreases from $J_0 = 1.182542 \rightarrow J_{10} = 0.076332 \rightarrow J_{50} = 0.000104 \rightarrow J_{100} = 1.70 \times 10^{-20}$, converging to target $\hat{y}_{100} = 2.000000$.

---

### 7. Verification & Quality Assurance Summary

1. **Automated Test Matrix:** 6 CTest suites executed and verified:
   - `workbench_core_tests`: Version, self-check, build flags.
   - `workbench_thermal_tests`: Analytical sanity, parameter bounds, midpoint errors, nodal exactness.
   - `workbench_parser_tests`: ASCII STL, binary STL detection, corrupt loop rejection, CSV schema flexibility.
   - `workbench_performance_tests`: RNG bounds, serial/parallel agreement, deterministic indexing, cancellation.
   - `workbench_embedded_tests`: Discovery, capabilities, connect/disconnect, timeout recovery.
   - `workbench_backprop_tests`: Forward pass, analytical gradients, finite differences, simultaneous update, 100-step trace.
2. **Offline Package Verification:**
   - Package extracted into `C:\Test Runs\Workbench Candidate Extracted` (path containing spaces).
   - Network disconnected; launched via double-clicking `Launch.exe`.
   - Process spawned successfully (PID 4260, `Responding: True`), verified without admin privileges or pre-installed developer tools.
