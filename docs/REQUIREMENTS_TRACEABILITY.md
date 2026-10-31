# Requirements traceability and scoring map

The source brief defines the following 100-point rubric (page 3). Shortlisting also depends on minimum scores in C++ backend, thermal FEA and delivery/report, plus a working backend and successful standalone launch.

| Criterion | Max | Required implementation / evidence | Gate |
|---|---:|---|---|
| Interface / webpage design | 8 | Coherent workflow; readable Geometry, Thermal, Performance, Hardware views; validation; responsive progress; actionable errors; useful plots; exports | — |
| C++ backend and testing | 12 | Working C++17 core; clear module boundaries; robust error handling; positive, negative and numerical tests | **At least 6/12 and working C++ backend** |
| Folder architecture | 6 | Clean folders; core/presentation separation; relative paths; controlled dependencies | — |
| Code readability | 6 | Clear names; focused functions; consistent style; useful comments; maintainable interfaces | — |
| Version control | 5 | Meaningful commits; traceable changes; sensible exclusions; dependency/version records | — |
| Thermal FEA and adoption | 14 | Correct element formulation, assembly and BCs (6); analytical verification and convergence (5); assumptions/units/learning explanation (3) | **At least 7/14** |
| CAD / ANSYS visualization | 10 | Both input types; units/legend; interaction; provenance; malformed-file handling | — |
| Multicore performance | 10 | Genuine parallel work (4); reproducible timing and agreement checks (4); honest analysis (2) | — |
| Embedded integration | 8 | Modular host-connected design and electrical assumptions (4); three mocks and failure recovery (4) | — |
| Backpropagation | 7 | Correct handwritten chain rule and update (4); C++ gradient check and loss plot (3) | — |
| Prompt engineering / AI judgement | 8 | Clear prompts, iterative refinement, independent verification, disclosed AI use and critical judgement | — |
| One-click delivery / report | 6 | Clean-machine launch and editor configuration (3); evidence/report/limitations (3) | **At least 3/6 and successful standalone launch** |
| **Total** | **100** | | |

## Task-to-file mapping

| Assignment requirement | Primary implementation area | Required evidence |
|---|---|---|
| Task A — interface, backend, launch | `src/app/`, `src/core/`, `packaging/windows/`, `.vscode/` | workflow screenshots, validation/error checks, cancellation check, exported results, clean extraction launch log |
| Task B — STL + nodal CSV | `src/core/io/`, geometry views, `data/examples/` | sample provenance, schema/units, parser tests, import screenshots, tested limits |
| Task C — thermal FEA | `src/core/thermal/`, Thermal Analysis view | code/test evidence, node CSV, midpoint error CSV, temperature and convergence plots, assumptions/units explanation |
| Task D — multicore | `src/core/performance/`, Performance view | fixed seed/config, raw five-run timings, median/speedup/efficiency, hardware/compiler/flags, agreement result |
| Task E — embedded | `src/core/embedded/`, Hardware view, docs | labelled block diagram, pin/voltage table, three simulated devices, timeout and recovery screenshot/log |
| Task F — backprop | `src/core/backprop/`, Backpropagation view | handwritten scan/photo (max two pages), gradient/finite-difference CSV or log, loss plot, C++ tests |
| ZIP/report/evidence | root, `docs/`, `results/`, `dist/` | required archive structure, six-page maximum report (handwritten appendix excluded), 3–5 minute demo or timestamped walkthrough, AI-use log, ownership statement, Git bundle/log |

## Shortlisting gates (must be treated separately from the raw score)

The brief states:
- **80–100:** strong interview shortlist.
- **50–80:** interview shortlist.
- **Below 50:** not shortlisted.
- Both shortlist bands additionally require at least **6/12 C++ backend**, **7/14 thermal FEA**, and **3/6 delivery/report**, a working C++ backend, and successful standalone launch.
- A packaging-only failure receives one clarification/repackaging opportunity within 48 hours, with no new feature work; if unresolved, the launch gate fails while the scored rubric is still returned.

Do not infer an outcome from the total alone. Record each criterion score, evidence references, gate outcome, two strengths, two improvement priorities, and exact reason.

## Evidence ledger template

| ID | Criterion | Evidence path / commit | Command or manual action | Actual outcome | Reviewer note |
|---|---|---|---|---|---|
| E-001 | C++ backend | TODO | TODO | NOT RUN | |
| E-002 | Thermal FEA | TODO | TODO | NOT RUN | |
| E-003 | CAD/CSV | TODO | TODO | NOT RUN | |
| E-004 | Parallel | TODO | TODO | NOT RUN | |
| E-005 | Embedded | TODO | TODO | NOT RUN | |
| E-006 | Backprop | TODO | TODO | NOT RUN | |
| E-007 | Launch/package | TODO | TODO | NOT RUN | |
| E-008 | Report/demo/AI log | TODO | TODO | NOT RUN | |
