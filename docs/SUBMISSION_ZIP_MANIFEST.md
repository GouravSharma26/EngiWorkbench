# Final submission ZIP manifest

Archive name: `CandidateID_Workbench.zip`

Required contents from the assignment:

```text
CandidateID_Workbench/
├── Launch.exe
├── [all packaged runtime files]
├── src/                       # frontend/presentation and C++ backend source
├── CMakeLists.txt
├── [dependency lock/version information]
├── .vscode/ or equivalent editor run configuration
├── data/                      # examples and provenance
├── tests/
├── results/                   # raw CSV and labelled plots
├── docs/
├── README.md
├── [Git bundle or local repository]
├── [readable commit log]
├── [AI-use log]
├── [ownership statement]
└── [report <= 6 pages + separate handwritten appendix <= 2 pages]
```

Also include a 3–5 minute demonstration video or a timestamped screenshot walkthrough with logs. Keep relative paths. Exclude credentials, personal data, machine-specific absolute paths, transient build trees, unrelated files and secrets.

## Pre-archive audit

- [ ] Archive extracts without path errors.
- [ ] Launch.exe is at the expected root/path and runs offline.
- [ ] All required runtime files and licences are present.
- [ ] CMake source-build instructions are separate from the one-click launch steps.
- [ ] Source, tests, examples, provenance, raw results and plots are included.
- [ ] Report is no more than six pages, excluding handwritten appendix.
- [ ] Demo is 3–5 minutes or screenshot equivalent is timestamped and logged.
- [ ] Git bundle/repository and readable commit log are included.
- [ ] AI-use log and ownership statement are complete and truthful.
- [ ] No credentials, personal data or developer absolute paths.
- [ ] ZIP hash and size recorded.
