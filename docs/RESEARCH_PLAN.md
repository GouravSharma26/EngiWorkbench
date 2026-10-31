# Research and verification plan

This is an engineering prototype, not a research claim. Use external references to verify library and board-specific details, and record the source/version consulted.

## Questions to resolve before or during implementation

| Topic | Question | Verification approach | Evidence to save |
|---|---|---|---|
| Qt deployment | Which Qt runtime DLLs/plugins are required by the selected modules? | Build Release, run Qt deployment tool, inspect package dependencies, clean-machine launch | deployment log, file manifest, licence notices |
| OpenGL | Which context/profile is supported by test hardware? | Query context at runtime and test the 3D views on target Windows machine | GPU/driver info and screenshot |
| STL parser | What practical triangle/file limit is stable? | Generate files at increasing sizes and measure import time/peak memory | test table and import logs |
| CSV parser | What node count/file size is safe? | Generate structured files and measure parse/render cost | test table and memory observations |
| FE solver | Does assembly/BC treatment match the exact solution? | Unit tests and analytical errors on all four meshes | test logs, CSV and plots |
| Parallel benchmark | Is workload large enough for stable timings? | Pilot fixed workload; increase case count if medians are noisy; preserve fair comparison | pilot and final raw timings |
| Embedded | Are board/connector/voltage assumptions right for the exact revision? | Consult vendor manual, inspect board revision, review connector pinout before any physical wiring | manual links, revision notes, reviewed table |
| Licences | Can every shipped dependency be redistributed in the chosen manner? | Read licence for exact Qt and third-party versions | third-party notice and licence copies |

## Hardware reference notes used in the initial design

- Espressif's ESP32-DevKitC V4 guide describes USB-to-UART, its power choices (one at a time), 3.3V supply pin and header functions. Confirm the exact module (WROOM vs WROVER) before relying on GPIO16/17 availability.
- Digilent's Arty A7 manual describes J10 USB-JTAG/UART and the separate external 6-pin JTAG header J8; it identifies the JTAG-HS2 as an external programmer option for J8. Consult the exact board revision manual.
- Digilent's JTAG-HS2 product/reference documentation describes its target-referenced voltage and VREF requirement. VREF is a signal-level reference, not a board power output.

Reference URLs:
- https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-devkitc/user_guide.html
- https://digilent.com/reference/programmable-logic/arty-a7/reference-manual
- https://digilent.com/shop/jtag-hs2-programming-cable/
- https://digilent.com/reference/_media/reference/programmers/jtag-hs2/jtag-hs2_rm.pdf

## Research log

| Date | Question | Source / version | Finding | Decision / effect | Evidence path |
|---|---|---|---|---|---|
| TODO | TODO | TODO | TODO | TODO | TODO |
