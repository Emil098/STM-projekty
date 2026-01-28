# DHT22_CI_Pipeline

This project demonstrates a **CI pipeline for an STM32 embedded application**
with **host-based unit testing**, centered around a fully tested **DHT22 temperature and humidity sensor driver**.

The repository showcases modern embedded development practices:
- testable C design
- hardware abstraction
- automated unit testing with Unity + CMock
- CI/CD using GitHub Actions
- Docker-based, reproducible build environment

---

## Project Highlights

- **DHT22 sensor support**
  - Temperature and humidity acquisition
  - Timing-critical single-wire protocol
  - Clean separation between protocol logic and hardware access
  - Fully unit-tested using mocked GPIO and delay functions

- **Host-based unit testing (Unity + CMock)**
  - Tests executed on a PC (Linux runner)
  - No STM32 hardware required
  - Deterministic and fast feedback in CI

- **Interrupt-driven debug UART**
  - RX/TX ring buffers
  - Non-blocking transmit
  - Tested using a fake UART backend

- **Lightweight console module**
  - NanoPrintf-based formatted output
  - Minimal footprint compared to standard printf
  - UART dependency fully mockable

- **CI pipeline with quality gates**
  - Unit tests must pass before firmware build
  - Code coverage generated with gcov/gcovr
  - Versioned firmware artifacts published as GitHub Releases