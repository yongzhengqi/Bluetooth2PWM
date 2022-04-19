# Bluetooth2PWM

This project is for ESP32 to get a coordinate from Oculus Quest via Bluetooth and then control L298N to move accordingly via (PWM simulated) analog signals.

## Setup

### CLion + PlatformIO

If you use CLion with PlatformIO, just clone this project, and you are ready to code. To upload the program to ESP32, run configuration `PlatformIO Upload` in CLion.

### Arduino IDE

If you code in Arduino IDE, install [Servo Library for ESP32](https://github.com/madhephaestus/ESP32Servo) first. Then copy content from `src/main.cpp` into Arduino IDE and upload the program as you usually do.

## Reference

- More on how to control a L298N: https://create.arduino.cc/projecthub/ryanchan/how-to-use-the-l298n-motor-driver-b124c5
- More on PlatformIO with ESP32: https://docs.platformio.org/en/latest/platforms/espressif32.html
- More on `AnalogWrite` and `Servo` with ESP32: https://github.com/madhephaestus/ESP32Servo 