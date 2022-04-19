#include <cmath>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

#include <Arduino.h>
#include <BluetoothSerial.h>

// ESP32 developers never wrote a Servo library for the ESP32. So `ESP32-Servo` in `lib/` is imported.
// See more at https://github.com/madhephaestus/ESP32Servo
#include <ESP32Servo.h>

// ESP32 developers never wrote an analogWrite function for the ESP32. So `ESP32-AnalogWrite` in `lib/` is imported.
// See more at https://forum.arduino.cc/t/problem-to-compile-with-esp32-board-after-board-support-installation-success/627650.
#include <analogWrite.h>

// Board specific constants.
const int LedPin = 2;
const int DcMotorMaxSpeed = 255;

// Control a DC motor with a L298N control board.
// Example:
//    motor = Motor(2, 3, 9);  // Number 2, 3, and 9 is the pin # you connect on ESP32.
//    motor.run(speed);  // Speed should be a int in (0, 255].
class Motor {
private:
    // A L298N requires 3 pins to control one motor. First two pins are used to control direction. And the third pin is
    // used to control speed with an analog signal. See more at https://create.arduino.cc/projecthub/ryanchan/how-to-use-the-l298n-motor-driver-b124c5
    int _pin1;
    int _pin2;
    int _pinSpeed;

    int MAX_SPEED;
public:
    Motor() = default;

    Motor(int pin1, int pin2, int pinSpeed) {
        _pin1 = pin1;
        _pin2 = pin2;
        _pinSpeed = pinSpeed;

        pinMode(_pin1, OUTPUT);
        pinMode(_pin2, OUTPUT);
        pinMode(_pinSpeed, OUTPUT);

        MAX_SPEED = DcMotorMaxSpeed;
    }

    void stop() {
        analogWrite(_pinSpeed, 0);
    }

    void run(int speed) {
        speed = max(min(speed, MAX_SPEED), -MAX_SPEED);

        if (speed > 0) {
            digitalWrite(_pin1, HIGH);
            digitalWrite(_pin2, LOW);
        } else {
            digitalWrite(_pin2, HIGH);
            digitalWrite(_pin1, LOW);
        }

        int abs_speed = abs(speed);
        analogWrite(_pinSpeed, abs_speed);
    }
};

Motor leftMotor, rightMotor;

Servo xServo, yServo;

BluetoothSerial BTSerial;

void setup() {
    pinMode (LedPin, OUTPUT);

    leftMotor = Motor(12, 13, 25);
    rightMotor = Motor(14, 27, 26);

    xServo.attach(9);
    yServo.attach(10);

    // Required by Lou, the name of Bluetooth has to be set as `Lou_ESP32`.
    BTSerial.begin("Lou_ESP32");

    Serial.begin(9600);
}

int ratio2speed(float ratio) {
    return int(ratio * 90) + 90;
}

void loop() {
    if (!BTSerial.available()) {
        digitalWrite (LedPin, LOW);

        leftMotor.stop();
        rightMotor.stop();

        Serial.println("No Bluetooth input.");
        return;
    }

    digitalWrite (LedPin, HIGH);

    String arduino_data = BTSerial.readStringUntil(')');
    std::string cpp_data(arduino_data.c_str());
    std::stringstream coordinates_str_stream(cpp_data);

    float x_speed_ratio, y_speed_ratio;
    char comma, open_paren;
    if (!(coordinates_str_stream >> open_paren >> x_speed_ratio >> comma >> y_speed_ratio)) {
        Serial.printf("Two floats are expected, but got %s instead.\n", arduino_data.c_str());
        return;
    }

    Serial.printf("Data from Bluetooth: (%f, %f)\n", x_speed_ratio, y_speed_ratio);

    int x_speed = ratio2speed(x_speed_ratio);
    xServo.write(x_speed);
    Serial.printf("X axis servo running at %d (0~180)\n", x_speed);

    int y_speed = ratio2speed(y_speed_ratio);
    yServo.write(y_speed);
    Serial.printf("Y axis servo running at %d (0~180)\n", y_speed);
}