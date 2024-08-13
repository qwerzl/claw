/// ------ PROGRAM CONFIGURATIONS ------
#define WIFI_SSID "ASUS"
#define WIFI_PASSWORD "13601748441"
#define WIFI 0
#define MQTT_SERVER "192.168.50.199"

#define X_AXIS_MAX_VALUE 1773
#define Y_AXIS_MAX_VALUE 2573

/// ------ LIBRARY IMPORTS ------
#include "Arduino.h"
#include "AccelStepper.h"
#include "MultiStepper.h"
#include "ArduinoJson.h"
#include "Wire.h"

#if WIFI == 1
#include "utils/wifiConnection.h"
#include <PubSubClient.h>
#include "utils/mqtt.h"
WiFiClient espClient;
PubSubClient client(espClient);
#endif

TaskHandle_t steppersTask;
TaskHandle_t dataCollectionTask;

AccelStepper xStepper1(AccelStepper::DRIVER, D5, D2); // X Axis on the CNC shield
AccelStepper xStepper2(AccelStepper::DRIVER, D6, D3); // Y Axis on the CNC shield
AccelStepper yStepper(AccelStepper::DRIVER, D7, D4); // Z Axis on the CNC shield

MultiStepper steppers;

enum dir {
    XPositive,
    XNegative,
    YPositive,
    YNegative,
    Still
};

dir currentDir = Still;

[[noreturn]] void steppersTaskFunc( void * pvParameters ){
    for(;;){
        long positions[3];

        positions[0] = xStepper1.currentPosition();
        positions[1] = xStepper2.currentPosition();
        positions[2] = yStepper.currentPosition();

        int command = Serial0.readStringUntil('\n').toInt();

        if (command == 1) {
            currentDir = XPositive;
        } else if (command == 2) {
            currentDir = YPositive;
        } else if (command == 3) {
            currentDir = XNegative;
        } else if (command == 4) {
            currentDir = YNegative;
        } else if (command == 5) {
            currentDir = Still;
        }

        Serial.println(currentDir);

        switch (currentDir) {
            case XPositive:
                positions[0] = X_AXIS_MAX_VALUE;
                positions[1] = X_AXIS_MAX_VALUE;
                break;
            case YPositive:
                positions[2] = Y_AXIS_MAX_VALUE;
                break;
            case XNegative:
                positions[0] = 0;
                positions[1] = 0;
                break;
            case YNegative:
                positions[2] = 0;
                break;
            default:
                break;
        }

        steppers.moveTo(positions);

        steppers.run();
    }
}

[[noreturn]] void dataCollectionTaskFunc( void * pvParameters ){
    for(;;){
        Serial.print("STATS | ");

        JsonDocument stats;
        stats["x"] = xStepper1.currentPosition();
        stats["y"] = yStepper.currentPosition();

        char buffer[100];

        int bytesWritten = serializeMsgPack(stats, buffer, 100);

#if WIFI == 0
        for(int i = 0; i<bytesWritten; i++){
            Serial.printf("%02X ",buffer[i]);
        }
        Serial.println();
#else
        if (!client.connected()) {
            mqtt::reconnect(&client);
        }
        client.loop();
        client.publish("out", buffer);
#endif
        delay(1000);
    }
}

void setup() {
    Serial.begin(115200);
    Serial0.begin(115200);
    Serial0.setTimeout(1);

#if WIFI == 1
    initWiFi(WIFI_SSID, WIFI_PASSWORD);
    client.setServer(MQTT_SERVER, 1883);
    client.setCallback(callback);
#endif

//    pinMode(JOYSTICK_NEGATIVE_X_PIN, INPUT_PULLUP);
//    pinMode(JOYSTICK_NEGATIVE_Y_PIN, INPUT_PULLUP);
//    pinMode(JOYSTICK_POSITIVE_X_PIN, INPUT_PULLUP);
//    pinMode(JOYSTICK_POSITIVE_Y_PIN, INPUT_PULLUP);

    xStepper1.setMaxSpeed(100);
    xStepper2.setMaxSpeed(100);
    yStepper.setMaxSpeed(100);

    xStepper1.setCurrentPosition(0);
    xStepper2.setCurrentPosition(0);
    yStepper.setCurrentPosition(0);

    steppers.addStepper(xStepper1);
    steppers.addStepper(xStepper2);
    steppers.addStepper(yStepper);


    //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
    xTaskCreatePinnedToCore(
            steppersTaskFunc,   /* Task function. */
            "Task1",     /* name of task. */
            10000,       /* Stack size of task */
            nullptr,        /* parameter of the task */
            1,           /* priority of the task */
            &steppersTask,      /* Task handle to keep track of created task */
            1);          /* pin task to core 0 */
    delay(500);

    xTaskCreatePinnedToCore(
            dataCollectionTaskFunc,   /* Task function. */
            "Task2",     /* name of task. */
            10000,       /* Stack size of task */
            nullptr,        /* parameter of the task */
            1,           /* priority of the task */
            &dataCollectionTask,      /* Task handle to keep track of created task */
            0);          /* pin task to core 1 */
    delay(500);
}

void loop() {

}