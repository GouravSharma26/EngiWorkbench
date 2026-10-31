# Core implementation area

Implement the Qt-independent C++17 domain, file parsing, thermal FE solver, worker pool, embedded mocks and backpropagation here. Suggested public headers live under `include/workbench/`; implementation files live under `src/`. Keep all backend behavior in this layer. Add the real CMake target in the repository root during Phase 0.