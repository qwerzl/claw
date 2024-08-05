//
// Created by Tom Tang on 2024/8/5.
//

#ifndef CLAW_MQTT_H
#define CLAW_MQTT_H

#endif //CLAW_MQTT_H

#include "Arduino.h"
#include <PubSubClient.h>

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

namespace mqtt {
    void reconnect(PubSubClient *client) {
        // Loop until we're reconnected
        while (!client->connected()) {
            Serial.print("Attempting MQTT connection...");
            // Create a random client ID
            String clientId = "ESP32Client-";
            clientId += String(random(0xffff), HEX);
            // Attempt to connect
            if (client->connect(clientId.c_str())) {
                Serial.println("connected");
                // Once connected, publish an announcement...
                client->publish("outTopic", "hello world");
                // ... and resubscribe
                client->subscribe("inTopic");
            } else {
                Serial.print("failed, rc=");
                Serial.print(client->state());
                Serial.println(" try again in 5 seconds");
                // Wait 5 seconds before retrying
                delay(5000);
            }
        }
    }
}