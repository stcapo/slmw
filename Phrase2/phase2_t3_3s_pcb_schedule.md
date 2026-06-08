# Phase 2 T3-3S PCB Schedule and Collaboration Plan

> Updated: 2026-06-08  
> Owner: project lead / PM reviewer  
> PCB engineer: department intern / PCB implementer  
> Phase 2 target: T3-3S + 12/24V input + dual-channel CCT low-side MOSFET control PCB

## 1. Locked Scope

Phase 2 is a first custom low-voltage control PCB, not a final mass-production board.

In scope:

- T3-3S Wi-Fi/BLE module as the Phase 2 main control module.
- 12V and 24V constant-voltage DC input.
- 3.3V Buck supply for T3-3S and small-signal circuitry.
- Two low-side N-MOSFET channels for common-anode CCT LED loads.
- WW/CW PWM control using T3-3S hardware PWM pins.
- Test points, readable silkscreen, and bring-up-friendly layout.
- Gerber, BOM, pick/place file, schematic PDF, PCB screenshots, and bring-up report.

Out of scope for the first board:

- 220V AC input or onboard mains AC-DC power.
- 48V wide-input product version.
- Zigbee version.
- Extreme miniaturization.
- Mass-production cost optimization.
- Full EMC/certification closure.

Success criteria:

- 12V and 24V input both produce stable 3.3V.
- T3-3S powers on and exposes usable UART/debug path.
- `P24` and `P32` output measurable PWM.
- WW and CW channels drive a common-anode CCT LED load independently.
- Brightness and CCT direction are understandable and correctable by mapping or wiring.
- 12V and 24V tests each run for 30 minutes without abnormal heat, smell, reset, flicker beyond prototype tolerance, or connector/MOSFET/Buck distress.

## 2. System Architecture

```text
Certified 12V/24V constant-voltage supply
  -> Phase 2 low-voltage PCB
      -> input protection/filtering
      -> 3.3V Buck
      -> T3-3S
      -> P24 PWM -> gate resistor -> WW N-MOSFET -> WW-
      -> P32 PWM -> gate resistor -> CW N-MOSFET -> CW-
  -> common-anode CCT LED load
      -> LED+ common positive
      -> WW- warm channel
      -> CW- cool channel
```

Phase 1 facts that must be carried into Phase 2:

- The CCT LED load is common-anode.
- Red wire is common positive; yellow is WW negative; white is CW negative.
- Low-side MOSFET switching is valid for the current load style.
- 12V/24V power must never be connected directly to a Tuya module GPIO or 3.3V rail.
- YYNMOS-4/NMOS-4 was a prototype module only; the PCB needs a product-grade MOSFET power stage.
- 200Hz prototype PWM showed visible flicker risk; the PCB power stage should tolerate a higher PWM target, at least 2kHz for the next firmware target.

## 3. Role Split

Project lead / PM reviewer:

- Freeze requirements and stop scope drift.
- Explain Phase 1 evidence and project boundaries to the intern.
- Approve each gate before the intern moves to the next step.
- Review schematic, BOM, placement, routing, and Gerber outputs using the checklists.
- Run or supervise bring-up tests and record results.

PCB engineer / intern:

- Build the schematic and PCB according to this document and the T3-3S datasheet.
- Maintain the pin map, BOM, issue list, and change log.
- Export review artifacts before each gate.
- Prepare manufacturing files after Gerber release approval.
- Support sample soldering, inspection, and bring-up.

## 4. Four-Week Schedule

| Time | Goal | PM review focus | Intern deliverables |
|---|---|---|---|
| Week 1 Day 1 | Kickoff and requirement freeze | Explain Phase 1 facts, low-voltage boundary, no AC/48V/Zigbee for first board | Requirement note, system block diagram |
| Week 1 Day 2-3 | Schematic v0.1 | Confirm T3-3S pins: `P24=WW PWM`, `P32=CW PWM`; expose `3V3/GND/TX1/RX1/CEN/P24/P32` test points | Schematic PDF, pin map table |
| Week 1 Day 4-5 | Power, MOSFET, BOM review | Confirm 12/24V input, 3.3V Buck, logic-level N-MOSFETs, gate resistors, gate pulldowns, input protection | BOM v0.1, key datasheet links |
| Week 2 Day 1-2 | Placement review | T3-3S antenna points outward; antenna area has no copper/traces; module is away from Buck inductor and LED current loops | Placement screenshot, 3D preview |
| Week 2 Day 3-4 | Routing review | Check VIN/GND/LED current loops, MOSFET copper, reachable test points, readable silkscreen, DRC/ERC | PCB layout v0.9, DRC/ERC report |
| Week 2 Day 5 | Gerber release gate | Approve only if schematic has no known hard error, antenna keep-out is correct, and connectors/silkscreen cannot be easily reversed | Gerber, BOM, pick/place, assembly drawing |
| Week 3 | Board wait and lab prep | Prepare soldering tools, 12/24V safe supplies, meter, logic analyzer, inspection checklist | Soldering plan, power-up checklist |
| Week 4 Day 1-2 | Assembly and visual inspection | No LED load yet; inspect polarity, shorts, module orientation, solder bridges | Assembled sample, inspection photos |
| Week 4 Day 3 | Low-voltage power-up | Start with 12V current-limited input; measure VIN, 3V3, T3-3S current; then check UART/log | Power-up record |
| Week 4 Day 4 | PWM and MOSFET test | Measure `P24/P32` unloaded PWM, then single-channel LED, then dual-channel CCT | Logic analyzer screenshots, LED video |
| Week 4 Day 5 | 24V, thermal, and summary | Test 24V after 12V is stable; run 30 minutes and record temperatures | Phase 2 v0.1 bring-up report |

