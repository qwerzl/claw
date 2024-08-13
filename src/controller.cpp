#include "Arduino.h"

#define JOYSTICK_POSITIVE_X_PIN D2
#define JOYSTICK_POSITIVE_Y_PIN D3
#define JOYSTICK_NEGATIVE_X_PIN D4
#define JOYSTICK_NEGATIVE_Y_PIN D5

enum dir {
    XPositive,
    XNegative,
    YPositive,
    YNegative,
    Still
};

dir currentDir = Still;

void setup() {
    Serial.begin(115200);
    Serial0.begin(115200);

    pinMode(JOYSTICK_NEGATIVE_X_PIN, INPUT_PULLUP);
    pinMode(JOYSTICK_NEGATIVE_Y_PIN, INPUT_PULLUP);
    pinMode(JOYSTICK_POSITIVE_X_PIN, INPUT_PULLUP);
    pinMode(JOYSTICK_POSITIVE_Y_PIN, INPUT_PULLUP);
}

void loop() {
    int joystick_positive_x = digitalRead(JOYSTICK_POSITIVE_X_PIN);
    int joystick_positive_y = digitalRead(JOYSTICK_POSITIVE_Y_PIN);
    int joystick_negative_x = digitalRead(JOYSTICK_NEGATIVE_X_PIN);
    int joystick_negative_y = digitalRead(JOYSTICK_NEGATIVE_Y_PIN);

    if (joystick_positive_x == LOW && currentDir != XPositive) {
        Serial.println("1");
        Serial0.println(1);
        currentDir = XPositive;
    }
    else if (joystick_positive_y == LOW && currentDir != YPositive) {
        Serial.println("2");
        Serial0.println(2);
        currentDir = YPositive;
    }
    else if (joystick_negative_x == LOW && currentDir != XNegative) {
        Serial.println("3");
        Serial0.println(3);
        currentDir = XNegative;
    }
    else if (joystick_negative_y == LOW && currentDir != YNegative) {
        Serial.println("4");
        Serial0.println(4);
        currentDir = YNegative;
    }
    else if ((joystick_positive_x + joystick_negative_x + joystick_positive_y + joystick_negative_y) == 4 && currentDir != Still) {
        Serial.println("0");
        Serial0.println(5);
        currentDir = Still;
    }

    delay(50);
}