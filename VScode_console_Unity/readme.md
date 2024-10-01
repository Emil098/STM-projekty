# VScode_console_Unity

## Project Overview
**VScode_console_Unity** is a custom project developed on top of a CubeMX-generated framework for the STM32L053 Nucleo board. The project integrates three custom libraries: `debugUart`, `debugPrintf`, and `console`. These libraries enable low-level UART handling, custom `printf` functions using `nanoprintf`, and a user command console via UART2.

The project is thoroughly tested with throwtheswitch Unity Test Framework, using cmock for external dependencies, and includes detailed code coverage reports.

## Key Features
- **debugUart**: Handles UART interrupts for data transmission and reception, managing RX and TX buffers to ensure efficient communication.
  
- **debugPrintf**: Custom `printf` implementations that route formatted strings to UART, using the lightweight `nanoprintf` library.

- **console**: A user-interactive console that processes commands entered over the UART interface, allowing the user to interact with the firmware.

## Technologies Used
- **Platform**: STM32 (Nucleo STM32L053)
- **Framework**: STM32 HAL, CubeMX
- **Testing**: Unity Test Framework, using cmock for creating mocks of the included libraries
- **Libraries**: `nanoprintf`, `fakeUart`, `debugUart`, `debugPrintf`, `console`
- **Programming Language**: C
- **Tools**: Makefile, VSCode