# Bluetooth2PWM

This project is for ESP32 to get a coordinate from Oculus Quest via Bluetooth and then control L298N to move accordingly via analog signals.

## Setup

### Clion + PlatformIO

If you use Clion with PlatformIO, just clone this project and run configuration `PlatformIO Upload` in Clion, and you are good to go.

### Arduino IDE

If you code in Arduino IDE, copy content from `src/main.cpp` into Arduino IDE and upload the program as you usually do.

## Reference

- More on how to control a L298N: https://create.arduino.cc/projecthub/ryanchan/how-to-use-the-l298n-motor-driver-b124c5
- More on PlatformIO with ESP32: https://docs.platformio.org/en/latest/platforms/espressif32.html
- More on `AnalogWrite` with ESP32: https://github.com/Dlloydev/ESP32-ESP32S2-AnalogWrite