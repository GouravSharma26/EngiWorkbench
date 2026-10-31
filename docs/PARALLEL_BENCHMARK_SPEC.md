# Task D — multicore benchmark specification

## 1. Required behavior

Run a batch of independent steady thermal solves in two modes:
- Serial: one case solved at a time.
- Parallel: a bounded C++ worker pool using `std::thread` (or OpenMP if deliberately selected).

Both modes must call the same solver, consume the same ordered cases and produce results in deterministic input order. This is real CPU parallel work, not simply running the UI in a background thread.

## 2. Workload generation

- Use `std::mt19937` with a fixed, recorded seed.
- Generate a reproducible vector of independent cases before timing.
- Vary positive `k` and positive `q`; proposed default ranges are `k ∈ [5,25] W/(m·K)` and `q ∈ [1,000,20,000] W/m³` (make ranges visible/configurable and record the chosen values).
- Keep geometry, area, boundary conditions, mesh size and solver method consistent within a benchmark run; case values must be exactly identical between serial and parallel modes.
- Proposed default: 10,000 cases and 200 elements per case. These are tunable starting values, not a promised performance target. Adjust workload until measurements are stable enough to report, and disclose the exact workload.
- Reject zero/negative/non-finite k and invalid case count/mesh settings.

## 3. Worker counts

Measure `p = 1, 2, 3, 4` where the count is available, capped at available logical CPUs. Deduplicate counts when a machine has fewer logical CPUs. Always include one-worker baseline. On a single-logical-CPU machine, still invoke and demonstrate the parallel code path and disclose that speedup cannot be assessed.

## 4. Measurement protocol

1. Generate cases outside the timed region.
2. Run one unmeasured warm-up for each mode/worker count.
3. Run five measured repetitions for each configuration using `std::chrono::steady_clock`.
4. Time compute only. Exclude file loading, case generation, UI rendering, CSV export and compilation.
5. Record all five raw times; compute median time.
6. `speedup = median(T1) / median(Tp)`.
7. `efficiency = speedup / p`, where `p` is the actual number of workers used.
8. Keep raw per-case results or a deterministic checksum; verify full serial/parallel agreement.
9. Verify `max_i |T_serial_i - T_parallel_i| <= 1e-8 °C` over the comparable output temperatures.
10. Report application launch-to-ready time separately; do not mix it with compute timing.

## 5. Hardware/build metadata

Capture:
- CPU exact model.
- Physical core count and logical processor count. On Windows, use a documented OS API such as `GetLogicalProcessorInformationEx`; report unknown if detection fails.
- Installed RAM (e.g. `GlobalMemoryStatusEx`) and whether this is total or available memory.
- OS version.
- Compiler name/version and architecture.
- Optimization flags and build configuration (e.g. Release `/O2`, if that is what was actually built).
- Qt/CMake versions if relevant to runtime/launch measurement.
- Seed, case count, elements per case, k/q ranges, boundary settings and solver.
- Warm-up procedure and five raw repetition values.

Do not assume `std::thread::hardware_concurrency()` equals physical cores; it normally reports logical concurrency and may return zero.

## 6. Cancellation and data safety

- Workers stop taking new tasks after cancellation is requested.
- Wait for workers to join cleanly.
- Do not publish a partial batch as a complete benchmark.
- Preserve the previous successful result if the new run is cancelled or fails.
- Store each case result at its input index, not completion order.
- Capture worker exceptions and return a controlled error rather than terminating the app.

## 7. Output files

- `benchmark_cases.csv`: the generated deterministic cases.
- `benchmark_raw_timings.csv`: one row per worker count and repetition, including mode, p, repetition, wall-clock milliseconds.
- `benchmark_summary.csv`: median time, speedup, efficiency, p and workload.
- `benchmark_agreement.csv` or JSON: max absolute serial/parallel temperature difference and pass/fail.
- `benchmark_environment.txt` or JSON: CPU, physical/logical cores, RAM, OS, compiler, flags, seed and workload.

## 8. Interpretation guidance

Do not promise a minimum speedup. Small workloads can be slower in parallel because scheduling, synchronization, memory traffic and pool startup have costs. Larger case counts improve amortization but do not guarantee linear scaling. Explain actual observations and limitations honestly; do not cherry-pick a single favourable repetition.
