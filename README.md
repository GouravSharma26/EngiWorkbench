# Unified Engineering Workbench

**PASS Science Works | R&D Internship 2026–2027 — common engineering assignment**

This repository is planned as a compact, testable engineering desktop workbench. It covers geometry import, thermal finite-element analysis, CPU-parallel batch computation, embedded-system integration mocks, and handwritten-style backpropagation.

> **Important status:** This pack is the project specification and Antigravity handoff. It is not a completed application and contains no claim that the build, tests, benchmark, Windows packaging, or physical hardware have been verified. Antigravity must implement the application and replace every `TODO` / `NOT RUN` evidence item with real results.

## Start here

1. Read [`AGENTS.md`](AGENTS.md) before editing code.
2. Read [`plan.md`](plan.md) and [`docs/ANTIGRAVITY_FIRST_PROMPT.md`](docs/ANTIGRAVITY_FIRST_PROMPT.md).
3. Read [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md), [`docs/MODEL_AND_TECH_DECISIONS.md`](docs/MODEL_AND_TECH_DECISIONS.md), and [`docs/REQUIREMENTS_TRACEABILITY.md`](docs/REQUIREMENTS_TRACEABILITY.md).
4. Follow [`docs/IMPLEMENTATION_PLAN.md`](docs/IMPLEMENTATION_PLAN.md) in small, testable increments.
5. Record actual commands, outputs, failures, decisions, and time in [`docs/WORKLOG.md`](docs/WORKLOG.md).

## Product direction

- Windows 11 x64 standalone desktop application.
- C++17 or later for all backend logic, numerical computation, file parsing, and concurrency.
- Qt 6 Widgets for presentation; Qt must not leak into the portable computational core.
- A single packaged process launched by double-clicking `Launch.exe`.
- A genuine bounded C++ worker pool for independent thermal solves.
- Device interactions are explicitly simulated; physical wiring/programming is outside scope.

The architecture and decision rationale are in [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md) and [`docs/MODEL_AND_TECH_DECISIONS.md`](docs/MODEL_AND_TECH_DECISIONS.md).

## Assignment requirements covered

| Area | Deliverable |
|---|---|
| Interface / backend | Geometry, Thermal Analysis, Performance, Hardware, and Backpropagation views; validation, status, cancellation, export |
| Geometry / temperature data | ASCII STL triangle viewer and ANSYS-style nodal temperature CSV point cloud |
| Thermal FEA | Uniform 1D two-node linear-element steady-state solver, exact comparison, midpoint error, mesh convergence |
| Multicore | Same seeded workload in serial and parallel, 1–4 workers as available, five repetitions, median/speedup/efficiency |
| Embedded | Microcontroller, FPGA, and JTAG adapter mock interfaces; block diagram and electrical assumptions |
| Backpropagation | One-sample tanh regression, handwritten derivation, C++ gradients, central finite differences, 100-step loss trace |
| Submission | Windows package, editor launch configuration, source, tests, raw results, report, demo, AI-use log, ownership statement, Git history |

See [`docs/REQUIREMENTS_TRACEABILITY.md`](docs/REQUIREMENTS_TRACEABILITY.md) for criterion-level mapping to the 100-point rubric.

## Proposed repository layout

```text
.
├── AGENTS.md
├── README.md
├── plan.md
├── CMakeLists.txt                 # create during implementation
├── CMakePresets.json              # create during implementation
├── vcpkg.json                     # create only if vcpkg dependencies are selected
├── .vscode/
│   ├── launch.json                # launch packaged application
│   └── tasks.json                 # optional build tasks; not needed to launch package
├── src/
│   ├── core/                      # Qt-free C++17 domain, parsers, FEA, worker pool, mocks, backprop
│   │   ├── include/workbench/
│   │   └── src/
│   └── app/                       # Qt widgets, OpenGL view, QPainter plots, app entry point
├── tests/                         # core unit/integration tests and fixtures
├── data/examples/                 # self-created geometry and synthetic temperature CSV
├── results/                       # actual CSVs, plots, benchmark and verification records
├── docs/                          # design, validation, report, evidence and AI-use documentation
├── packaging/windows/             # deployment manifest, licences, packaging scripts
├── scripts/                       # reproducible helper scripts; no backend feature logic
└── dist/                          # generated package; do not commit transient build files
```

## Example data

- `data/examples/sample_tetrahedron_ascii.stl` — self-created ASCII STL, dimensions in metres by convention in its manifest. STL itself does not encode units.
- `data/examples/synthetic_temperature_nodes.csv` — self-created synthetic nodal data in the required schema. It is **not** an actual ANSYS export.
- `data/examples/manifest.json` — provenance, assumptions, units, and schema notes.

## Build and run (to be completed by implementation)

The intended source build uses CMake and a supported Visual Studio C++ toolchain. Exact Qt/compiler/CMake/dependency versions must be pinned in the implementation's dependency record and reproduced in CI/build notes. See [`docs/HUMAN_SETUP.md`](docs/HUMAN_SETUP.md).

The final package must work offline after ZIP extraction. The user double-clicks `Launch.exe`; there is no first-run build, package installation, terminal command, administrator requirement, or internet dependency. See [`docs/PACKAGING_AND_RELEASE.md`](docs/PACKAGING_AND_RELEASE.md).

## Evidence integrity

Never invent test counts, benchmark speedups, CPU information, clean-machine launch results, AI interactions, screenshots, or hardware checks. Use `NOT RUN` until evidence exists. If a check cannot be performed, state why and record the remaining risk.

## Primary assignment source

The requirements in this pack are based on the three-page **PASS SCIENCE WORKS | R&D INTERNSHIP 2026–2027 — Unified Engineering Workbench** brief supplied with this conversation. Its detailed scope is on pages 1–2; its points and shortlist gates are on page 3.
