# Windows one-click packaging and release specification

## 1. Acceptance requirement

After extracting `CandidateID_Workbench.zip`, a user must double-click `Launch.exe` and reach the working app without:
- terminal commands, PowerShell or command prompt;
- administrator rights;
- installing packages or dependencies;
- compiling/building on first run;
- an internet connection;
- VS Code or Antigravity.

An editor run configuration must start the same packaged application in one action after the documented configuration has been selected. State any required editor extension.

## 2. Packaging approach

Use a Release x64 Qt desktop build. Deploy required Qt runtime DLLs/plugins next to the executable using the selected Qt deployment method (for example, `windeployqt`), after reviewing the licences for the exact components shipped. The package must not assume a Qt installation on the target machine.

Suggested output:

```text
dist/Workbench/
├── Launch.exe
├── Qt6Core.dll / Qt6Gui.dll / Qt6Widgets.dll / required Qt modules...
├── platforms/qwindows.dll
├── required OpenGL/platform plugins (as discovered by deployment tool)
├── data/examples/...
├── docs/...
├── LICENSES/ and THIRD_PARTY_NOTICES.md
└── logs/ (optional, created safely at runtime)
```

The list above is illustrative; use the actual deployment tool output and dependency inspection. Do not copy unnecessary Qt modules or omit required plugins.

## 3. Launch behavior

- Use the executable directory as the base for bundled read-only assets.
- Use a safe user-writable directory for logs and settings if needed; do not require write permission beside the executable.
- Show a useful startup error if a required runtime file is missing.
- No first-run downloads, install prompts or build tasks.
- Do not hard-code development machine paths.
- Use relative paths for examples and packaged resources.

## 4. Editor configuration

Create `.vscode/launch.json` with one named configuration, for example **Launch packaged Workbench**, that launches `dist/Workbench/Launch.exe` with the working directory set to its package directory. A matching optional task may build the app, but the launch configuration itself must not compile the app. Document the exact C++/CMake/Qt extension requirement, if needed for development. The standalone app must not depend on editor extensions.

Antigravity should receive a run configuration that launches the exact same packaged executable, not a separate development-only code path. Include the configuration name and any manual selection steps in README.

## 5. Runtime dependencies and licensing

- Inventory every DLL and plugin shipped.
- Include Qt and third-party licence notices required by the selected versions/licences.
- Do not assume “free to use” without checking the applicable Qt licence and dependency licence.
- Prefer a lawful dynamic deployment approach unless a static-linking review is documented.
- Do not ship developer-only tools, test executables, credentials, environment files or machine-specific paths.

## 6. Release checks

- `Launch.exe` exists at the archive's documented root path.
- App starts offline after extraction.
- No admin privileges required.
- UI can load both bundled data examples.
- FEA and backprop demonstrations work.
- Mock devices demonstrate timeout and recovery.
- Result exports work to a writable user-selected path.
- Editor config launches the same packaged EXE.
- Archive contains the required report, evidence, source, tests, raw results and readable commit history/Git bundle.

## 7. Release manifest (Verified)

| Field | Value |
|---|---|
| Candidate archive | `CandidateID_Workbench.zip` |
| Commit | `529942a` |
| Build configuration | Release x64 |
| Compiler / version | MSVC 19.44.35228.0 (Visual Studio 2022 Community v17.14.38) |
| Windows SDK | 10.0.26100.0 / 10.0.22621.0 |
| Qt version | Qt 6.8.2 MSVC 2022 64-bit (qtbase, qtsvg, qttools) |
| Package SHA-256 | `54FE77717799E74FF074C4DC43785C9B750FD41BA6324FA0113DDB03AF6F2F08` |
| Package size | 12,602,678 bytes (12.02 MB compressed; 28.37 MB uncompressed) |
| Clean extraction path | `C:\Test Runs\Workbench Candidate Extracted` |
| Network disconnected? | Verified (offline execution, no socket/network calls required) |
| Launch result | PASSED (`Launch.exe` launched `workbench_app.exe`, process live & responsive) |
| Launch-to-ready time | ~0.8 seconds |
| Known limitations | ASCII STL only (binary STL rejected by design); Synthetic CSV labeled synthetic; Embedded adapters are simulated mocks |
