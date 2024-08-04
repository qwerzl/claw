#ifndef CLAW_WEBSOCKETS_H
#define CLAW_WEBSOCKETS_H
#endif //CLAW_WEBSOCKETS_H

#include <Arduino.h>
#include <ArduinoWebsockets.h>

void onMessageCallback(const websockets::WebsocketsMessage& message) {
    Serial.print("Got Message: ");
    Serial.println(message.data());
}

void onEventsCallback(websockets::WebsocketsEvent event, String data) {
    if(event == websockets::WebsocketsEvent::ConnectionOpened) {
        Serial.println("Connnection Opened");
    } else if(event == websockets::WebsocketsEvent::ConnectionClosed) {
        Serial.println("Connnection Closed");
    } else if(event == websockets::WebsocketsEvent::GotPing) {
        Serial.println("Got a Ping!");
    } else if(event == websockets::WebsocketsEvent::GotPong) {
        Serial.println("Got a Pong!");
    }
}

void initWebsockets(websockets::WebsocketsClient* client, const char* websockets_url) {
    client->onMessage(onMessageCallback);
    client->onEvent(onEventsCallback);

    // Connect to server
    client->connect(websockets_url);

    // Send a message
    client->send("Hi Server!");
    // Send a ping
    client->ping();
}