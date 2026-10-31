# Final technical report outline (maximum six pages)

The assignment permits a report of at most six pages, excluding the two-page handwritten appendix. Keep the report evidence-led and concise.

## Suggested six-page structure

### Page 1 — Executive summary and architecture
- Purpose and scope.
- Main architecture diagram and core/presentation separation.
- Toolchain/dependencies and one-click design.
- Actual effort recorded.

### Page 2 — Geometry/CSV handling and interface
- Supported formats and scope exclusions.
- Parser validation/error cases and tested file limits.
- Screenshot of STL view and synthetic temperature cloud with legend/node inspect.
- Provenance and unit conventions.

### Page 3 — Thermal FEA verification
- Governing equation, assumptions, material/geometric parameters and BCs.
- Element stiffness/load formulation and Dirichlet treatment.
- Exact solution and required midpoint error table.
- Temperature and convergence plots; cite raw CSV and tests.
- One debugging insight.

### Page 4 — Multicore results
- CPU/physical/logical core/RAM/compiler/flags.
- Seed, workload, warm-up and five repetitions.
- Raw timing summary, medians, speedups, efficiencies and agreement result.
- Explain overhead and limitations; no unsupported speedup claims.

### Page 5 — Embedded design and backpropagation
- Hardware block diagram and compact interface/voltage table.
- Explain mock-only scope and JTAG role.
- Hand-coded gradients and finite-difference verification summary.
- Loss curve; reference handwritten derivation is in appendix.

### Page 6 — Testing, delivery and limitations
- Test outcomes and key manual checks.
- Clean-machine/offline `Launch.exe` result and editor run config.
- Demonstration reference and Git history.
- Known limitations, unfinished scope and future work.

## Evidence references

Use paths/IDs (for example, `results/thermal_midpoint_errors.csv`, `results/benchmark_raw_timings.csv`, `tests/`, `docs/evidence/launch-smoke-test.md`) so every quantitative or completion claim can be independently checked.

Do not include the two-page handwritten appendix in the six-page count if it is submitted separately as requested. Keep the scan legible and do not substitute a typed derivation.
