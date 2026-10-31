# Third-Party Software Notices and Licences

This project (PASS Science Works — Unified Engineering Workbench) incorporates or links against the following third-party libraries and runtime components.

---

## 1. The Qt Company — Qt 6.8.2 (MSVC 2022 64-bit)

- **Components Used:**
  - `Qt6Core.dll`
  - `Qt6Gui.dll`
  - `Qt6Widgets.dll`
  - `Qt6OpenGLWidgets.dll`
  - `Qt6Svg.dll`
  - Platform plugin: `platforms/qwindows.dll`
  - Style plugin: `styles/qmodernwindowsstyle.dll`
  - Image formats: `imageformats/qsvg.dll`, `imageformats/qjpeg.dll`, `imageformats/qico.dll`
- **Licence:** GNU Lesser General Public License version 3 (LGPLv3) / GNU General Public License version 2 (GPLv2).
- **Compliance:**
  - Qt is dynamically linked (`workbench_app.exe` loads the Qt shared DLLs at runtime).
  - Users are permitted to re-link or replace the Qt libraries with alternative versions in accordance with LGPLv3 terms.
  - Source code of the Qt framework is available at: https://code.qt.io/cgit/qt/qt5.git/
  - The Qt Company: https://www.qt.io/

---

## 2. Microsoft Visual C++ 2022 Redistributable (x64)

- **Components Used:**
  - `MSVCP140.dll`
  - `VCRUNTIME140.dll`
  - `VCRUNTIME140_1.dll`
  - Universal C Runtime (`api-ms-win-crt-*`, `ucrtbase.dll`)
- **Licence:** Microsoft Software License Terms for Microsoft Visual Studio 2022 Redistributables.
- **Copyright:** (c) Microsoft Corporation. All rights reserved.

---

## 3. Data Examples and Synthetic Datasets

- `data/examples/sample_tetrahedron_ascii.stl`: Original ASCII STL geometry generated for this project.
- `data/examples/synthetic_temperature_nodes.csv`: Synthetic nodal temperature distribution created exclusively for software demonstration and user interface verification.
- **Declaration:** NOT an actual ANSYS export. No proprietary ANSYS project files, mesh formats, or result databases are included or required.
