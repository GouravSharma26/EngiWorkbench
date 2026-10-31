# PASS Science Works — Unified Engineering Workbench

**R&D Internship 2026–2027 Engineering Assignment**  
**Author:** Gourav Sharma  
**Version:** v0.1.0-prototype  
**Target Platform:** Windows 11 x64  
**Repository:** https://github.com/GouravSharma26/EngiWorkbench.git  

---

## 1. Project Overview

The **Unified Engineering Workbench** is a high-performance, modular desktop engineering workstation designed for scientific analysis, geometry inspection, finite-element thermal simulation, multicore parallel benchmarking, embedded hardware telemetry monitoring, and transparent neural network backpropagation.

The application is built strictly following standard C++17 for all core domain logic, numerical algorithms, file parsing, and multithreading, completely independent of the presentation framework. The graphical user interface is implemented using Qt 6.8.2 Widgets with custom high-performance 2D vector plots and a 3D geometry viewport. The application ships as a fully self-contained Windows package launched via a native, double-clickable `Launch.exe` with zero third-party installer or admin requirements.

---

## 2. Key Capabilities Across All Five Views

### 1. Geometry View (Task B)
- **ASCII STL 3D Visualizer:** Interactive 3D renderer displaying triangular surface meshes with directional Lambertian shading, facet normal computation, and optional wireframe overlays.
- **Full Viewport Navigation:** Intuitive mouse controls:
  - Left Mouse Drag: Spherical camera orbit (azimuth and elevation).
  - Right / Middle Mouse Drag: Camera viewport panning.
  - Mouse Wheel: Continuous zoom in and out.
  - Reset View: Fit-to-view bounding-box centering.
- **Nodal Temperature Point Cloud:** Renders ANSYS-style 3D temperature coordinate fields with continuous smooth false-color colormap gradients (blue $\rightarrow$ cyan $\rightarrow$ green $\rightarrow$ yellow $\rightarrow$ red).
- **Physical Legend & Proximity Inspection:** Calibrated temperature legend bar displaying values in Celsius (°C). Clicking any point picks the closest node and presents its exact coordinates, ID, and temperature in an inspection table.
- **Provenance Transparency:** Bundled temperature data is prominently labeled **SYNTHETIC**; binary STLs are safely rejected with actionable diagnostics.

### 2. Thermal Analysis View (Task C)
- **1D Steady-State Heat Conduction Solver:** Solves the boundary value problem $-k A \frac{d^2T}{dx^2} = q A$ over a rod of length $L$, area $A$, conductivity $k$, volumetric generation $q$, and prescribed Dirichlet boundary temperatures $T(0) = T_0$ and $T(L) = T_L$.
- **Finite Element Assembly:** Assembles two-node linear element stiffness matrices $[K_e] = \frac{k A}{h} \begin{bmatrix} 1 & -1 \\ -1 & 1 \end{bmatrix}$ and consistent load vectors $\{f_e\} = \frac{q A h}{2} \begin{Bmatrix} 1 \\ 1 \end{Bmatrix}$.
- **Analytical Solution & Midpoint Error Verification:** Compares FE solutions against the exact analytical distribution $T(x) = T_0 + (T_L - T_0)\frac{x}{L} + \frac{q}{2k}(x L - x^2)$. Computes maximum absolute errors at element midpoints ($e_{\max,\text{mid}} = \frac{q}{8k} h^2$).
- **Mesh Convergence Study:** Refines meshes systematically ($N = 10, 20, 40, 80$), verifies exact quadratic asymptotic convergence ($O(h^2)$ with convergence ratio strictly equal to $4.0000$), and exports verified nodal, midpoint, and convergence CSV datasets.

### 3. Performance View (Task D)
- **Reproducible Multicore Benchmark:** Benchmarks batches of independent thermal FEA cases generated deterministically using a fixed-seed Mersenne Twister RNG (`std::mt19937`).
- **Standard Library Worker Pool:** Powered by a bounded C++17 `ThreadPool` using condition-variable work queues and atomic dynamic scheduling.
- **Rigorous 5-Repetition Protocol:** Executes 1 unmeasured warm-up cycle followed by 5 timed repetitions per worker count ($p \in \{1, 2, 4\}$). Computes median wall-clock time, speedup factor $S_p = T_1 / T_p$, and parallel efficiency $E_p = S_p / p$.
- **Numerical Output Agreement:** Verifies that serial and parallel execution order yields deterministic results and identical temperatures within $\max |T_{\text{serial}} - T_{\text{parallel}}| \le 10^{-8\ \circ}\text{C}$.
- **Responsive Execution & Safe Cancellation:** Workloads run on background threads; atomic cancellation halts pending tasks immediately without publishing partial or corrupted datasets.

