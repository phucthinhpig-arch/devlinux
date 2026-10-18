# IoT Environmental Monitoring System

<!-- FINAL_PROJECT_REPO: https://github.com/thinhadmin/iot_monitoring_system -->
**Repository:** https://github.com/thinhadmin/iot_monitoring_system

## Overview
This is an industry-grade IoT Environmental Monitoring System desktop simulation written in C. It reads simulated temperature and humidity data, applies moving average filtering, checks alert thresholds, and serializes telemetry packets using TLV format. The system is driven by a Finite State Machine (FSM) and controlled via CLI.

## Architecture
The project follows a 4-layer Clean Architecture (Application, Service, Driver, HAL). It enforces MISRA-C and CERT-C coding standards, uses Unity for TDD, and integrates GitHub Actions for CI/CD.

## Build Instructions
`cmake -B build -DCMAKE_BUILD_TYPE=Debug`
`cmake --build build`

## Running Tests & Quality Gates
**Run Unity Tests with Sanitizers:**
`cmake -B build-asan -DSANITIZER=asan+ubsan && cmake --build build-asan && ctest --test-dir build-asan -V`
**Run Static Analysis:**
`cmake --build build --target cppcheck`
`pre-commit run --all-files`

## Usage
Run the application:
`./build/iot_monitor`
Available CLI commands: `start`, `stop`, `status`, `config`, `alert_ack`, `reset`, `help`, `quit`.

## CI Pipeline
Integrated with GitHub Actions checking 4 stages: Build, Pre-commit, Static Analysis (cppcheck), and Unit Tests (Unity + ASan/UBSan).