/// ------ PROGRAM CONFIGURATIONS ------
#define WIFI_SSID "ASUS"
#define WIFI_PASSWORD "13601748441"
#define WIFI 0
#define MQTT_SERVER "192.168.50.199"

/// ------ LIBRARY IMPORTS ------
#include "Arduino.h"
#include "AccelStepper.h"
#include "MultiStepper.h"
#include "ArduinoJson.h"

#if WIFI == 1
#include "utils/wifiConnection.h"
#include <PubSubClient.h>
#include "utils/mqtt.h"
WiFiClient espClient;
PubSubClient client(espClient);
#endif

TaskHandle_t steppersTask;
TaskHandle_t dataCollectionTask;

AccelStepper xStepper1(AccelStepper::DRIVER, 5, 2); // X Axis on the CNC shield
AccelStepper xStepper2(AccelStepper::DRIVER, 6, 3); // Y Axis on the CNC shield

MultiStepper xSteppers;

[[noreturn]] void steppersTaskFunc( void * pvParameters ){
    for(;;){
        long positions[2]; // Array of desired stepper positions

        positions[0] = 99999999;
        positions[1] = 99999999;
        xSteppers.moveTo(positions);

        xSteppers.run();

        delay(1);
    }
}

[[noreturn]] void dataCollectionTaskFunc( void * pvParameters ){
    for(;;){
        Serial.print("STATS | ");

        JsonDocument stats;
        stats["x"] = xStepper1.currentPosition();

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

#if WIFI == 1
    initWiFi(WIFI_SSID, WIFI_PASSWORD);
    client.setServer(MQTT_SERVER, 1883);
    client.setCallback(callback);
#endif

    xStepper1.setMaxSpeed(100);
    xStepper2.setMaxSpeed(100);

    xStepper1.setCurrentPosition(0);
    xStepper2.setCurrentPosition(0);

    xSteppers.addStepper(xStepper1);
    xSteppers.addStepper(xStepper2);


    //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
    xTaskCreatePinnedToCore(
            steppersTaskFunc,   /* Task function. */
            "Task1",     /* name of task. */
            10000,       /* Stack size of task */
            nullptr,        /* parameter of the task */
            1,           /* priority of the task */
            &steppersTask,      /* Task handle to keep track of created task */
            0);          /* pin task to core 0 */
    delay(500);

    xTaskCreatePinnedToCore(
            dataCollectionTaskFunc,   /* Task function. */
            "Task2",     /* name of task. */
            10000,       /* Stack size of task */
            nullptr,        /* parameter of the task */
            1,           /* priority of the task */
            &dataCollectionTask,      /* Task handle to keep track of created task */
            1);          /* pin task to core 1 */
    delay(500);
}

void loop() {

}