### 4. Hardware View (Task E)
- **Embedded System Mocks:** Interfaces with three simulated physical adapters:
  1. *ESP32-DevKitC V4 Microcontroller* (Silicon Labs CP2102 USB-UART, 3.3V logic, internal temperature sensor).
  2. *Digilent Arty A7-35T FPGA* (FTDI FT2232HQ USB-JTAG/UART, Xilinx Artix-7, onboard auxiliary rail sensing).
  3. *Digilent JTAG-HS2 Debug Adapter* (High-speed JTAG programming and debug bridge; explicitly declared as an interface adapter, not a compute device).
- **Status & Electrical Badges:** Prominent `SIMULATED` badge on all device cards declaring that physical hardware is mocked. Displays logic standards (LVCMOS 3.3V), supply rails, and supported protocols (UART, SPI, I2C, JTAG).
- **Live Telemetry & Fault Recovery:** Real-time 1 Hz telemetry polling (die temperature, core voltage, packet throughput). Demonstrates simulated communication timeout faults and clean, safe recovery via reconnection without application restart.

### 5. Backpropagation View (Task F)
- **Single-Sample Tanh Neural Regression:** Transparent implementation of a single-neuron regression model ($x = 1.0, y = 2.0, \eta = 0.1$):
  $$z = w x + b, \quad h = \tanh(z), \quad \hat{y} = v h + c, \quad J = \frac{1}{2}(\hat{y} - y)^2$$
- **Exact Handwritten Analytical Gradients:** Evaluates exact partial derivatives prior to parameter mutation:
  $$\frac{\partial J}{\partial w} = \delta v (1 - h^2) x, \quad \frac{\partial J}{\partial b} = \delta v (1 - h^2), \quad \frac{\partial J}{\partial v} = \delta h, \quad \frac{\partial J}{\partial c} = \delta$$
- **Central Finite-Difference Verification:** Verifies analytical derivatives against central differences ($\epsilon = 10^{-6}$), confirming all errors satisfy $\Delta \le 10^{-5}$ (measured error: $1.13 \times 10^{-10}$).
- **Monotonic 100-Step Training:** Executes 100 simultaneous parameter updates ($\theta \leftarrow \theta - \eta \nabla J$) demonstrating smooth, monotonic loss convergence to $J_{100} = 1.70 \times 10^{-20}$ and $\hat{y} = 2.000000$. Exports full step-by-step training curves.

---

## 3. Technology Stack & Environment

| Component | Specification / Version |
|---|---|
| **Programming Language** | C++17 (ISO/IEC 14882:2017) |
| **GUI Framework** | Qt 6.8.2 Widgets & OpenGLWidgets (The Qt Company, LGPLv3) |
| **Compiler** | MSVC 19.44.35228.0 (Microsoft Visual Studio 2022 Community v17.14.38) |
| **Build System** | CMake 3.20+ with native `CMakePresets.json` |
| **Host OS** | Windows 11 Home / Pro x64 (Version 10.0.26100+) |
| **Windows SDK** | Windows 11 SDK 10.0.26100.0 / 10.0.22621.0 |
| **Launcher** | Native Win32 C++ application (`Launch.exe`, 37 KB, 0 Qt dependencies) |

---

## 4. Architectural Separation

```text
EngiWorkbench/
├── src/
│   ├── core/           # PURE C++17 COMPUTATIONAL CORE (ZERO QT HEADERS)
│   │   ├── domain/     # Physical problems, geometry, solution representations
│   │   ├── thermal/    # 1D FEA solver, tridiagonal algorithm, analytical solutions
│   │   ├── io/         # ASCII STL parser, CSV parser, validation models
│   │   ├── performance/# ThreadPool, parallel batch runner, agreement verifier
│   │   ├── embedded/   # Mock adapter interfaces, telemetry generator, timeout fault injection
│   │   └── backprop/   # Handwritten forward pass, analytical gradients, finite differences
│   │
│   ├── app/            # QT 6 PRESENTATION LAYER
│   │   ├── widgets/    # Viewport3D (CAD/point cloud), PlotWidget (2D curves & bars)
│   │   ├── views/      # GeometryView, ThermalView, PerformanceView, HardwareView, BackpropView
│   │   ├── main_window # QTabWidget shell, dark theme stylesheet, menu bar, status bar
│   │   └── main.cpp    # QApplication entry point
│   │
│   └── launcher/       # NATIVE WIN32 LAUNCHER (Launch.exe)
│       └── main.cpp    # Win32 CreateProcessW relative launcher with error dialogs
│
├── tests/              # AUTOMATED TEST SUITE (6 CTest test suites, 33 tests)
├── data/examples/      # Bundled sample STL, synthetic CSV, and manifest.json
├── results/            # Verified raw CSV benchmark timings, convergence studies, and training logs
├── scripts/            # Reproducible packaging, launcher build, verification, and submission scripts
├── docs/               # Specifications, Architecture, Worklog, and Verification Protocol
└── dist/Workbench/     # Final standalone offline Windows distribution package
```

---

## 5. Standalone Execution (One-Click Offline Launch)

The standalone application is deployed in `dist/Workbench/` (and archived in `CandidateID_Workbench.zip`).

