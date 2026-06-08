# Phase 2 T3-3S PCB Status Log

> Updated: 2026-06-08  
> Purpose: one place to record daily progress, decisions, risks, changes, and bring-up evidence.

## 1. Current Baseline

| Item | Baseline |
|---|---|
| Board target | T3-3S + 12/24V dual-channel CCT low-side MOSFET PCB |
| PM/reviewer | Project lead |
| PCB implementer | Department intern / PCB engineer |
| Input | 12V/24V DC only |
| Output | `LED+`, `WW-`, `CW-` |
| PWM mapping | `P24 -> WW`, `P32 -> CW` |
| Debug/test points | `3V3`, `GND`, `TX1`, `RX1`, `CEN`, `P24`, `P32` |
| Out of scope | 220V AC, 48V, Zigbee, extreme miniaturization, mass-production optimization |

## 2. Daily Standup Template

```text
Date:
Yesterday:
Today:
Blocked by:
Decision needed from PM:
Biggest technical risk today:
Artifacts updated:
```

## 3. Formal Review Log

| Date | Review gate | Decision | Required rework | Owner | Due |
|---|---|---|---|---|---|
| 2026-06-08 | Plan implementation | Phase 2 schedule/checklist/status docs created | Start Week 1 Day 1 kickoff with intern | PM | Week 1 Day 1 |

## 4. Change Log

| Date | Change | Reason | Impacted tests | Approved by |
|---|---|---|---|---|
| 2026-06-08 | T3-3S locked as Phase 2 Wi-Fi module | Phase 2 module decision completed | Schematic, footprint, pin map, layout antenna keep-out | PM |
| 2026-06-08 | First board scope locked to 12/24V dual CCT | Keep v0.1 bring-up tractable | Power, MOSFET, LED load, thermal tests | PM |

## 5. Issue Log

| ID | Date | Issue | Severity | Owner | Status | Resolution |
|---|---|---|---|---|---|---|
| P2-001 | 2026-06-08 | T3-3S App/firmware path may lag behind PCB bring-up | Medium | PM/software | Open | Use local PWM test firmware for hardware validation if App DP mapping is not ready |
| P2-002 | 2026-06-08 | 24V power source must be safe and current-limited before bring-up | High | PM/lab | Open | Prepare safe 24V supply before Week 4 |
| P2-003 | 2026-06-08 | T3-3S antenna layout can be degraded by copper, metal, or switching power placement | High | PCB engineer | Open | Enforce datasheet footprint and antenna keep-out review before routing |

## 6. Week 1 Log

### Day 1 Kickoff

```text
Date:
Attendees:
Phase 1 facts explained:
Scope confirmed:
Out-of-scope confirmed:
Open questions:
Artifacts received from intern:
PM decision:
```

### Day 2-3 Schematic Draft

```text
Date:
Schematic version:
Pin map version:
T3-3S symbol checked:
PWM mapping checked:
Test points checked:
Open schematic issues:
PM decision:
```

### Day 4-5 Power/MOSFET/BOM Review

```text
Date:
BOM version:
Buck selected:
MOSFET selected:
Protection/filtering selected:
Datasheets attached:
Open BOM issues:
PM decision:
```

## 7. Week 2 Log

### Day 1-2 Placement Review

```text
Date:
PCB version:
T3-3S antenna placement:
Antenna keep-out:
Buck/MOSFET placement:
Connector placement:
Open placement issues:
PM decision:
```

### Day 3-4 Routing Review

```text
Date:
PCB version:
VIN/GND routing:
LED current routing:
Buck switching loop:
MOSFET thermal copper:
Test point access:
DRC/ERC status:
Open routing issues:
PM decision:
```

### Day 5 Gerber Release

```text
Date:
Schematic PDF:
PCB screenshots:
BOM:
Pick/place:
Gerber:
Assembly drawing:
Issue log status:
Release decision:
```

## 8. Week 3 Log

```text
Date:
Board fabrication status:
Components ordered:
Tools ready:
12V supply ready:
24V supply ready:
Logic analyzer ready:
Soldering plan:
Open lab prep issues:
```

## 9. Week 4 Bring-Up Records

### Assembly and Visual Inspection

| Check | Result | Notes |
|---|---|---|
| T3-3S orientation |  |  |
| Buck orientation |  |  |
| MOSFET orientation |  |  |
| Connector polarity |  |  |
| Solder bridges |  |  |
| `VIN+` to `VIN-` resistance |  |  |
| `3V3` to `GND` resistance |  |  |

### 12V Power Test

| Measurement | Expected | Actual | Pass/Fail |
|---|---:|---:|---|
| VIN | about 12V |  |  |
| 3V3 | 3.3V +/- tolerance |  |  |
| Board input current | no runaway |  |  |
| T3-3S temperature | no abnormal heat |  |  |
| Buck temperature | no abnormal heat |  |  |

### PWM Test

| Signal | Expected | Actual | Evidence |
|---|---|---|---|
| P24 | PWM for WW |  |  |
| P32 | PWM for CW |  |  |
| WW gate | follows P24 |  |  |
| CW gate | follows P32 |  |  |

### LED Load Test

| Test | Expected | Actual | Pass/Fail |
|---|---|---|---|
| WW single-channel | Warm channel changes brightness |  |  |
| CW single-channel | Cool channel changes brightness |  |  |
| Dual-channel CCT | WW/CW mix changes color temperature |  |  |
| CCT direction | App or local mapping is known |  |  |

### 24V Power and Thermal Test

| Measurement | Start | 10 min | 20 min | 30 min | Notes |
|---|---:|---:|---:|---:|---|
| VIN |  |  |  |  |  |
| 3V3 |  |  |  |  |  |
| T3-3S temp |  |  |  |  |  |
| Buck temp |  |  |  |  |  |
| WW MOSFET temp |  |  |  |  |  |
| CW MOSFET temp |  |  |  |  |  |
| Input connector temp |  |  |  |  |  |
| LED connector temp |  |  |  |  |  |

## 10. v0.2 Follow-Up List

| Priority | Finding | Proposed v0.2 change | Evidence |
|---|---|---|---|
|  |  |  |  |

