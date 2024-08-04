#include <AccelStepper.h>
#include <Arduino.h>
#include "utils/wifiConnection.h"
#include <MultiStepper.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>
#include "WiFi.h"
#include <ArduinoJson.h>

#define MQTT_HOST IPAddress(192, 168, 50, 199)
#define MQTT_PORT 1883

AccelStepper xStepper1(AccelStepper::DRIVER, 5, 2); // X Axis on the CNC shield
AccelStepper xStepper2(AccelStepper::DRIVER, 6, 3); // Y Axis on the CNC shield

MultiStepper xSteppers;

const char* ssid = "ASUS";
const char* password = "13601748441";
const char* mqtt_server = "192.168.50.199";

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

WiFiEvent_t wifiConnectHandler;
WiFiEvent_t wifiDisconnectHandler;
Ticker wifiReconnectTimer;

JsonDocument stats;

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void connectToMqtt() {
    Serial.println("Connecting to MQTT...");
    mqttClient.connect();
}

void onMqttConnect(bool sessionPresent) {
    Serial.println("Connected to MQTT.");
    Serial.print("Session present: ");
    Serial.println(sessionPresent);
    uint16_t packetIdSub = mqttClient.subscribe("test/lol", 2);
    Serial.print("Subscribing at QoS 2, packetId: ");
    Serial.println(packetIdSub);
    mqttClient.publish("test/lol", 0, true, "test 1");
    Serial.println("Publishing at QoS 0");
    uint16_t packetIdPub1 = mqttClient.publish("test/lol", 1, true, "test 2");
    Serial.print("Publishing at QoS 1, packetId: ");
    Serial.println(packetIdPub1);
    uint16_t packetIdPub2 = mqttClient.publish("test/lol", 2, true, "test 3");
    Serial.print("Publishing at QoS 2, packetId: ");
    Serial.println(packetIdPub2);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
    Serial.println("Disconnected from MQTT.");

    if (WiFi.isConnected()) {
        mqttReconnectTimer.once(2, connectToMqtt);
    }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
    Serial.println("Subscribe acknowledged.");
    Serial.print("  packetId: ");
    Serial.println(packetId);
    Serial.print("  qos: ");
    Serial.println(qos);
}

void onMqttUnsubscribe(uint16_t packetId) {
    Serial.println("Unsubscribe acknowledged.");
    Serial.print("  packetId: ");
    Serial.println(packetId);
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
    Serial.println("Publish received.");
    Serial.print("  topic: ");
    Serial.println(topic);
    Serial.print("  qos: ");
    Serial.println(properties.qos);
    Serial.print("  dup: ");
    Serial.println(properties.dup);
    Serial.print("  retain: ");
    Serial.println(properties.retain);
    Serial.print("  len: ");
    Serial.println(len);
    Serial.print("  index: ");
    Serial.println(index);
    Serial.print("  total: ");
    Serial.println(total);
}

void onMqttPublish(uint16_t packetId) {
    Serial.println("Publish acknowledged.");
    Serial.print("  packetId: ");
    Serial.println(packetId);
}

void connectToWifi() {
    Serial.println("Connecting to Wi-Fi...");
    WiFi.begin(ssid, password);
}

void onWifiConnect(WiFiEvent_t event) {
    Serial.println("Connected to Wi-Fi.");
    connectToMqtt();
}

void onWifiDisconnect(WiFiEvent_t event) {
    Serial.println("Disconnected from Wi-Fi.");
    mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
    wifiReconnectTimer.once(2, connectToWifi);
}

void setup()
{
    WiFi.onEvent(onWifiConnect, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
    WiFi.onEvent(onWifiDisconnect, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

    Serial.begin(9600);

    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    mqttClient.onSubscribe(onMqttSubscribe);
    mqttClient.onUnsubscribe(onMqttUnsubscribe);
    mqttClient.onMessage(onMqttMessage);
    mqttClient.onPublish(onMqttPublish);
    mqttClient.setServer(MQTT_HOST, MQTT_PORT);

    connectToWifi();

    xStepper1.setMaxSpeed(100);
    xStepper2.setMaxSpeed(100);

    xStepper1.setCurrentPosition(0);
    xStepper2.setCurrentPosition(0);

    xSteppers.addStepper(xStepper1);
    xSteppers.addStepper(xStepper2);

}


void loop()
{
//    client.poll();
    char output[128];

    size_t size = serializeMsgPack(stats, output, 128);

    mqttClient.publish("topic", 0, false, output, size);

    long positions[2]; // Array of desired stepper positions

    positions[0] = 99999999;
    positions[1] = 99999999;
    xSteppers.moveTo(positions);

    xSteppers.run();
    stats["x"] = xStepper1.currentPosition();


//    sendStats(client, websockets_url, stats);
}
