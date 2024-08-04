#ifndef CLAW_WIFICONNECTION_H
#define CLAW_WIFICONNECTION_H

#endif //CLAW_WIFICONNECTION_H

#include "Arduino.h"
#include "WiFi.h"

void initWiFi(const char* ssid, const char* password) {
    WiFiClass::mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi ..");
    while (WiFiClass::status() != WL_CONNECTED) {
        Serial.print('.');
        delay(1000);
    }
    Serial.println("WiFi Connected!");
};
