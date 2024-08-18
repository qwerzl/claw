#include "Arduino.h"

#define WIFI_SSID "ASUS"
#define WIFI_PASSWORD "1360**48441"
#define MQTT_SERVER "192.168.50.199"
#define JOYSTICK_POSITIVE_X_PIN D2
#define JOYSTICK_POSITIVE_Y_PIN D3
#define JOYSTICK_NEGATIVE_X_PIN D4
#define JOYSTICK_NEGATIVE_Y_PIN D5
#define BUTTTON_PIN D6

#include "shared.h"
#include "utils/wifiConnection.h"
#include <PubSubClient.h>
#include "utils/mqtt.h"

WiFiClient espClient;
PubSubClient client(espClient);

command currentDir = Still;
bool buttonPressed = false;

void setup() {
    Serial.begin(115200);
    Serial0.begin(115200);
    Serial0.setTimeout(10);

    initWiFi(WIFI_SSID, WIFI_PASSWORD);
    client.setServer(MQTT_SERVER, 1883);
    client.setCallback(callback);

    pinMode(JOYSTICK_NEGATIVE_X_PIN, INPUT_PULLUP);
    pinMode(JOYSTICK_NEGATIVE_Y_PIN, INPUT_PULLUP);
    pinMode(JOYSTICK_POSITIVE_X_PIN, INPUT_PULLUP);
    pinMode(JOYSTICK_POSITIVE_Y_PIN, INPUT_PULLUP);
    pinMode(BUTTTON_PIN, INPUT_PULLUP);
}

void loop() {
    if (!client.connected()) {
        mqtt::reconnect(&client);
    }
    client.loop();
    String receivedCommand = Serial0.readStringUntil('\n');

    if (!receivedCommand.isEmpty()) {
        Serial.println(receivedCommand);
        client.publish("out", receivedCommand.c_str());
    }
//    client.publish("out", buffer);

    int joystick_positive_x = digitalRead(JOYSTICK_POSITIVE_X_PIN);
    int joystick_positive_y = digitalRead(JOYSTICK_POSITIVE_Y_PIN);
    int joystick_negative_x = digitalRead(JOYSTICK_NEGATIVE_X_PIN);
    int joystick_negative_y = digitalRead(JOYSTICK_NEGATIVE_Y_PIN);
    int button = digitalRead(BUTTTON_PIN);
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

    if (button == LOW && !buttonPressed) {
        buttonPressed = true;
    } else if (button == HIGH && buttonPressed) {
        Serial0.println(BUTTON_TOGGLE);
        Serial.println(BUTTON_TOGGLE);
        buttonPressed = false;
    }

    delay(50);
}