# Sprint Plan

**Sprint Goal:** Deliver a working IoT Environmental Monitoring System (v1.0.0) with full CI/CD and MISRA-C compliance within 1 sprint.

## Product Backlog & WBS
| Ticket | Task | Story Points | Status |
|---|---|---|---|
| IOT-001 | Project Scaffolding & HAL | 3 | Done |
| IOT-002 | Industrial Logger Module | 3 | Done |
| IOT-003 | Sensor Drivers (OOP) | 5 | Done |
| IOT-004 | Data Proc (Ring Buffer, Event Log) | 5 | Done |
| IOT-005 | Telemetry Serialization (TLV) | 5 | Done |
| IOT-006 | Application FSM & CLI | 8 | Done |
| IOT-007 | System Integration & Config | 8 | Done |
| IOT-008 | CI/CD, Docs & Release | 5 | Done |

## Definition of Done (DoD)
- Code compiles with zero warnings (`-Wall -Wextra -pedantic -Werror -std=c11`).
- All Unity unit tests pass (ASan + UBSan).
- Static analysis clean (cppcheck + clang-tidy).
- Pre-commit hooks pass.
- BARR-C naming and Doxygen comments applied.
- Code reviewed and merged into `develop`.