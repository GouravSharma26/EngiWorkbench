# AI Assistance and Prompt Log

**Project:** PASS Science Works — Unified Engineering Workbench  
**Candidate:** Gourav Sharma  
**Date:** 2026-10-31  

This log provides an honest, factual disclosure of the AI programming tools, models, representative interactions, corrections, and independent validation performed during the project.

---

## 1. Tools and Models Used

| Date | Tool / Model | Purpose | Files / Areas Affected | Candidate Verification |
|---|---|---|---|---|
| 2026-10-31 | Antigravity IDE (Gemini / Claude agentic model) | Architecture design, CMake project scaffolding, core domain types | `CMakeLists.txt`, `CMakePresets.json`, `src/core/` | Manual review of CMake target boundaries and Qt decoupling |
| 2026-10-31 | Antigravity IDE | Numerical 1D thermal FEA and exact analytical benchmark | `src/core/src/thermal/solver.cpp`, `test_thermal.cpp` | Independent analytical derivation and midpoint convergence check |
| 2026-10-31 | Antigravity IDE | ASCII STL and synthetic CSV parser implementation | `src/core/src/io/` | Unit tests with malformed, binary, NaN, and inverted records |
| 2026-10-31 | Antigravity IDE | Multicore ThreadPool, batch runner, and deterministic ordering | `src/core/src/performance/` | Verified serial vs parallel max difference $\le 10^{-8\ \circ}\text{C}$ |
| 2026-10-31 | Antigravity IDE | Embedded device mock adapters and timeout fault recovery | `src/core/src/embedded/` | Verified state transitions, SIMULATED status, and fault injection |
| 2026-10-31 | Antigravity IDE | Handwritten-style backpropagation, analytical gradients, and finite differences | `src/core/src/backprop/` | Central finite-difference test ($\le 10^{-5}$ tolerance) and 100-step trace |
| 2026-10-31 | Antigravity IDE | Qt 6 Widgets presentation layer (3D viewport, 2D plots, 5 views) | `src/app/` | Tested compilation under MSVC, signal-slot typing, and UI responsiveness |
| 2026-10-31 | Antigravity IDE | Win32 launcher, automated packaging scripts, and offline testing | `src/launcher/`, `scripts/`, `dist/` | Clean directory extraction and offline double-click verification |

---

## 2. Representative Interaction Records

### Interaction 1: Strict Core Decoupling & 1D Thermal FEA Solver
- **Context:** Establishing `workbench_core` as a standalone C++17 library with zero Qt dependencies.
- **Prompt:** *"Implement the 1D steady-state thermal FEA solver for rod conduction with internal heat generation, analytical midpoint error comparison, and mesh convergence study over N=10, 20, 40, 80."*
- **Output:** Element stiffness assembly $[K_e] = \frac{kA}{h}\begin{bmatrix}1 & -1 \\ -1 & 1\end{bmatrix}$, consistent load $\{f_e\} = \frac{qAh}{2}\begin{Bmatrix}1 \\ 1\end{Bmatrix}$, Dirichlet BC imposition, and Thomas algorithm tridiagonal solver.
- **Candidate Changes:** Ensured both Dirichlet boundary temperatures are strictly imposed on the system, enforced positive physical parameters ($L, A, k > 0$), and added analytical solution verification at element midpoints.
- **Files Affected:** `src/core/include/workbench/domain/thermal.hpp`, `src/core/src/thermal/solver.cpp`, `tests/test_thermal.cpp`.
- **Validation:** Nodal exactness verified ($\le 10^{-9\ \circ}\text{C}$); midpoint errors strictly matched theoretical $e = \frac{q}{8k} h^2$ ($1.25, 0.3125, 0.078125, 0.01953125^\circ\text{C}$); convergence ratio $= 4.0000$.

### Interaction 2: Central Finite Differences & Simultaneous Backprop Update
- **Context:** Implementing Task F backpropagation with analytical chain rule derivatives and finite differences.
- **Prompt:** *"Derive and implement handwritten-style backpropagation for single-sample tanh regression: $x=1.0, y=2.0, \eta=0.1$. Verify gradients with central finite differences ($\epsilon=10^{-6}$, error $\le 10^{-5}$)."*
- **Output:** Analytical gradients: $\frac{\partial J}{\partial w} = \delta v (1-h^2) x, \frac{\partial J}{\partial b} = \delta v (1-h^2), \frac{\partial J}{\partial v} = \delta h, \frac{\partial J}{\partial c} = \delta$.
- **Candidate Changes:** Verified that all four parameter updates occur simultaneously from the unmutated parameter state ($\theta \leftarrow \theta - \eta \nabla J$) so that updated weights do not corrupt subsequent gradient calculations within the step.
- **Files Affected:** `src/core/include/workbench/backprop/model.hpp`, `src/core/src/backprop/model.cpp`, `tests/test_backprop.cpp`.
- **Validation:** Analytical gradients matched numerical central finite differences with max error $1.13 \times 10^{-10} \le 10^{-5}$. Monotonic convergence over 100 steps verified.

---

## 3. Genuine Unsuitable Suggestions and Challenge Tests

### Case 1: Incorrect Standard Library Method on Qt String Class
- **What Was Suggested:** During the drafting of `src/app/views/hardware_view.cpp`, the AI drafted string concatenation checking whether an interface list was empty using:
  ```cpp
  if (!ifaces.empty()) ifaces += ", ";
  ```
- **Why It Was Unsuitable:** In Qt, `QString` does not have a member function named `empty()`; the Qt idiom and method is `isEmpty()`. The MSVC compiler failed with:
  ```text
  hardware_view.cpp(95,21): error C2039: 'empty': is not a member of 'QString'
  ```
- **Candidate Correction:** Identified the error immediately, checked Qt 6 `QString` documentation, and corrected the call to `if (!ifaces.isEmpty())`. Re-ran MSVC compilation to confirm the fix.
- **Evidence Path:** `src/app/views/hardware_view.cpp`, git commit `529942a`.

### Case 2: Windows SDK Dependency Resolution vs Silent Installation
- **What Was Suggested:** An AI agent initially attempted to silently install the Windows 11 SDK via non-interactive CLI:
  ```powershell
  winget install Microsoft.WindowsSDK.10.0.22621
  ```
- **Why It Was Unsuitable:** On Windows 11, installing system-level SDK headers and libraries into `C:\Program Files (x86)\Windows Kits` requires administrative UAC elevation. Running `winget` in an unelevated sandbox silently failed with exit code `2147942450` (`0x80070032`), leaving `kernel32.lib` missing and MSBuild unable to link.
- **Candidate Correction:** Rejected further automated workarounds. Diagnosed the exact missing component via `MSBuild` logs (`warning MSB8003: The WindowsSDKDir property is not defined`), guided the human user to open the Visual Studio Installer GUI with elevation, and verified that `10.0.26100.0` / `10.0.22621.0` headers were placed on disk before proceeding.
- **Evidence Path:** `docs/WORKLOG.md` (Increment 8), `build/windows-msvc-x64-release/CMakeFiles/CMakeConfigureLog.yaml`.

---

## 4. Candidate Ownership and Verification Statement

All code generated with AI assistance was reviewed for correctness, memory safety, thread safety, and unit test coverage prior to integration. No AI output was accepted as proof of correctness without independent compilation, CTest execution, and numerical verification against analytical formulas.
