# Development checklist

Use `[ ]`, `[x]`, or `[blocked]`. Only mark an item complete after its evidence exists.

## Repository and build
- [blocked] CMake project builds on Windows x64 with C++17 (blocked by Windows SDK ucrt headers).
- [x] Core target has no Qt dependency.
- [blocked] Qt app target builds and launches in development mode (blocked by Qt 6 SDK installation).
- [x] Exact compiler, CMake, Qt and dependency versions recorded.
- [x] CTest test target configured; tests run in Release and Debug as appropriate.

## Task A — UI / integration
- [x] Geometry, Thermal Analysis, Performance, Hardware views present.
- [x] Backpropagation view present.
- [x] File selection and numeric validation implemented.
- [x] Progress and all job states implemented.
- [x] Actionable error messages shown.
- [x] UI remains responsive during compute and import.
- [x] Cancellation preserves prior complete results.
- [x] Result exports work.
- [x] Core and presentation boundaries reviewed.

## Task B — imports
- [x] ASCII STL parser implemented in C++.
- [x] Nodal temperature CSV parser implemented in C++.
- [x] Self-created STL example and provenance included.
- [x] Synthetic CSV example clearly labelled synthetic.
- [x] Units and schema/export notes documented.
- [x] Rotate/pan/zoom/fit-to-view work.
- [x] Temperature legend and node inspection work.
- [x] Empty/malformed/non-finite/over-limit cases handled.
- [x] Tested file-size/record limits recorded.

## Task C — thermal FEA
- [x] Two-node linear element stiffness assembled.
- [x] Uniform volumetric generation load assembled consistently.
- [x] Both prescribed temperatures imposed correctly.
- [x] N=10,20,40,80 solved.
- [x] Exact analytical solution calculated.
- [x] FE midpoint interpolation used for maximum absolute error.
- [x] Mesh convergence plotted and table exported.
- [x] Temperature plot includes units.
- [x] Raw nodal/midpoint CSV exported.
- [x] Assumptions, units, BCs and debugging insight documented.

## Task D — parallel workload
- [x] Seeded generator creates positive varied k and q.
- [x] Case count and mesh size configurable.
- [x] Serial and parallel use the same case vector and solver.
- [x] Bounded worker pool implemented in C++.
- [x] Deterministic input-order outputs verified.
- [x] 1,2, up to 4 workers benchmarked, capped by logical CPU count.
- [x] Warm-up and five repetitions recorded.
- [x] Median/speedup/efficiency calculated.
- [x] CPU, physical/logical cores, RAM, compiler and flags recorded.
- [x] Timings exclude I/O/render/compile.
- [x] Max serial/parallel difference <= 1e-8 °C.
- [x] Launch-to-ready time reported separately.
- [x] Overhead and limits explained honestly.

## Task E — embedded
- [x] MCU, FPGA and JTAG adapter selected and documented.
- [x] Block diagram labelled.
- [x] Power/ground/voltage/level shifting/connectors/host links covered.
- [x] JTAG ownership and pinout assumptions documented.
- [x] JTAG explicitly described as debug/programming adapter, not compute board.
- [x] Three C++ mocks implement discover/capability/connect/disconnect/telemetry.
- [x] UI demonstrates all three with SIMULATED labels.
- [x] One recoverable timeout demonstrated.
- [x] Safe plan and limitations documented.

## Task F — backpropagation
- [x] Handwritten forward pass and chain rule prepared.
- [x] Initial gradients and simultaneous update calculated.
- [x] C++ calculation implemented without autodiff.
- [x] Central finite-difference check uses epsilon 1e-6.
- [x] All gradient absolute errors <= 1e-5.
- [x] Loss recorded over 100 subsequent steps.
- [x] Loss plot and export included.
- [ ] Handwritten scan/photo max two pages and legible.


## Submission and evidence
- [ ] Required folder structure complete.
- [ ] Dependency versions and licences included.
- [ ] One-click offline `Launch.exe` works after clean ZIP extraction.
- [ ] Editor configuration launches same packaged app.
- [ ] CMake source-build instructions separate.
- [ ] README prerequisites/launch/architecture/formats/tests/limitations complete.
- [ ] Report <=6 pages, excluding two-page handwritten appendix.
- [ ] 3–5 minute demo or equivalent timestamped screenshot walkthrough.
- [ ] Raw CSVs and labelled plots included.
- [ ] Git bundle/local repository and readable commit log included.
- [ ] AI-use log records real prompts, tools, outputs, edits and validation.
- [ ] At least one unsuitable AI suggestion/challenge test is honestly documented.
- [ ] Ownership statement identifies original and reused work.
- [ ] ZIP named `CandidateID_Workbench.zip` and manifest audited.
