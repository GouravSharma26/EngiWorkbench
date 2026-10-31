# Verification and validation protocol

## 1. General test rules

- Test the C++ core independently from the Qt UI.
- Use deterministic test data and fixed seeds.
- Include positive, negative, boundary and numerical tests.
- Automated tests must not require a physical board or internet access.
- Store exact commands, compiler/build config, output, exit code and test count.
- Keep the old result if a new run is cancelled or fails.

## 2. Core module test matrix

| Module | Positive tests | Negative / boundary tests |
|---|---|---|
| ASCII STL | one facet, multiple facets, whitespace/name, valid normals | empty, binary signature, missing endfacet, wrong vertex count, nonfinite coordinate, degenerate triangle, over-limit file/count |
| CSV parser | required header, reordered columns, valid multiple nodes | empty, missing column, duplicate ID, short/long row, malformed number, NaN/Inf, overflow, invalid encoding if handled |
| Thermal FE | all required N meshes, boundary values, exact nodal values, midpoint error table | k/A/L <= 0, N=0, non-finite input, invalid pivot, cancellation |
| Worker pool | deterministic order, serial/parallel same outputs, counts 1..4, seeded generator repeatability | zero workers, worker count > cap, task exception, cancellation during batch, empty/invalid case set |
| Embedded mocks | discovery, capability, connect, telemetry, disconnect | injected timeout, retry/reconnect, duplicate connect, telemetry while disconnected |
| Backprop | forward pass, analytic gradients, update, loss trace | non-finite parameter, invalid epsilon, central difference mismatch |
| Export | valid CSV/plot metadata, invariant decimal separator | invalid path, write failure, partial write, destination preservation |

## 3. Required numerical acceptance criteria

- Thermal boundary values exactly satisfy prescribed Dirichlet data within implementation tolerance.
- FE nodal solution matches analytical nodal values within a documented tolerance.
- Midpoint errors are approximately 1.25, 0.3125, 0.078125 and 0.01953125 °C for N=10,20,40,80.
- Midpoint error ratios are approximately 4 under each mesh refinement.
- Serial/parallel maximum absolute output difference `<= 1e-8 °C`.
- Backprop initial analytic gradients differ from central differences by at most `1e-5` absolute for epsilon `1e-6`.

## 4. UI manual test cases

| ID | Action | Expected result |
|---|---|---|
| UI-01 | Launch from extracted folder by double-clicking `Launch.exe` | Window opens without terminal, admin prompt, build or internet |
| UI-02 | Import bundled ASCII STL | Mesh displays; rotate/pan/zoom/fit work; triangle count visible |
| UI-03 | Import bundled synthetic temperature CSV | Colored cloud and °C legend display; clicking node shows metadata; synthetic label visible |
| UI-04 | Import malformed CSV | Actionable error identifies row/column; app remains usable |
| UI-05 | Run thermal mesh set | Curves, convergence table, midpoint errors and CSV export appear |
| UI-06 | Cancel a long benchmark | UI remains responsive; partial run is not marked complete; prior result remains |
| UI-07 | Run serial and parallel benchmark | Five reps and summary visible; agreement threshold shown |
| UI-08 | Discover three mock devices | All three appear with `SIMULATED` labels and capabilities |
| UI-09 | Inject timeout then retry | Timeout shown; recovery succeeds without restarting application |
| UI-10 | Run backprop gradient check | Four gradients meet `1e-5` tolerance; loss curve available |
| UI-11 | Export each result | File is written to user-selected path with units/schema headers |
| UI-12 | Close/reopen app | No stale busy state or corrupted output; previous exports remain intact |

## 5. Clean-package test

1. Build Release x64.
2. Create the final package and include applicable runtime DLLs, Qt plugins, licence notices and required data.
3. Disconnect network.
4. Copy/extract ZIP to a fresh directory path with spaces (e.g. `C:\Test Runs\Workbench Candidate`).
5. Launch by double-clicking `Launch.exe`.
6. Run smoke tests UI-02, UI-03, UI-05, UI-08, UI-10.
7. Check that no development tools, environment variables, source tree or installed Qt are required.
8. Record OS, hardware, exact package hash, start/ready time, log and screenshots.

Do not call this a clean-machine test if it was run from the developer's build tree or if Qt/dev tools were installed and required.

## 6. Result record template

```text
Date/time:
Commit:
Machine / OS:
Compiler and version:
Build configuration:
Command / manual steps:
Expected result:
Actual result:
Exit code:
Test count:
Pass / fail / skipped:
Log path:
Screenshot path:
Known limitation:
```
