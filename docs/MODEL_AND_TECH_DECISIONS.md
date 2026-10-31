# Model and technology decisions

**Decision status:** Initial recommendations. Antigravity must validate the actual installed toolchain and record final versions before implementation is considered reproducible.

| ID | Decision | Rationale | Constraints / revisit trigger |
|---|---|---|---|
| ADR-001 | C++17 baseline for core and app integration | Meets the explicit C++17-or-later backend requirement and broad MSVC support | Do not use C++20-only APIs unless the project explicitly raises its language baseline and records why |
| ADR-002 | Qt 6 Widgets desktop UI | Direct Windows desktop UX and practical single-app packaging | Must bundle runtime DLLs and Qt plugins lawfully; verify launch on a machine without Qt installed |
| ADR-003 | Qt-independent `workbench_core` library | Enforces presentation/core separation and makes numerical tests independent of UI | No `QObject`, `QString`, `QFile`, or Qt container types in public core interfaces |
| ADR-004 | Custom small OpenGL viewer in `QOpenGLWidget` | Covers triangle and point-cloud interactions without adding a large 3D engine | Implement only camera controls, fit-to-view, triangle display, color mapping and node picking needed for the rubric |
| ADR-005 | `QPainter` plots | Sufficient for required 2D curves and legends | Export raw CSV; plots are not the source of numerical truth |
| ADR-006 | Standard-library worker pool | Genuine C++ CPU concurrency with bounded worker count and no OpenMP toolchain dependency | Handle queue shutdown, cancellation, exceptions, deterministic ordering and no oversubscription |
| ADR-007 | Tridiagonal FE solve for the 1D uniform rod | The assembled 1D linear-element matrix is tridiagonal; efficient and easy to verify | Keep assembly explicit so stiffness/load/BC logic is inspectable; test against analytic solution |
| ADR-008 | Self-contained parsers for scoped formats | Scope is small: ASCII STL and one CSV schema | Do not claim broad CAD/ANSYS format support; parser must be defensive and size-limited |
| ADR-009 | Mock-only embedded adapters | Physical hardware and firmware are explicitly not required | All UI labels, logs, screenshots and report statements must say simulated |
| ADR-010 | No runtime network dependency | The standalone package must run offline | Build-time dependency acquisition is separate; bundle required runtime components |
| ADR-011 | Dynamic Qt deployment unless licensing review approves another method | Simplifies LGPL compliance and Qt deployment tooling | Bundle applicable notices and DLLs; verify Qt plugin path and licences for exact Qt version |
| ADR-012 | Test framework selected at setup (Catch2 v3 or GoogleTest) | Enables CTest-based regression and numerical tests | Pin exact release/commit and licence; keep it out of the release package |

## Build/toolchain decision record to fill

| Item | Actual selected value | Verification command / evidence | Status |
|---|---|---|---|
| Windows version / build | TODO | `winver` / system info screenshot | NOT VERIFIED |
| Compiler / version | TODO | `cl` / compiler output | NOT VERIFIED |
| CMake version | TODO | `cmake --version` | NOT VERIFIED |
| Qt version and modules | TODO | Qt installation metadata / build output | NOT VERIFIED |
| Test framework/version | TODO | dependency manifest | NOT VERIFIED |
| Dependency manager/baseline | TODO | manifest / lock file | NOT VERIFIED |
| Build configuration | Release / x64 / TODO flags | CMake configure/build log | NOT VERIFIED |
| Qt deployment method | TODO | packaging script and clean-machine launch | NOT VERIFIED |
| Licence notices | TODO | `THIRD_PARTY_NOTICES.md` | NOT VERIFIED |

## Dependency policy

1. Prefer the C++ standard library and Qt already required for UI.
2. Before adding a library, explain the problem it solves, why existing tools are insufficient, its version, licence, runtime impact and deployment plan.
3. Pin exact versions in `vcpkg.json`/baseline or another lock-capable manifest. Never depend on a floating `latest` version.
4. Do not use online services or paid solver licences.
5. Runtime package must not download or install dependencies.
6. Generate a third-party notice file from the dependencies actually shipped; do not list libraries that are merely considered.

## Known design risks

- Qt runtime/plugin packaging on a clean Windows machine.
- OpenGL driver/context compatibility on test machines.
- Large imported geometry/CSV memory footprint.
- A very fast batch workload may produce noisy timings; allow workload size to be tuned and report it.
- Hardware voltage and connector details vary by revision; physical wiring is not part of the prototype. Use the exact board manuals before any real connection.
