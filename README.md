# Microcontroller-PC Serial Interface

This project demonstrates a two-part embedded communication system in which a PC application exchanges arithmetic commands with a microcontroller over a UART/RS232 serial link. The microcontroller parses each received expression, performs the requested two-operand operation, returns the result, and supports logging of the message exchange.

## Project focus

- UART/RS232 communication between a PC application and a microcontroller.
- Separate PC-side and microcontroller-side applications.
- Command framing, input validation, and expression parsing.
- Embedded two-operand arithmetic processing.
- Result feedback from the microcontroller to the PC application.
- Text logging of transmitted commands and returned results.

## System flow

1. The user enters an expression in the PC application.
2. The PC application transmits the command through the serial port.
3. The microcontroller validates and parses the expression.
4. The microcontroller performs the requested operation.
5. The result is sent back to the PC application for display and logging.

## Hardware and software

- Arduino or ST Nucleo-compatible microcontroller board.
- UART/RS232 serial connection.
- Microcontroller firmware written in C.
- PC application written in C/C++.
- A compiler and serial-terminal or PC-side application environment appropriate to the selected board.

## Repository contents

- Development-phase documentation and design decisions.
- C++ PC-side project archive.
- Arduino/microcontroller source archive.
- Compilation and testing instructions.
- Final project documentation and supporting reports.

## Documentation

Start with `Compiling and testing Instructions.pdf` for the build and test procedure, then review the development-phase documents before opening the source archives.

The project was completed for the IU course Programming with C/C++ (DLBROEPRS01_E), Task 1.2. Original submitted artifacts are retained for traceability.
