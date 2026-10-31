# Project plan — Unified Engineering Workbench

**Status:** Specification prepared; implementation not yet started in this pack.

## Objective
Deliver an individual 15-calendar-day prototype (target effort 40–50 hours) matching the PASS Science Works common R&D internship assignment. The application must import ASCII STL and ANSYS-style nodal temperature CSV, solve a specified steady 1D thermal FE problem, benchmark independent CPU-parallel solves, demonstrate embedded-system adapter mocks, and implement one-sample handwritten-style backpropagation.

## Priority order
1. A working, testable C++ core and thermal solver. The shortlist gate requires at least 6/12 in C++ backend and 7/14 in FEA.
2. Correct parsers and usable 3D views.
3. Genuine parallel workload and reproducible benchmark evidence.
4. Embedded mocks and electrical integration design.
5. Backpropagation implementation and handwritten appendix.
6. Responsive UI integration, reproducible packaging, report, demo, and evidence audit.

## 15-calendar-day execution plan (target 46.5 hours)
| Day | Target hours | Focus | Exit evidence |
|---:|---:|---|---|
| 1 | 3.0 | Confirm toolchain, repository, CMake skeleton, core/app boundaries | Build configuration, dependency record, first commit |
| 2 | 3.0 | Domain types, validation/error model, test harness | Core builds; basic positive/negative tests |
| 3 | 4.0 | Thermal FE assembly, Dirichlet treatment, tridiagonal solve | Solver tests for N=10,20,40,80 |
| 4 | 3.0 | Exact solution, midpoint-error verification, CSV/plot exports | Mesh convergence CSV/plot; verified errors |
| 5 | 3.0 | ASCII STL and nodal CSV parsers | Fixtures, malformed/empty/non-finite tests |
| 6 | 3.0 | STL 3D renderer and camera controls | Rotate/pan/zoom/fit checks |
| 7 | 3.0 | Temperature point cloud, legend, node inspection | Color scale, units, click inspection checks |
| 8 | 4.0 | Bounded worker pool, deterministic seeded workload | Unit tests, cancellation, serial/parallel agreement |
| 9 | 3.0 | Benchmark runner and report exports | Five-repetition raw data and summary on actual machine |
| 10 | 3.0 | Embedded interfaces and mock adapters | Three device types, telemetry, timeout recovery |
| 11 | 3.0 | Backprop C++ implementation and numerical checks | Gradient/finite-difference test and loss CSV/plot |
| 12 | 3.0 | Integrate tabs, progress, errors, export, cancellation | End-to-end UI smoke checks |
| 13 | 3.0 | Windows package and editor run configuration | `Launch.exe` package candidate and launch log |
| 14 | 3.0 | Full test, clean extraction, report and evidence audit | Test report, clean-machine checklist, report draft |
| 15 | 2.5 | Demo recording/walkthrough, ZIP manifest, final review | Candidate ZIP, commit log, final checklist |

Total target: **46.5 hours**. The candidate must record actual effort; adjust order if a blocker threatens the core/FEA/delivery gates.

## Milestones
- **M1 — Core correctness:** domain model, parsers, FE solver, backprop, tests.
- **M2 — Visual workbench:** tabs, geometry rendering, point cloud, result plots, exports.
- **M3 — Performance and embedded:** actual worker pool, reproducible benchmark, adapter mocks and design diagram.
- **M4 — Submission-ready:** Windows one-click package, VS Code/Antigravity launch configuration, report, demonstration, AI-use log, ownership statement and repository history.

## Stop-the-line conditions
Pause feature work and fix the blocker if any of these occur:
- The core target does not build with the selected C++17 toolchain.
- FEA does not match the analytical benchmark or prescribed boundary values.
- Serial and parallel outputs differ by more than `1e-8 °C`.
- A parser accepts NaN/Inf or malformed required data.
- A long task blocks the UI or cancellation leaves incomplete published results.
- `Launch.exe` requires a first-run build, internet, admin rights, or an undeclared installed runtime.

## Definition of done
Use the rubric mapping and final checklist in `docs/REQUIREMENTS_TRACEABILITY.md` and `docs/DEVELOPMENT_CHECKLIST.md`. No item is complete based on planned design alone; each requires implementation and evidence.
