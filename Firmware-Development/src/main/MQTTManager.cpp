#include "MQTTManager.hpp"

WiFiClient wifiClient;
PubSubClient client(wifiClient);


//SUBSCRIBE TOPICS
char mqttServer[]     = "184.169.181.179";
char clientId[]       = "amebaClient";
char subtopic1[] = "controller/filetransfer/chunk/#";
char subtopic2[] = "controller/filetransfer/chunkCount";
char subtopic3[] = "controller/filetransfer/complete";
char controltopic[50] = "led/control/";

//PUBLISHING TOPICS
char devicetopic[50] = "led/device/";
// char devicetopic_payload[] = "hello world";



String fileBuffer = "";
int totalChunks = 0;
int receivedChunks = 0;




void conc(){

    char *macStrrecived = BLEManager::getmacStr();    
    if (strlen(controltopic) + strlen(macStrrecived) < sizeof(controltopic)) {
        strcat(controltopic, macStrrecived);
    } else {
        Serial.println("Buffer overflow risk!");
    }
    if (strlen(devicetopic) + strlen(macStrrecived) < sizeof(devicetopic)) {
        strcat(devicetopic, macStrrecived);
    } else {
        Serial.println("Buffer overflow risk!");
    }

}

void MQTTManager::init() {
    client.setServer(mqttServer, 1883);
    client.setCallback(callback);
}

void MQTTManager::loop(){
  client.loop();
}

void MQTTManager::disconnect(){
  client.disconnect();
}

void MQTTManager::publish_JSON(){

      String publishable_data = JSONManager::make_mqtt_JSON();
         // Publish JSON string over MQTT
      if (client.publish(devicetopic, publishable_data.c_str())) {
          Serial.println("JSON message sent: " + publishable_data);
      } else {
          Serial.println("Failed to send JSON message");
      }


}

void MQTTManager::attemptConnection() {
    conc();
    Serial.print("\r\nAttempting MQTT connection...");
              // Attempt to connect
              if (client.connect(clientId)) {
                  BLEManager::updateDeviceStatus();
                  Serial.println("connected");
                  //Once connected, publish an announcement and resubscribe
                  client.subscribe("controller/filetransfer/chunk/#");
                  client.subscribe("controller/filetransfer/chunkCount");
                  client.subscribe("controller/filetransfer/complete");
                  client.subscribe(controltopic);
                  // client.publish(devicetopic, devicetopic_payload);

              } else {
                  BLEManager::updateDeviceStatus();
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
    Serial.print("MQTT JSON recieved [");  
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
 
            // JSONManager::parse_mqtt_JSON(fileBuffer);
            // TODO: Handle the parsed JSON data
        } else {
            Serial.println("Error: Incomplete file transfer!");
      }

    }
    else if(String(topic) == controltopic ){
            JSONManager::parse_mqtt_JSON(message);
           
    }
}