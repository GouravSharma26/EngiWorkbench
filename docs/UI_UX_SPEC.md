# UI / UX specification

## 1. Navigation

Use one coherent desktop window with these views:
1. **Geometry** — import/view ASCII STL and CSV temperature cloud.
2. **Thermal Analysis** — inputs, solve, exact comparison, mesh convergence, CSV/plots export.
3. **Performance** — workload configuration, serial/parallel run, worker selection, timing table and agreement.
4. **Hardware** — simulated MCU/FPGA/JTAG devices, capabilities, connection state, telemetry and fault recovery.
5. **Backpropagation** — initial values, forward pass, analytic gradients, finite-difference check, one-step update and 100-step loss plot.

The four views named in Task A are mandatory; Backpropagation is added for Task F.

## 2. Consistent page structure

Each view should present:
- Page title and concise purpose.
- Input/configuration area, with labels and units.
- Primary action and disabled/enabled state.
- Progress/status row and cancellation while running.
- Result region (plot/table/viewer).
- Export action when the view produces data.
- Clear validation or failure message with correction guidance.

Avoid clutter, decorative metrics without evidence, and controls that do not invoke a working feature.

## 3. Geometry view

- Two clear import actions: `Open ASCII STL` and `Open temperature CSV`.
- STL viewport: rotate/orbit, pan, zoom, fit-to-view, reset camera; show triangle count and bounding box.
- CSV viewport: 3D point cloud, stable color mapping from low to high temperature, legend with °C, node count.
- Node inspection: click nearest visible node and show node ID, x/y/z in metres and temperature in °C.
- Show provenance and unit note; STL units are not embedded in the standard format.
- Explain binary STL is unsupported rather than silently treating it as ASCII.

## 4. Thermal Analysis view

- Display all default parameter values and units.
- Validate before starting; do not accept NaN/Inf, nonpositive L/A/k, zero element count or unreasonable mesh size.
- Default run must include N=10,20,40,80.
- Plot FE and exact curves; show maximum midpoint error and convergence table.
- Export raw nodal and midpoint CSV files.
- Include a short assumptions panel: steady, 1D, uniform rod, constant properties, insulated lateral surface, uniform internal heat generation and prescribed endpoint temperatures.

## 5. Performance view

- Inputs: fixed seed, case count, elements/case, k range, q range and worker count(s).
- Show actual logical CPU cap; include 1,2 and available up to 4 workers.
- Clearly distinguish setup/generation from timed compute.
- Show five raw repetitions, median, speedup and efficiency.
- Show max serial/parallel difference and pass/fail at `1e-8 °C`.
- Display environment and workload metadata with every exported report.

## 6. Hardware view

- Display three devices as `SIMULATED`.
- Provide discover, show capabilities, connect/disconnect and telemetry actions.
- Include one deterministic timeout demonstration and retry/reconnect.
- Include an electrical assumptions summary and “verify exact revision before physical wiring” warning.
- Do not show real COM ports or claim physical device enumeration unless a separate, explicitly out-of-scope implementation is added and verified.

## 7. Backpropagation view

- Show x, target, w, b, v, c and learning rate with defaults.
- Display h, prediction, error and loss from the forward pass.
- Display four analytic gradients and four finite-difference results.
- Button for one simultaneous update and separate 100-step training/loss plot.
- Export gradient check and loss history.

## 8. Responsiveness, state and accessibility

- No core solver, parser or benchmark executes on the GUI thread.
- Status states: Idle, Validating, Running, Cancelling, Completed, Cancelled, Failed.
- During a job, disable only controls that would invalidate its inputs; allow the user to inspect other tabs if safe.
- Cancellation must preserve the last completed result.
- Use readable font sizes, clear focus indication, keyboard-accessible buttons and high-contrast text.
- Always show units in labels/table headers, not only in hover text.
