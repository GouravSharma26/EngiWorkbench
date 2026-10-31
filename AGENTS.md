# Instructions for Antigravity / coding agents

## Mission
Implement the PASS Science Works Unified Engineering Workbench described in this repository. Treat the supplied assignment brief and the specification files as the source of truth. The goal is a small, reliable, demonstrably correct prototype—not a feature-heavy production platform.

## Mandatory reading order
1. `README.md`
2. `plan.md`
3. `docs/ARCHITECTURE.md`
4. `docs/MODEL_AND_TECH_DECISIONS.md`
5. `docs/REQUIREMENTS_TRACEABILITY.md`
6. The task-specific specification before changing that task
7. `docs/DEVELOPMENT_CHECKLIST.md` and `docs/VALIDATION_PROTOCOL.md`

## Non-negotiable constraints
- C++17 or later implements **all application backend logic, numerical computation, file processing, and concurrency**.
- Keep `src/core` independent of Qt and presentation code. Do not put numerical or parsing logic in widgets.
- Use the same thermal solver for the FEA view and batch benchmark. Serial and parallel paths must use identical input cases and numerical methods.
- The batch implementation must do real CPU-parallel work using a bounded worker pool or standard threads/OpenMP. UI background execution alone is not the parallel feature.
- Preserve deterministic input order in batch output. Verify serial/parallel maximum absolute temperature difference is `<= 1e-8 °C`.
- The final Windows package must run from an extracted ZIP by double-clicking `Launch.exe`, offline, without terminal commands, admin rights, first-run compilation, or package installation.
- The UI must remain responsive during computation. Cancellation must not publish partial/corrupt result sets.
- Mark the supplied temperature CSV as synthetic. Never describe it as an actual ANSYS export.
- Only ASCII STL is in required scope. Do not imply binary STL, native CAD solids, or native ANSYS result/project files are supported.
- Hardware adapters are mocks. Never imply that physical hardware was detected, powered, connected, debugged, or programmed.
- The JTAG adapter is a debug/programming interface, not a generic compute board.
- Disclose AI assistance and reused code. Do not fabricate prompts, tool names, tests, results, commit history, or manual work.

## Working method
- Before each implementation increment, state the relevant requirements, planned files, and acceptance checks in `docs/WORKLOG.md`.
- Implement one vertical slice at a time. Prefer a small working feature with tests to broad, unverified scaffolding.
- Inspect existing code before editing. Do not overwrite user work or rewrite unrelated modules.
- Keep public interfaces focused and typed. Use explicit units in names and comments where values cross module boundaries.
- Validate finite numeric inputs, positive physical parameters, ranges, empty files, malformed records, overflow, and cancellation paths.
- Use relative paths based on the executable/repository location. Never embed the developer machine's absolute paths or credentials.
- Use meaningful commits. Do not make empty commits or optimize for commit count.
- Update the relevant docs whenever implementation behavior, dependency versions, limits, or decisions change.
- For every claim in a report, point to a test, raw result, screenshot, log, or source decision.

## Numerical correctness rules
- FEA: assemble two-node linear element stiffness and consistent load terms; impose both Dirichlet temperatures correctly; compare FE interpolation to the analytical temperature at element midpoints.
- Backpropagation: calculate all gradients from the original parameter values before applying any updates; do not update parameters sequentially inside a single gradient step.
- Finite differences: use central differences with epsilon `1e-6` and absolute gradient error `<= 1e-5`.
- Parallel benchmarks: fixed seed, identical cases, one warm-up, five timed repetitions, median, speedup `T1/Tp`, efficiency `(T1/Tp)/p`. Exclude file loading, rendering, and compilation.

## UI and error handling
- Required views: Geometry, Thermal Analysis, Performance, Hardware. Add a Backpropagation view for Task F.
- Provide input validation, progress/status, actionable errors, result export, and cancel or safe completion.
- Distinguish `Idle`, `Validating`, `Running`, `Cancelling`, `Completed`, `Cancelled`, and `Failed` states.
- Do not block the GUI thread with solver, parser, or benchmark work.
- Present units and provenance beside imported and generated data.

## Testing expectations
- Add positive, negative, boundary, and numerical tests for each core module.
- Tests must run without physical hardware and without internet access.
- Run the narrowest relevant test after each change, then the full CTest suite before integration commits.
- Record exact commands, exit codes, toolchain version, test counts, and failures. Do not say “passed” unless the command actually ran successfully.

## Completion definition
A task is complete only when its acceptance criteria in `docs/REQUIREMENTS_TRACEABILITY.md` are implemented and supported by reproducible evidence. A UI control without a real core action is not complete. A mocked hardware interaction must say “simulated.” A package that only works on the developer machine is not complete.