## 5. Review Gates

Gate 1: requirement freeze

- First board is 12/24V low-voltage only.
- T3-3S is the only Wi-Fi module target.
- LED interface is `LED+ / WW- / CW-`.
- PM and intern agree that connector orientation and silkscreen clarity matter more than board miniaturization.

Gate 2: schematic release to layout

- T3-3S recommended footprint is used.
- `P24` and `P32` are reserved for WW/CW PWM.
- UART/debug and reset-related test points are exposed.
- Buck input/output capacitors and feedback components follow the selected Buck datasheet.
- MOSFET gate series resistors and gate pulldowns are present.
- No LED power current flows through the T3-3S ground path as a thin signal-only trace.

Gate 3: placement release to routing

- T3-3S antenna is at a PCB edge and points away from large metal or high-current zones.
- T3-3S antenna keep-out has no copper, traces, vias, or soldermask openings unless the datasheet explicitly requires them.
- Buck, inductor, diode/switching node, and MOSFET current loops are compact and separated from the antenna.
- Connectors are placed so wiring follows the real lamp harness direction.

Gate 4: Gerber release

- Schematic PDF, PCB top/bottom screenshots, 3D screenshot, BOM, DRC/ERC report, and issue log are all reviewed.
- `VIN+`, `VIN-`, `LED+`, `WW-`, `CW-`, `3V3`, `GND`, `P24`, `P32`, `TX1`, `RX1`, and `CEN` labels are readable.
- The board includes enough test access for first bring-up.
- Known issues are either fixed or explicitly accepted in the issue log.

Gate 5: bring-up complete

- 12V and 24V 3.3V output tests pass.
- PWM is measurable on both channels.
- WW/CW channels switch independently.
- 30-minute thermal test is recorded.
- Changes needed for v0.2 are listed.

## 6. Meeting Rhythm

Daily 15-minute standup:

```text
Yesterday:
Today:
Blocked by:
Decision needed from PM:
Biggest technical risk today:
Artifacts updated:
```

Twice-weekly formal review:

- Mid-week: direction check and issue triage.
- Friday: gate review and approval/rework decision.

Every formal review must use only these artifacts:

- Schematic PDF.
- PCB screenshot or 3D view.
- BOM table.
- Issue/change log.

## 7. Required Discussion Topics With The Intern

Project boundary:

- This is a low-voltage smart middle-layer board.
- The shipped product architecture is `220V AC -> certified constant-voltage supply -> smart low-voltage PCB -> LED`.
- Bench supplies are lab tools only, not final product architecture.

T3-3S footprint and layout:

- Use the datasheet recommended footprint.
- Do not reuse WB3S mechanically or electrically.
- Keep-out areas are not thermal pads and are not for routing.
- The PCB antenna area must not be covered with copper or crossed by traces.

Pin map:

- `P24` -> WW PWM.
- `P32` -> CW PWM.
- `TX1/RX1` -> debug/UART access.
- `CEN` -> reset/test access.
- `3V3/GND/P24/P32/TX1/RX1/CEN` must be available as test points.

Power chain:

- VIN is 12/24V DC only for v0.1.
- 3.3V must come from Buck, not a 24V-to-3.3V LDO.
- 3.3V powers T3-3S and small-signal circuits only.

MOSFET stage:

- Low-side switching for WW and CW negative channels.
- LED common positive remains on `LED+`.
- Each gate needs a series resistor and a pulldown.
- MOSFETs must be logic-level at 3.3V gate drive.
- Copper and thermal area must match expected LED current.

Debuggability:

- First board can be larger.
- Test points and silkscreen are required.
- Connector labels must be understandable by a non-PCB engineer.

## 8. Deliverable Naming

Use these names so review artifacts do not become treasure maps:

- `phase2_t3_3s_schematic_v0.1.pdf`
- `phase2_t3_3s_pin_map_v0.1.xlsx`
- `phase2_t3_3s_bom_v0.1.xlsx`
- `phase2_t3_3s_placement_v0.1.png`
- `phase2_t3_3s_layout_v0.9.png`
- `phase2_t3_3s_drc_erc_v0.9.pdf`
- `phase2_t3_3s_gerber_v0.1.zip`
- `phase2_t3_3s_pick_place_v0.1.csv`
- `phase2_t3_3s_assembly_v0.1.pdf`
- `phase2_t3_3s_bringup_report_v0.1.md`

