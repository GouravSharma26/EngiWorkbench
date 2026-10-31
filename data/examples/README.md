# Bundled example data and provenance

- `sample_tetrahedron_ascii.stl`: self-created tetrahedral mesh in ASCII STL format, 4 triangles. Coordinates are in metres by convention for this sample only; STL does not encode a universal unit.
- `synthetic_temperature_nodes.csv`: self-created 7-node synthetic dataset with the exact required column semantics. It is **not an actual ANSYS export**.
- `manifest.json`: provenance, units, record count and schema notes.

The geometry and temperature cloud are independent examples. No mesh-to-result registration is implied. These are small parser/rendering fixtures, not validation data for the thermal rod solver.