# Implementation plan and work breakdown

## Phase 0 — repository and toolchain (Day 1)

- Inspect machine and record Windows, Visual Studio/MSVC, CMake, Qt, SDK, dependency-manager versions.
- Create CMake project with `workbench_core`, Qt app, and test targets.
- Configure x64 Debug and Release builds.
- Confirm `workbench_core` builds without Qt include or link dependencies.
- Add `.gitignore`, `CMakePresets.json`, dependency manifest/lock and CI once exact versions are selected.

**Exit:** clean configure/build and one core test executable.

## Phase 1 — domain and numerical core (Days 2–4)

- Define unit-bearing domain structs and typed error results.
- Implement thermal problem validation, two-node element assembly, Dirichlet reduction and tridiagonal solution.
- Implement FE interpolation at arbitrary x and exact analytical reference.
- Test required N values and theoretical midpoint-error curve.
- Export raw nodal values and midpoint errors before any plot is added.

**Exit:** numerical tests pass and CSV agrees with the expected reference values in `THERMAL_FEA_SPEC.md`.

## Phase 2 — data import (Day 5)

- Implement ASCII STL parser with triangle count, finite coordinates, facet structure and normal handling.
- Implement CSV parser requiring the five specified columns.
- Implement file-size/record limits and actionable parse errors.
- Add positive, empty, malformed, duplicate ID, non-finite, over-limit and truncated-file tests.
- Add provenance/units metadata for the examples.

**Exit:** parser unit tests pass; synthetic data labels are visible in UI/docs.

## Phase 3 — visualisation and UI shell (Days 6–7 and 12)

- Build a responsive Qt main window with the required views.
- Add STL rendering with camera orbit, pan, zoom and fit-to-view.
- Add colored nodal point cloud and temperature legend; support selecting a node to inspect ID, coordinates and °C.
- Add thermal plots, midpoint/convergence plots and export controls.
- Add progress, status, cancel and error surfaces; avoid blocking GUI thread.

**Exit:** documented manual UI test cases pass with sample files.

## Phase 4 — multicore (Days 8–9)

- Implement seeded case generator; validate positive k/q.
- Implement serial and bounded-pool parallel execution through the same solver.
- Keep result vector indexed by input case; publish only complete outputs.
- Add cancellation and deterministic-order tests.
- Run five repetitions for each worker count available in {1,2,3,4}; compute median, speedup, efficiency and agreement.
- Capture hardware/compiler/optimisation/workload/warm-up metadata.

**Exit:** actual benchmark files and honest analysis are available.

## Phase 5 — embedded mock design (Day 10)

- Finalize board/adaptor choice and pin-voltage table with authoritative manuals.
- Implement discover/capabilities/connect/disconnect/telemetry contracts and three mock adapters.
- Implement deterministic timeout injection and recovery.
- Show all three device types in the UI with “SIMULATED” status.

**Exit:** automated mock tests and manual recovery walkthrough pass.

## Phase 6 — backpropagation (Day 11)

- Implement forward pass, analytic gradients, simultaneous update and training trace.
- Validate four initial gradients with central finite differences.
- Export 101 loss points (step 0 to 100) and plot them.
- Prepare handwritten derivation separately; never represent typed derivation as handwritten evidence.

**Exit:** numerical tests pass and handwritten appendix is captured by the candidate.

## Phase 7 — packaging and submission audit (Days 13–15)

- Bundle Qt runtime/plugins and all other legally redistributable runtime files.
- Build `Launch.exe` and VS Code/Antigravity launch configuration.
- Extract package to a clean path and test on a Windows machine/environment without Qt/dev tools installed; keep internet disconnected for the launch check.
- Run all automated tests; inspect outputs; record known limitations.
- Produce <=6-page report, demo video (3–5 minutes) or timestamped screenshot walkthrough, AI-use log, ownership statement, Git bundle/commit log and `CandidateID_Workbench.zip`.

**Exit:** ZIP manifest matches assignment; every completion claim maps to actual evidence.
