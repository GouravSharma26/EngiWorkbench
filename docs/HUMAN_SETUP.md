# Human setup guide (developer machine)

This file records the planned development setup. Fill in exact versions after inspecting the actual Windows 11 machine.

## Required environment

- Windows 11 x64.
- Visual Studio 2022 Build Tools or Visual Studio with MSVC x64 C++ toolchain and Windows SDK.
- CMake version compatible with the chosen Qt release.
- Qt 6 installation containing Widgets and OpenGLWidgets for MSVC x64.
- Git.
- Antigravity IDE and its C++/CMake tooling as applicable.
- Optional: vcpkg (only if selected for pinned third-party dependencies).

No physical microcontroller, FPGA, JTAG adapter, commercial solver or paid AI plan is required by the assignment.

## Record actual toolchain

Run and record:

```powershell
winver
cmake --version
cl
where.exe cmake
where.exe cl
git --version
```

Also record Qt version from the selected Qt installation/kit. Do not invent a version if the command is not in PATH; use the actual Qt kit metadata.

## Configure/build (implementation must complete these commands)

Expected pattern only; actual preset and target names must be filled after CMake setup:

```powershell
cmake --preset windows-msvc-x64-release
cmake --build --preset windows-msvc-x64-release --config Release
ctest --test-dir build --build-config Release --output-on-failure
```

If the chosen preset names differ, update this document and all Antigravity run instructions.

## Antigravity / VS Code

Open the repository root. Select the documented Windows x64 kit/preset. For normal development, use the CMake configure/build commands. For the packaging requirement, select **Launch packaged Workbench** to run the already-built `Launch.exe`; do not make users build from the editor to satisfy standalone launch.

## Machine-specific information to record

```text
Windows edition/build:
CPU model:
Physical cores:
Logical processors:
RAM installed:
Compiler + version:
CMake version:
Qt version/modules:
CMake generator:
Build configuration:
Dependency manager and baseline:
Antigravity version (if available):
Editor extension prerequisites:
```
