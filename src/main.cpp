#include <AccelStepper.h>
#include <Arduino.h>
#include "utils/wifiConnection.h"
#include <MultiStepper.h>
#include <ArduinoWebsockets.h>
#include "utils/websockets.h"

AccelStepper xStepper1(AccelStepper::DRIVER, 5, 2); // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5
AccelStepper xStepper2(AccelStepper::DRIVER, 6, 3); // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5

MultiStepper xSteppers;

const char* ssid = "The Weave";
const char* password = "weave277";
const char* websockets_url = "ws://10.249.18.172:8080";

websockets::WebsocketsClient client;

void setup()
{
    Serial.begin(9600);
    initWiFi(ssid, password);
    xStepper1.setMaxSpeed(100);
    xStepper2.setMaxSpeed(100);

    xSteppers.addStepper(xStepper1);
    xSteppers.addStepper(xStepper2);

    initWebsockets(&client, websockets_url);
}

void loop()
{
    client.poll();
    long positions[2]; // Array of desired stepper positions

    positions[0] = 99999999;
    positions[1] = 99999999;
    xSteppers.moveTo(positions);

    xSteppers.run();
}
