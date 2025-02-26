#include "MQTTManager.hpp"

WiFiClient wifiClient;
PubSubClient client(wifiClient);


//SUBSCRIBE TOPICS
char mqttServer[]     = SERVER_IP;
char clientId[]       = CLIENT_ID;
char subtopic1[] = CHUNK_TOPIC_1_SUB;
char subtopic2[] = CHUNK_TOPIC_2_SUB;
char subtopic3[] = CHUNK_TOPIC_3_SUB ;
char controltopic[50] = CONTROL_TOPIC_SUB;

//PUBLISHING TOPICS
char devicetopic[50] = DEVICE_TOPIC_PUB;
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
    client.setServer(mqttServer, SERVER_PORT);
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
                  client.subscribe(subtopic1);
                  client.subscribe(subtopic2);
                  client.subscribe(subtopic3);
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
    
    String message;       //430 bytes per chunk limit
    // fileBuffer.reserve(76 * 1024);  // Request 76KB
    // if (fileBuffer.length() == 0) {
    //     Serial.println("Memory allocated successfully.");
    // } else {
    //     Serial.println("Memory allocation failed.");
    // }
    
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.println(message);

    if (String(topic) == subtopic2 ) {
        totalChunks = message.toInt();
        receivedChunks = 0;
        fileBuffer = "";  // Reset buffer
        Serial.print("Expecting ");
        Serial.print(totalChunks);
        Serial.println(" chunks.");
    } 

     else if (String(topic).startsWith("controller/filetransfer/chunk/")){
        fileBuffer += message;  // Append chunk
        receivedChunks++;

        Serial.print("Received chunk ");
        Serial.print(receivedChunks);
        Serial.print(" of ");
        Serial.println(totalChunks);
    } 
    else if (String(topic) == subtopic3 ) {
        if (receivedChunks  == totalChunks ) {
     

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