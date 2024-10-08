/// ------ LIBRARY IMPORTS ------
#include "Arduino.h"
#include "AccelStepper.h"
#include "MultiStepper.h"
#include "ArduinoJson.h"
#include "Wire.h"
#include <semphr.h>

#include "shared.h"

/// ------ PROGRAM CONFIGURATIONS ------
#define X_AXIS_MAX_VALUE (-62200)
#define Y_AXIS_MAX_VALUE (150000)
#define Z_AXIS_MAX_VALUE (15000) //TODO: Determine Z axis max value
#define STATS_COLLECTION_INTERVAL 1000

TaskHandle_t steppersTask;
TaskHandle_t dataCollectionTask;

AccelStepper xStepper1(AccelStepper::DRIVER, D5, D2); // X Axis on the CNC shield
AccelStepper yStepper(AccelStepper::DRIVER, D7, D4); // Y Axis on the CNC shield
AccelStepper zStepper(AccelStepper::DRIVER, D6, D3);

MultiStepper steppers;

command currentDir = Still;
bool zOn = false;

long positions[4];

SemaphoreHandle_t positionsMutex;

[[noreturn]] void steppersTaskFunc( void * pvParameters ){
    for(;;){
        if ( xSemaphoreTake( positionsMutex, ( TickType_t ) 2 ) == pdTRUE )
        {
            if (currentDir == Still) {
                positions[0] = xStepper1.currentPosition();
                positions[1] = yStepper.currentPosition();
            }
            if (!zOn) {
                positions[2] = zStepper.currentPosition();
            }
            xSemaphoreGive( positionsMutex ); // Now free or "Give" the Serial Port for others.
        }

        steppers.moveTo(positions);

        steppers.run();

//        delay(2);
    }
}

[[noreturn]] void dataCollectionTaskFunc( void * pvParameters ){
    long last_collected = millis();
    for(;;){

        if (millis()-last_collected > STATS_COLLECTION_INTERVAL) {
            Serial0.print("STATS | ");
            JsonDocument stats;
            stats["x"] = xStepper1.currentPosition();
            stats["y"] = yStepper.currentPosition();
            stats["z"] = zStepper.currentPosition();

            char buffer[100];

            int bytesWritten = serializeMsgPack(stats, buffer, 100);

            for(int i = 0; i<bytesWritten; i++){
                Serial0.printf("%02X ",buffer[i]);
            }
            Serial0.println();
            last_collected = millis();
        }

        String receivedCommand = Serial0.readStringUntil('\n');

        if (!receivedCommand.isEmpty()) {
            auto processedReceivedCommand = static_cast<command>(receivedCommand.toInt());
            if (receivedCommand) {
                Serial.println(receivedCommand);
                if (processedReceivedCommand == BUTTON_TOGGLE) {
                    Serial.println("Button Toggled");
//                    if (xStepper1.currentPosition() in some area && yStepper.currentPosition() in some area) toggle EM
                    zOn = !zOn;
                }
                else currentDir = processedReceivedCommand;
            }
        }

        if ( xSemaphoreTake( positionsMutex, ( TickType_t ) 5 ) == pdTRUE )
        {
            switch (currentDir) {
                case YPositive:
                    positions[1] = Y_AXIS_MAX_VALUE;
                    break;
                case XPositive:
                    positions[0] = X_AXIS_MAX_VALUE;
                    break;
                case YNegative:
                    positions[1] = 0;
                    break;
                case XNegative:
                    positions[0] = 0;
                    break;
                default:
                    break;
            }
            if (zOn) {
                if (abs(positions[2] - zStepper.currentPosition()) < 5){
                    if (positions[2] == Z_AXIS_MAX_VALUE) {
                        positions[2] = 0;
                    } else {
                        positions[2] = Z_AXIS_MAX_VALUE;
                    }
                }
            }
            xSemaphoreGive( positionsMutex ); // Now free or "Give" the Serial Port for others.
        }

        delay(20);
    }
}

void setup() {
    Serial.begin(115200);
    Serial0.begin(115200);
    Serial0.setTimeout(1);

    xStepper1.setMaxSpeed(15000);
//    xStepper2.setMaxSpeed(600);
    yStepper.setMaxSpeed(14000);
    zStepper.setMaxSpeed(5000);

    xStepper1.setCurrentPosition(0);
//    xStepper2.setCurrentPosition(0);
    yStepper.setCurrentPosition(0);
    zStepper.setCurrentPosition(0);

    steppers.addStepper(xStepper1);
//    steppers.addStepper(xStepper2);
    steppers.addStepper(yStepper);
    steppers.addStepper(zStepper);

    if ( positionsMutex == nullptr )  // Check to confirm that the Serial Semaphore has not already been created.
    {
        positionsMutex = xSemaphoreCreateMutex();  // Create a mutex semaphore we will use to manage the Serial Port
        if ( ( positionsMutex ) != nullptr )
            xSemaphoreGive( ( positionsMutex ) );  // Make the Serial Port available for use, by "Giving" the Semaphore.
    }

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