### To Run:
1. Extract `CandidateID_Workbench.zip` to any local folder (paths with spaces supported, e.g., `C:\Engineering Workbench`).
2. Double-click `Launch.exe`.
3. The application opens immediately. **No administrator rights, terminal commands, compiler installations, or internet connections are required.**

---

## 6. Building and Running from Source

Prerequisites: Visual Studio 2022 (MSVC x64 C++ workload and Windows 11 SDK) and Qt 6.8.2 (`C:\Qt\6.8.2\msvc2022_64`).

### Release Configuration
```powershell
# 1. Configure
cmake --preset windows-msvc-x64-release

# 2. Build in parallel
cmake --build --preset windows-msvc-x64-release --parallel

# 3. Run all unit tests
ctest --preset windows-msvc-x64-release --output-on-failure
```

### Debug Configuration
```powershell
cmake --preset windows-msvc-x64-debug
cmake --build --preset windows-msvc-x64-debug --parallel
ctest --preset windows-msvc-x64-debug --output-on-failure
```

### Packaging Script
To reproduce the complete standalone distribution and submission ZIP:
```powershell
powershell -ExecutionPolicy Bypass -File scripts/package_release.ps1
powershell -ExecutionPolicy Bypass -File scripts/create_submission.ps1
```

---

## 7. Verified Automated Test Results (100% Pass)

Both Release and Debug configurations have been executed and verified via CTest:

```text
Test project .../build/windows-msvc-x64-release
    Start 1: workbench_core_tests
1/6 Test #1: workbench_core_tests .............   Passed    0.01 sec
    Start 2: workbench_thermal_tests
2/6 Test #2: workbench_thermal_tests ..........   Passed    0.01 sec
    Start 3: workbench_parser_tests
3/6 Test #3: workbench_parser_tests ...........   Passed    0.03 sec
    Start 4: workbench_performance_tests
4/6 Test #4: workbench_performance_tests ......   Passed    0.01 sec
    Start 5: workbench_embedded_tests
5/6 Test #5: workbench_embedded_tests .........   Passed    0.01 sec
    Start 6: workbench_backprop_tests
6/6 Test #6: workbench_backprop_tests .........   Passed    0.01 sec

100% tests passed out of 6 (33 of 33 individual assertions passed)
```

### Numerical Criteria Summary:
- **Thermal Convergence Ratio:** $4.0000$ across $N=10, 20, 40, 80$ (exact $O(h^2)$).
- **Thermal Nodal Error:** $\le 10^{-9\ \circ}\text{C}$ (superconvergence at mesh vertices).
- **Multicore Agreement:** $\max |T_{\text{serial}} - T_{\text{parallel}}| = 0.00 \times 10^{-00}\ ^\circ\text{C} \le 10^{-8\ \circ}\text{C}$.
- **Backprop Gradient Check:** Maximum absolute error $1.13 \times 10^{-10} \le 10^{-5}$ at $\epsilon = 10^{-6}$.
- **Backprop Training:** Monotonic loss descent from $J_0 = 1.182542$ to $J_{100} = 1.70 \times 10^{-20}$.

---

## 8. Supported File Formats & Data Conventions

1. **ASCII STL Geometry (`.stl`):** Standard ASCII format starting with `solid <name>` and containing `facet normal` followed by `outer loop` with 3 vertices. Binary STLs are detected by null-byte byte header scans and rejected safely.
2. **ANSYS-Style Nodal Temperature CSV (`.csv`):** Formatted with headers `node_id,x_m,y_m,z_m,temperature_C`. Supports flexible column ordering, whitespace tolerance, and actionable validation of duplicate IDs, non-finite values, and malformed records.
3. **CSV Result Exports:** All tables across Thermal, Performance, and Backprop views can be exported to user-selected writable files with standard CSV formatting.

---

## 9. Known Limitations

- **ASCII STL Only:** Binary STL files, native solid geometry formats (STEP/IGES), and proprietary CAD databases are deliberately out of scope and rejected with actionable messages.
- **1D Rod Thermal Model:** The FEA solver implements 1D longitudinal conduction with internal generation. It is not a 3D volumetric thermal solver.
- **Simulated Hardware Mocks:** Physical microcontrollers and FPGA boards are not detected or programmed; all device cards are simulated software mocks designed for telemetry visualization and fault-recovery demonstration.

---

## 10. Licence and Third-Party Compliance

- **Project Code:** Copyright (c) 2026 Gourav Sharma. Developed for PASS Science Works.
- **Qt 6 Framework:** Licensed under the GNU Lesser General Public License (LGPL) version 3. Shipped dynamically to permit library replacement.
- **Microsoft Visual C++ Runtime:** Distributed in accordance with the Microsoft Visual Studio 2022 Community REDIST terms.
- For complete licensing details, see [`THIRD_PARTY_NOTICES.md`](THIRD_PARTY_NOTICES.md) and [`OWNERSHIP.md`](OWNERSHIP.md).
