# ESP32-S3 MP3 Player

An embedded MP3 player built around the **ESP32-S3** using **ESP-IDF**.

The goal of this project is to create a standalone MP3 player with a responsive user interface that allows users to browse music, control playback, and interact with the device using physical buttons and a rotary encoder.

## ESP-IDF

The project currently uses:
- ESP-IDF v6.1
- ESP-IDF VS Code Extension v2.2

## Hardware

The project currently uses:

- **ESP32-S3**
- Adafruit ANO Rotary Navigation Encoder
- Adafruit 160x80 0.96" TFT Display
- USB-C Breakout Board
- MicroSD Breakout Board

## Building

This project requires the ESP-IDF development environment.

Build the project:

```bash
idf.py build
```

Flash the ESP32-S3:

```bash
idf.py flash
```

Monitor serial output:

```bash
idf.py monitor
```

Build, flash, and monitor in one command:

```bash
idf.py build flash monitor
```

## Current Status

The project is currently under development.

Currently implemented:

- Button input using GPIO interrupts
- Rotary encoder input
- FreeRTOS-based input handling
- Input manager for receiving input events


## Future Work

Planned additions include:

- Display driver
- LVGL graphical interface
- SD card driver
- MP3 decoding and processing


## License

This project is currently intended for educational and personal development purposes.
