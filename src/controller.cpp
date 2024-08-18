#include "Arduino.h"

#define WIFI_SSID "ASUS"
#define WIFI_PASSWORD "13601748441"
#define MQTT_SERVER "192.168.50.199"
#define JOYSTICK_POSITIVE_X_PIN D2
#define JOYSTICK_POSITIVE_Y_PIN D3
#define JOYSTICK_NEGATIVE_X_PIN D4
#define JOYSTICK_NEGATIVE_Y_PIN D5
#define Z_BUTTTON_PIN D6
#define EM_BUTTON_PIN D7

#include "shared.h"
#include "utils/wifiConnection.h"
#include <PubSubClient.h>
#include "utils/mqtt.h"

WiFiClient espClient;
PubSubClient client(espClient);

command currentDir = Still;
bool z_button_pressed = false;
bool em_button_pressed = false;

void setup() {
    Serial.begin(115200);
    Serial0.begin(115200);

    initWiFi(WIFI_SSID, WIFI_PASSWORD);
    client.setServer(MQTT_SERVER, 1883);
    client.setCallback(callback);

    pinMode(JOYSTICK_NEGATIVE_X_PIN, INPUT_PULLUP);
    pinMode(JOYSTICK_NEGATIVE_Y_PIN, INPUT_PULLUP);
    pinMode(JOYSTICK_POSITIVE_X_PIN, INPUT_PULLUP);
    pinMode(JOYSTICK_POSITIVE_Y_PIN, INPUT_PULLUP);
    pinMode(EM_BUTTON_PIN, INPUT_PULLUP);
    pinMode(Z_BUTTTON_PIN, INPUT_PULLUP);
}

void loop() {
    if (!client.connected()) {
        mqtt::reconnect(&client);
    }
    client.loop();
//    client.publish("out", buffer);

    int joystick_positive_x = digitalRead(JOYSTICK_POSITIVE_X_PIN);
    int joystick_positive_y = digitalRead(JOYSTICK_POSITIVE_Y_PIN);
    int joystick_negative_x = digitalRead(JOYSTICK_NEGATIVE_X_PIN);
    int joystick_negative_y = digitalRead(JOYSTICK_NEGATIVE_Y_PIN);
    int z_button = digitalRead(Z_BUTTTON_PIN);
//    int em_button = digitalRead(EM_BUTTON_PIN);

    if (joystick_positive_x == LOW && currentDir != XPositive) {
        Serial0.println(XPositive);
        Serial.println(XPositive);
        currentDir = XPositive;
    }
    else if (joystick_positive_y == LOW && currentDir != YPositive) {
        Serial0.println(YPositive);
        Serial.println(YPositive);
        currentDir = YPositive;
    }
    else if (joystick_negative_x == LOW && currentDir != XNegative) {
        Serial0.println(XNegative);
        Serial.println(XNegative);
        currentDir = XNegative;
    }
    else if (joystick_negative_y == LOW && currentDir != YNegative) {
        Serial0.println(YNegative);
        Serial.println(YNegative);
        currentDir = YNegative;
    }
    else if ((joystick_positive_x + joystick_negative_x + joystick_positive_y + joystick_negative_y) == 4 && currentDir != Still) {
        Serial0.println(Still);
        Serial.println(Still);
        currentDir = Still;
    }

    if (z_button == LOW && !z_button_pressed) {
        Serial0.println(Z_ON);
        Serial.println(Z_ON);
        z_button_pressed = true;
    } else if (z_button == HIGH && z_button_pressed) {
        Serial0.println(Z_STILL);
        Serial.println(Z_STILL);
        z_button_pressed = false;
    }

//    if (em_button == LOW && !em_button_pressed) {
//        Serial0.println(EM_ON);
//        Serial.println(EM_ON);
//        z_button_pressed = true;
//    } else if (em_button == HIGH && em_button_pressed) {
//        Serial0.println(EM_OFF);
//        Serial.println(EM_ON);
//        z_button_pressed = false;
//    }

    delay(50);
}