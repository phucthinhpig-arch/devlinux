# Changelog

All notable changes to this project will be documented in this file.

## [1.0.0] - 2026-10-18
### Added
- IoT Monitoring System with 4-layer Clean Architecture.
- FSM with function pointer table dispatch (5 states, 10 events).
- CLI command dispatcher with 8 commands.
- Temperature & humidity sensor drivers using opaque pointer OOP pattern.
- Moving average filter with O(1) ring buffer.
- TLV telemetry serialization with CRC-8 checksum.
- Industrial logger with POSIX file I/O output.
- Config persistence with safe parsing (`strtol`).
- Event log with intrusive linked list and static memory pool.
- GitHub Actions CI pipeline with 4 quality gates.
- Full Doxygen documentation and UML diagrams.