# Development checklist

Use `[ ]`, `[x]`, or `[blocked]`. Only mark an item complete after its evidence exists.

## Repository and build
- [ ] CMake project builds on Windows x64 with C++17.
- [ ] Core target has no Qt dependency.
- [ ] Qt app target builds and launches in development mode.
- [ ] Exact compiler, CMake, Qt and dependency versions recorded.
- [ ] CTest test target configured; tests run in Release and Debug as appropriate.

## Task A — UI / integration
- [ ] Geometry, Thermal Analysis, Performance, Hardware views present.
- [ ] Backpropagation view present.
- [ ] File selection and numeric validation implemented.
- [ ] Progress and all job states implemented.
- [ ] Actionable error messages shown.
- [ ] UI remains responsive during compute and import.
- [ ] Cancellation preserves prior complete results.
- [ ] Result exports work.
- [ ] Core and presentation boundaries reviewed.

## Task B — imports
- [ ] ASCII STL parser implemented in C++.
- [ ] Nodal temperature CSV parser implemented in C++.
- [ ] Self-created STL example and provenance included.
- [ ] Synthetic CSV example clearly labelled synthetic.
- [ ] Units and schema/export notes documented.
- [ ] Rotate/pan/zoom/fit-to-view work.
- [ ] Temperature legend and node inspection work.
- [ ] Empty/malformed/non-finite/over-limit cases handled.
- [ ] Tested file-size/record limits recorded.

## Task C — thermal FEA
- [ ] Two-node linear element stiffness assembled.
- [ ] Uniform volumetric generation load assembled consistently.
- [ ] Both prescribed temperatures imposed correctly.
- [ ] N=10,20,40,80 solved.
- [ ] Exact analytical solution calculated.
- [ ] FE midpoint interpolation used for maximum absolute error.
- [ ] Mesh convergence plotted and table exported.
- [ ] Temperature plot includes units.
- [ ] Raw nodal/midpoint CSV exported.
- [ ] Assumptions, units, BCs and debugging insight documented.

## Task D — parallel workload
- [ ] Seeded generator creates positive varied k and q.
- [ ] Case count and mesh size configurable.
- [ ] Serial and parallel use the same case vector and solver.
- [ ] Bounded worker pool implemented in C++.
- [ ] Deterministic input-order outputs verified.
- [ ] 1,2, up to 4 workers benchmarked, capped by logical CPU count.
- [ ] Warm-up and five repetitions recorded.
- [ ] Median/speedup/efficiency calculated.
- [ ] CPU, physical/logical cores, RAM, compiler and flags recorded.
- [ ] Timings exclude I/O/render/compile.
- [ ] Max serial/parallel difference <= 1e-8 °C.
- [ ] Launch-to-ready time reported separately.
- [ ] Overhead and limits explained honestly.

## Task E — embedded
- [ ] MCU, FPGA and JTAG adapter selected and documented.
- [ ] Block diagram labelled.
- [ ] Power/ground/voltage/level shifting/connectors/host links covered.
- [ ] JTAG ownership and pinout assumptions documented.
- [ ] JTAG explicitly described as debug/programming adapter, not compute board.
- [ ] Three C++ mocks implement discover/capability/connect/disconnect/telemetry.
- [ ] UI demonstrates all three with SIMULATED labels.
- [ ] One recoverable timeout demonstrated.
- [ ] Safe plan and limitations documented.

## Task F — backpropagation
- [ ] Handwritten forward pass and chain rule prepared.
- [ ] Initial gradients and simultaneous update calculated.
- [ ] C++ calculation implemented without autodiff.
- [ ] Central finite-difference check uses epsilon 1e-6.
- [ ] All gradient absolute errors <= 1e-5.
- [ ] Loss recorded over 100 subsequent steps.
- [ ] Loss plot and export included.
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
