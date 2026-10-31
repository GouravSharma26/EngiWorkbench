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

## 7. Release manifest template

| Field | Value |
|---|---|
| Candidate archive | `CandidateID_Workbench.zip` |
| Commit | TODO |
| Build configuration | TODO |
| Compiler/version | TODO |
| Qt version | TODO |
| Package SHA-256 | TODO |
| Package size | TODO |
| Clean extraction path | TODO |
| Network disconnected? | TODO |
| Launch result | NOT RUN |
| Launch-to-ready time | NOT MEASURED |
| Known limitations | TODO |
