# System Architecture

## 1. Component Diagram (Layered Architecture)
The system uses a Clean Architecture model where dependencies point downwards:
Application (FSM, Alert, Config) -> Service (Data, Telemetry) -> Driver (Sensor Mgr) -> HAL (Sim, Timer).

## 2. System State Diagram (FSM)
- States: `SYS_INIT`, `SYS_IDLE`, `SYS_MONITORING`, `SYS_ALERT`, `SYS_ERROR`.
- Implemented using a static const function pointer table to avoid large switch-case blocks.

## 3. Module Descriptions & Integration Map
| Module | Description | Lecture Covered |
|---|---|---|
| **HAL** | Simulated volatile registers and tick timer. | L14 (POSIX/volatile) |
| **Logger** | Variadic macros for stdout and POSIX file logging. | L7 (Macros, Variadic) |
| **Drivers** | Opaque pointer OOP design for Temp/Humi sensors. | L4 (OOP in C) |
| **Data** | Ring buffer, moving average, and intrusive linked list. | L12 (Data Structures) |
| **Telemetry**| TLV serialization with Big-Endian and CRC-8. | L11 (Numerical/Endian) |
| **App/FSM** | Function pointer dispatch for FSM and CLI commands. | L5/L6 (Pointers, Patterns)|