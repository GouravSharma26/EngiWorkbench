# Task B — supported input formats and parser contract

## 1. Scope boundary

Required support is limited to:
1. ASCII STL geometry.
2. CSV nodal temperature data with the exact required semantic columns.

Out of scope: binary STL, STEP/IGES/native CAD solids, native ANSYS project/result formats such as `.rst`, and automatic mesh-to-result registration.

## 2. ASCII STL

### Accepted structure

- ASCII text beginning with `solid` and ending with `endsolid` (allow common whitespace and an optional solid name).
- Repeated facets containing `facet normal`, `outer loop`, exactly three `vertex x y z` rows, `endloop`, and `endfacet`.
- Floating-point coordinates/normals must parse completely and be finite.
- Reject malformed, truncated, empty, non-finite, unsupported binary, and over-limit data with actionable error messages.
- Compute or normalize display normals safely; reject degenerate zero-area triangles or explicitly report a documented policy.

### Units and provenance

STL has no universal unit field. Never infer metres or millimetres from the file. The bundled sample uses metres **by convention documented in its manifest**. UI should label the model as “STL model units” unless the user/manifest supplies a display unit. Do not silently scale geometry.

### Initial implementation limits (provisional targets)

- File size: 50 MiB maximum.
- Triangle count: 250,000 maximum.

These are design targets, not yet tested facts. The implementation must test them on the selected Windows machine, document the highest tested successful values and memory/runtime observations, and revise the cap if needed.

## 3. Temperature CSV

### Required columns

```csv
node_id,x_m,y_m,z_m,temperature_C
```

- `node_id`: unique non-negative integer identifier.
- `x_m`, `y_m`, `z_m`: finite coordinates in metres.
- `temperature_C`: finite temperature in degrees Celsius.
- Header names are required; parser may support any column order if it maps by header name.
- Rows must have the same number of fields as the header.
- Blank-only file, duplicate IDs, missing columns, malformed numeric fields, trailing junk, NaN/Inf, and truncated rows must be rejected with row/column context.
- Do not accept an empty dataset as a valid point cloud.

### Initial implementation limits (provisional targets)

- File size: 50 MiB maximum.
- Node count: 500,000 maximum.

These limits must be measured and documented, not merely hard-coded and claimed as tested.

## 4. Import UX

For each import show:
- File name and detected format.
- Record count (triangles or nodes).
- Units: model units for STL unless metadata supplied; m and °C for the required CSV.
- Data provenance (user file, self-created example, or synthetic example).
- Parser warnings, if any. Warnings must not conceal malformed data.

## 5. Error examples

- “The selected file appears to be binary STL. Export it as ASCII STL and try again.”
- “Missing required CSV column: `temperature_C`.”
- “Row 17, column `z_m` is not a finite number.”
- “Node ID 42 is repeated at rows 5 and 19.”
- “File exceeds the configured 50 MiB import limit.”
- “The CSV contains a header but no data rows.”

## 6. Parser tests

Create fixtures for valid triangle, multiple triangles, valid CSV with reordered columns, empty files, whitespace-only files, missing header, duplicate IDs, short/long rows, quoted field edge cases if quoting is supported, invalid number, NaN, Inf, overflow, truncated STL facet and binary STL signature. Confirm the parser does not crash or allocate unbounded memory.

## 7. CSV export conventions

- UTF-8 text, comma delimiter, one header row, invariant decimal point, LF or CRLF accepted by common tools.
- Use explicit column names and units in headers.
- Avoid locale-dependent decimal commas.
- Write to a temporary file and replace the destination only after successful serialization.
