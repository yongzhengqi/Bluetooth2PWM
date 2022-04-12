#include <Arduino.h>

// Because ESP32 developers never wrote an analogWrite function for the ESP32. See more at
// https://forum.arduino.cc/t/problem-to-compile-with-esp32-board-after-board-support-installation-success/627650
#define analogWrite ledcWrite

// See more at https://create.arduino.cc/projecthub/ryanchan/how-to-use-the-l298n-motor-driver-b124c5

class Motor {
private:
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
        MAX_SPEED = 255;
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
        Serial.println(abs_speed);
        analogWrite(_pinSpeed, abs_speed);
    }
};

int HALF_SPEED = 100;
String LEFT, RIGHT, UP, DOWN, STOP;
Motor leftMotor, rightMotor;

void setup() {
    leftMotor = Motor(2, 3, 9);
    rightMotor = Motor(4, 5, 10);

    Serial.begin(9600);

    LEFT = "left";
    RIGHT = "right";
    UP = "up";
    DOWN = "down";
    STOP = "stop";
}

void loop() {
    if (Serial.available() <= 0) {
        return;
    }

    Serial.println("Start parsing the command.");

    leftMotor.stop();
    rightMotor.stop();

    String command = Serial.readString();
    command.toLowerCase();

    if (command.indexOf(STOP) >= 0) {
        leftMotor.stop();
        rightMotor.stop();
        Serial.println("Command: stop\nIgnoring other commands.");
        return;
    }

    if (command.indexOf(LEFT) >= 0) {
        leftMotor.run(-HALF_SPEED);
        rightMotor.run(HALF_SPEED);
        Serial.println("Command: left");
    } else if (command.indexOf(RIGHT) >= 0) {
        leftMotor.run(HALF_SPEED);
        rightMotor.run(-HALF_SPEED);
        Serial.println("Command: right");
    }
    if (command.indexOf(UP) >= 0) {
        leftMotor.run(HALF_SPEED);
        rightMotor.run(HALF_SPEED);
        Serial.println("Command: up");
    } else if (command.indexOf(DOWN) >= 0) {
        leftMotor.run(-HALF_SPEED);
        rightMotor.run(-HALF_SPEED);
        Serial.println("Command: down");
    }

    Serial.println("Command parsed.");

    delay(3000);

    leftMotor.stop();
    rightMotor.stop();
}