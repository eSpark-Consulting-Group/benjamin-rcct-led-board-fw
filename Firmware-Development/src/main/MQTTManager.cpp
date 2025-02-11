#include "MQTTManager.hpp"


WiFiClient wifiClient;
PubSubClient client(wifiClient);

char mqttServer[]     = "184.169.181.179";
char clientId[]       = "amebaClient";
char subtopic1[] = "controller/filetransfer/chunk/#";
char subtopic2[] = "controller/filetransfer/chunkCount";
char subtopic3[] = "controller/filetransfer/complete";

String fileBuffer = "";
int totalChunks = 0;
int receivedChunks = 0;
uint8_t brightness1;

String test;

void MQTTManager::init() {
    client.setServer(mqttServer, 1883);
    client.setCallback(callback);
}

void MQTTManager::loop(){
  client.loop();
}

void MQTTManager::attemptConnection() {
    Serial.print("\r\nAttempting MQTT connection...");
              // Attempt to connect
              if (client.connect(clientId)) {
                  Serial.println("connected");
                  //Once connected, publish an announcement and resubscribe
                  client.subscribe("controller/filetransfer/chunk/#");
                  client.subscribe("controller/filetransfer/chunkCount");
                  client.subscribe("controller/filetransfer/complete");

              } else {
                  Serial.println("failed, rc=");
                  Serial.print(client.state());
                  Serial.println(" try again in 5 seconds");
                  //Wait 5 seconds before retrying
                  vTaskDelay(pdMS_TO_TICKS(5000)); 
              }

}

bool MQTTManager::isConnected() {
    return client.connected();
}

void MQTTManager::callback(char* topic, byte* payload, unsigned int length) {
Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    
    String message;
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.println(message);

    if (String(topic) == "controller/filetransfer/chunkCount") {
        totalChunks = message.toInt();
        receivedChunks = 0;
        fileBuffer = "";  // Reset buffer
        Serial.print("Expecting ");
        Serial.print(totalChunks);
        Serial.println(" chunks.");
    } 
    else if (String(topic).startsWith("controller/filetransfer/chunk/")) {
        fileBuffer += message;  // Append chunk
        receivedChunks++;

        Serial.print("Received chunk ");
        Serial.print(receivedChunks);
        Serial.print(" of ");
        Serial.println(totalChunks);
    } 
    else if (String(topic) == "controller/filetransfer/complete") {
        if (receivedChunks == totalChunks) {
            Serial.println("All chunks received. Parsing JSON...");

            JSONVar json = JSON.parse(fileBuffer);
            if (JSON.typeof(json) == "undefined") {
                Serial.println("Failed to parse JSON!");
                return;
            }

            Serial.println("Parsed JSON:");
            Serial.println(JSON.stringify(json));
            test = (const char*)json["test_param"];        

            brightness1 = (uint8_t)(int)json["brightness1"];

            Serial.print("brightness1: ");
            Serial.println(brightness1);
            Serial.print("testing: ");
            Serial.println(test);

            // TODO: Handle the parsed JSON data
        } else {
            Serial.println("Error: Incomplete file transfer!");
        }
    }
}