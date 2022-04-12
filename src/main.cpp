#include <cmath>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

#include <Arduino.h>
#include <BluetoothSerial.h>

// ESP32 developers never wrote an analogWrite function for the ESP32. So `ESP32-AnalogWrite` in `lib/` is imported.
// See more at https://forum.arduino.cc/t/problem-to-compile-with-esp32-board-after-board-support-installation-success/627650.
#include <analogWrite.h>

const float MinMoveDelta = 0.05;
const float MaxSpeedDelta = 0.2;

// Board specific constants.
const int LedPin = 2;
const int MaxSpeed = 255;

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

    int _speed;
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

        _speed = 0;
        MAX_SPEED = MaxSpeed;
    }

    void stop() {
        _speed = 0;
        analogWrite(_pinSpeed, 0);
    }

    void run(int speed) {
        _speed += speed;
        _speed = max(min(_speed, MAX_SPEED), -MAX_SPEED);

        if (_speed > 0) {
            digitalWrite(_pin1, HIGH);
            digitalWrite(_pin2, LOW);
        } else {
            digitalWrite(_pin2, HIGH);
            digitalWrite(_pin1, LOW);
        }

        int abs_speed = abs(_speed);
        analogWrite(_pinSpeed, abs_speed);
    }
};

Motor xMotor, yMotor;

BluetoothSerial BTSerial;

void setup() {
    pinMode (LedPin, OUTPUT);

    xMotor = Motor(2, 3, 9);
    yMotor = Motor(4, 5, 10);

    // Required by Lou, the name of Bluetooth has to be set as `Lou_ESP32`.
    BTSerial.begin("Lou_ESP32");

    Serial.begin(9600);
}

int distance2speed(float d) {
    int direction = d > 0 ? -1 : 1;
    d = abs(d);
    if (d < MinMoveDelta) {
        return 0;
    } else if (d > MaxSpeedDelta) {
        return MaxSpeed * direction;
    } else {
        int abs_speed = int(float(MaxSpeed) * (d - MinMoveDelta) / (MaxSpeedDelta - MinMoveDelta));
        return abs_speed * direction;
    }
}

void loop() {
    if (!BTSerial.available()) {
        digitalWrite (LedPin, LOW);
        Serial.println("No Bluetooth input.");
        return;
    }

    digitalWrite (LedPin, HIGH);

    String arduino_data = BTSerial.readStringUntil(')');
    std::string cpp_data(arduino_data.c_str());
    std::stringstream coordinates_str_stream(cpp_data);

    float x, y;
    char comma, open_paren;
    if (!(coordinates_str_stream >> open_paren >> x >> comma >> y)) {
        Serial.printf("Two floats are expected, but got %s instead.\n", arduino_data.c_str());
        return;
    }

    Serial.printf("(%f, %f)\n", x, y);

    int speed_x = distance2speed(x);
    Serial.printf("X axis motor running at %d\n", speed_x);
    xMotor.stop();
    xMotor.run(speed_x);

    int speed_y = distance2speed(y);
    Serial.printf("Y axis motor running at %d\n", speed_y);
    yMotor.stop();
    yMotor.run(speed_y);
}