# Intellectual Property and Code Ownership Declaration

**Project:** PASS Science Works — Unified Engineering Workbench  
**Candidate / Author:** Gourav Sharma  
**Date:** 2026-10-31  
**Repository:** https://github.com/GouravSharma26/EngiWorkbench.git  

---

## 1. Scope of Work and Originality Summary

This project was developed for the PASS Science Works R&D Internship 2026–2027 assignment. In accordance with the assignment guidelines, this document provides an honest, transparent breakdown of the source code, numerical derivations, AI tool assistance, and third-party dependencies.

---

## 2. Categorization of Project Assets

### A. Candidate-Authored & Architected Code
The core architecture, module decomposition, mathematical modeling, and engineering design were defined and directed by the candidate:
- **Architecture & System Design:**
  - Strict decoupling of `src/core` (pure C++17) from the Qt 6 presentation layer.
  - State machine design (`Idle`, `Running`, `Cancelling`, `Completed`, `Cancelled`, `Failed`).
  - Interface contracts for embedded mock adapters (`IDeviceAdapter`) and `ThreadPool`.
- **Numerical Derivations & Analytical Methods:**
  - 1D Steady-State Thermal conduction-generation formulation and exact analytical solution.
  - Finite element element stiffness $[K_e] = \frac{k A}{h} \begin{bmatrix} 1 & -1 \\ -1 & 1 \end{bmatrix}$ and consistent load $\{f_e\} = \frac{q A h}{2} \begin{Bmatrix} 1 \\ 1 \end{Bmatrix}$.
  - Analytical gradients and simultaneous parameter updates for single-sample tanh regression.
- **Native Launcher:**
  - Win32 API launcher (`src/launcher/main.cpp`) executing `workbench_app.exe` via `CreateProcessW` without Qt or admin privileges.
- **Packaging and Verification Infrastructure:**
  - Automated deployment and packaging scripts (`scripts/package_release.ps1`, `scripts/create_submission.ps1`, `scripts/verify_package.ps1`).

---

### B. AI-Assisted Implementation & Documentation
AI assistance (Antigravity IDE powered by Google DeepMind / Claude / Gemini coding agents) was utilized for pair programming, boilerplate generation, and verification:
- **C++ Implementation:**
  - Drafting parsing logic in `src/core/src/io/stl_parser.cpp` and `src/core/src/io/csv_parser.cpp`.
  - Drafting Qt Widgets layouts and custom `QPainter` drawing in `src/app/widgets/viewport_3d.cpp` and `plot_widget.cpp`.
  - Automated test assertions in `tests/test_*.cpp`.
- **Documentation & Traceability:**
  - Structuring markdown specification files, checklists, and worklog templates (`docs/WORKLOG.md`, `docs/REQUIREMENTS_TRACEABILITY.md`, `docs/VALIDATION_PROTOCOL.md`).

---

### C. Self-Generated Sample Datasets
- **`data/examples/sample_tetrahedron_ascii.stl`:**
  - 4-facet regular tetrahedron generated mathematically for geometric rendering verification.
  - No customer, proprietary, or commercial CAD files were used.
- **`data/examples/synthetic_temperature_nodes.csv`:**
  - 7-node synthetic temperature cloud with coordinates matching the sample tetrahedron vertices and edge midpoints.
  - **Explicit Notice:** Marked as **SYNTHETIC**. Not an actual ANSYS simulation export.

---

### D. Third-Party Libraries & Redistributables
- **The Qt Company — Qt 6.8.2 (MSVC 2022 64-bit):**
  - Used dynamically under the **GNU Lesser General Public License (LGPL) version 3**.
  - Modules shipped in standalone package: `Qt6Core.dll`, `Qt6Gui.dll`, `Qt6Widgets.dll`, `Qt6OpenGL.dll`, `Qt6OpenGLWidgets.dll`, `Qt6Svg.dll`, `Qt6Network.dll`, and platform/style plugins.
  - No Qt source code was modified. Shared libraries can be replaced by the end user.
- **Microsoft Corporation — Visual C++ 2022 Redistributable:**
  - `msvcp140.dll`, `vcruntime140.dll`, `vcruntime140_1.dll`, and Universal CRT runtimes distributed under the Visual Studio 2022 REDIST agreement.

---

## 3. Reused Code or Assets from External Repositories

No external third-party numerical solvers, automatic differentiation engines (e.g., PyTorch, TensorFlow, autodiff), CAD kernels (e.g., OpenCASCADE), or hardware drivers were copied or integrated. All numerical calculation, thread management, and file processing logic was written from first principles using standard C++17.
