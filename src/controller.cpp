#include "Arduino.h"

#define JOYSTICK_POSITIVE_X_PIN D2
#define JOYSTICK_POSITIVE_Y_PIN D3
#define JOYSTICK_NEGATIVE_X_PIN D4
#define JOYSTICK_NEGATIVE_Y_PIN D5
#define Z_BUTTTON_PIN D6
#define EM_BUTTON_PIN D7

#include "shared.h"

command currentDir = Still;
bool z_button_pressed = false;
bool em_button_pressed = false;

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
    int z_button = digitalRead(Z_BUTTTON_PIN);
    int em_button = digitalRead(EM_BUTTON_PIN);

    if (joystick_positive_x == LOW && currentDir != XPositive) {
        Serial0.println(XPositive);
        currentDir = XPositive;
    }
    else if (joystick_positive_y == LOW && currentDir != YPositive) {
        Serial0.println(YPositive);
        currentDir = YPositive;
    }
    else if (joystick_negative_x == LOW && currentDir != XNegative) {
        Serial0.println(XNegative);
        currentDir = XNegative;
    }
    else if (joystick_negative_y == LOW && currentDir != YNegative) {
        Serial0.println(YNegative);
        currentDir = YNegative;
    }
    else if ((joystick_positive_x + joystick_negative_x + joystick_positive_y + joystick_negative_y) == 4 && currentDir != Still) {
        Serial0.println(Still);
        currentDir = Still;
    }

    if (z_button == LOW && !z_button_pressed) {
        Serial0.println(Z_ON);
        z_button_pressed = true;
    } else if (z_button == HIGH && z_button_pressed) {
        Serial0.println(Z_STILL);
        z_button_pressed = false;
    }

    if (em_button == LOW && !em_button_pressed) {
        Serial0.println(EM_ON);
        z_button_pressed = true;
    } else if (em_button == HIGH && em_button_pressed) {
        Serial0.println(EM_OFF);
        z_button_pressed = false;
    }

    delay(50);
}