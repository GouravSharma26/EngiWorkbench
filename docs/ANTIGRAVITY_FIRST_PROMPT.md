# First prompt to give Antigravity

Copy the prompt below after opening this repository in Antigravity.

---

You are implementing the **PASS Science Works — Unified Engineering Workbench** assignment in this repository.

Start by reading `AGENTS.md`, `README.md`, `plan.md`, `docs/ARCHITECTURE.md`, `docs/MODEL_AND_TECH_DECISIONS.md`, `docs/REQUIREMENTS_TRACEABILITY.md`, and `docs/IMPLEMENTATION_PLAN.md`. Then inspect the current repository and report:

1. What files already exist and what is missing.
2. The exact development environment and toolchain you can verify (OS, compiler, CMake, Qt, dependency manager and versions). Do not guess missing values.
3. Any architecture or packaging blockers against the Windows 11 x64 `Launch.exe` requirement.
4. The smallest first implementation increment, its files, and testable acceptance criteria.

Follow `AGENTS.md` strictly. All backend behavior, numerical calculations, parsing, file operations and concurrency must be C++17 or later. Keep the core independent of Qt. Build a Qt 6 Widgets desktop application with Geometry, Thermal Analysis, Performance, Hardware and Backpropagation views. The app must ultimately ship as a self-contained offline Windows package launched by double-clicking `Launch.exe`.

Do not attempt to implement every task in one large change. Work in small increments, update `docs/WORKLOG.md` before and after each increment, add tests with each core feature, run the relevant tests, and record exact outcomes. Never invent successful tests, benchmark numbers, hardware detections, AI prompts, or packaging results. Use `NOT RUN` when evidence does not exist.

The first coding increment should establish the CMake project, a Qt-independent `workbench_core` target, a Qt presentation target, and a minimal automated test target. Do not implement the full UI until the core boundary compiles and is tested.

Before editing, propose the initial file changes and checks. Then implement only that first increment and return the exact commands run, outputs, test results, unresolved issues, and next recommended increment.

---

## Follow-on task prompts

Use one prompt at a time. Always ask Antigravity to inspect existing files before editing.

### Task 1 — Thermal FE core

Implement the thermal model and solver exactly as specified in `docs/THERMAL_FEA_SPEC.md`. Add tests for boundary conditions, all required meshes, midpoint interpolation errors, invalid parameters, and numerical finiteness. Export raw nodal and midpoint error CSVs. Do not implement the plots until the numerical outputs pass.

### Task 2 — File imports

Implement only ASCII STL and the required temperature CSV formats in `docs/DATA_FORMATS.md`. Add positive, empty, malformed, duplicate-ID, non-finite, over-limit, and truncation tests. Do not add binary STL or native CAD/ANSYS formats.

### Task 3 — Visualization

Implement STL triangle rendering and temperature point-cloud rendering according to `docs/UI_UX_SPEC.md`. The viewer must support rotate, pan, zoom and fit-to-view. Include a visible temperature legend and node inspection. Keep parsing in the C++ core.

### Task 4 — Parallel benchmark

Implement the bounded worker pool, deterministic workload generator and measurement protocol in `docs/PARALLEL_BENCHMARK_SPEC.md`. Use the same generated cases in serial and parallel modes. Add deterministic-order, cancellation, five-repeat, and `<=1e-8 °C` agreement tests.

### Task 5 — Embedded design and mocks

Implement the interfaces and mock adapters in `docs/EMBEDDED_INTEGRATION_SPEC.md`. Ensure all UI and logs label behavior as simulated. Include the recoverable timeout scenario. Do not access physical devices.

### Task 6 — Backpropagation

Implement the exact one-sample model and checks in `docs/BACKPROP_SPEC.md`. Use simultaneous updates, central finite differences with epsilon `1e-6`, and add the 100-step loss export.

### Task 7 — Package and release

Follow `docs/PACKAGING_AND_RELEASE.md`. Build the Windows package, run the extracted ZIP on a clean/non-development Windows environment, and record actual findings. Never mark one-click delivery complete without a clean extraction/launch check.
