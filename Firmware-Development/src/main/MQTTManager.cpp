#include "MQTTManager.hpp"
#include "OTAManager.hpp"

WiFiClient wifiClient;
PubSubClient client(wifiClient);


//SUBSCRIBE TOPICS
char mqttServer[] = SERVER_IP;
char clientId[] = CLIENT_ID;
char subtopic1[] = CHUNK_TOPIC_1_SUB;
char subtopic2[] = CHUNK_TOPIC_2_SUB;
char subtopic3[] = CHUNK_TOPIC_3_SUB;
char controltopic[50] = CONTROL_TOPIC_SUB;
char profiletopic[50] = PROFILE_TOPIC_SUB;
char otatopic[50] = OTA_TOPIC_SUB;
char otaupdate[50] = "led/ota/all";


//PUBLISHING TOPICS
char devicetopic[50] = DEVICE_TOPIC_PUB;
// char devicetopic_payload[] = "hello world";



String fileBuffer = "";
int totalChunks = 0;
int receivedChunks = 0;

bool profileNameRecived = false;
bool ledConfigRecieved = false;
bool otaUpdateAvailable = false;

void conc() {

  char* macStrrecived = BLEManager::getmacStr();
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
  if (strlen(profiletopic) + strlen(macStrrecived) < sizeof(profiletopic)) {
    strcat(profiletopic, macStrrecived);
  } else {
    Serial.println("Buffer overflow risk!");
  }
  if (strlen(otatopic) + strlen(macStrrecived) < sizeof(otatopic)) {
    strcat(otatopic, macStrrecived);
  } else {
    Serial.println("Buffer overflow risk!");
  }
}

void MQTTManager::init() {
  client.setServer(mqttServer, SERVER_PORT);
  client.setCallback(callback);
}

void MQTTManager::loop() {
  client.loop();
}

void MQTTManager::disconnect() {
  client.disconnect();
}

void MQTTManager::publishLedStatePayload() {

  const int numProps = 5;

  String keys[numProps] = { "Power", "Brightness", "color", "effect", "speed" };

  String values[numProps] = {
    POWER,
    BRIGHTNESS,
    COLOR,
    EFFECT,
    SPEED
  };

  String payload = JSONManager::makeJSON(numProps, keys, values);

  // Publish JSON string over MQTT
  if (client.publish(devicetopic, payload.c_str())) {
    Serial.println("JSON message sent: " + payload);
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
    client.subscribe(profiletopic);
    client.subscribe(controltopic);
    client.subscribe(otatopic);
    client.subscribe(otaupdate);
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

bool MQTTManager::profileNameRecivedFlag(bool write, bool write_val, bool read) {
  if (write == true) {
    profileNameRecived = write_val;
  } else if (read == true) {
    return (profileNameRecived);
  }
  return (profileNameRecived);
}

bool MQTTManager::ledConfigRecievedFlag(bool write, bool write_val, bool read) {
  if (write == true) {
    ledConfigRecieved = write_val;
  } else if (read == true) {
    return (ledConfigRecieved);
  }
  return (ledConfigRecieved);
}

void MQTTManager::setOtaUpdateAvailable(bool state) {
  otaUpdateAvailable = state;
}

bool MQTTManager::getOtaUpdateAvailable() {
  return (otaUpdateAvailable);
}

void MQTTManager::callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("MQTT JSON recieved [");
  Serial.print(topic);
  Serial.print("] ");

  String message;  //430 bytes per chunk limit


  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  if (String(topic) == subtopic2) {
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
  } else if (String(topic) == subtopic3) {
    if (receivedChunks == totalChunks) {


      Serial.println("All chunks received. Parsing JSON...");


      // TODO: Handle the parsed JSON data
    } else {
      Serial.println("Error: Incomplete file transfer!");
    }

  } else if (String(topic) == controltopic) {

    if (JSONManager::parseJSON(message, "power").startsWith("true")) {
      LED::getInstance().config.power = true;
    } else if (JSONManager::parseJSON(message, "power").startsWith("false")) {
      LED::getInstance().config.power = false;
    }

    LED::getInstance().config.color.colorString = JSONManager::parseJSON(message, "color");
    LED::getInstance().config.effect = JSONManager::parseJSON(message, "effect");
    LED::getInstance().config.speed = (uint16_t)atoi((JSONManager::parseJSON(message, "speed")).c_str());
    LED::getInstance().config.brightness = (uint16_t)atoi((JSONManager::parseJSON(message, "brightness")).c_str());


    Serial.print("Power: ");
    Serial.println(LED::getInstance().config.power);
    Serial.println("Color: " + LED::getInstance().config.color.colorString);
    Serial.println("Effect: " + LED::getInstance().config.effect);
    Serial.print("Brightness: ");
    Serial.println(LED::getInstance().config.brightness);
    Serial.print("Speed: ");
    Serial.println(LED::getInstance().config.speed);
    ledConfigRecieved = true;

  }

  else if (String(topic) == profiletopic) {
    profileNameRecived = true;
    LED::getInstance().config.profileName = JSONManager::parseJSON(message, "profileName");
    Serial.println("Profile Name: " + LED::getInstance().config.profileName);

  } else if (String(topic) == otatopic) {
    ota.version = JSONManager::parseJSON(message, "ota.version");
    ota.url = JSONManager::parseJSON(message, "ota.url");

    Serial.println("Version: " + ota.version);
    Serial.println("URL: " + ota.url);

  } else if (String(topic) == otaupdate) {
    if (JSONManager::parseJSON(message, "message").startsWith("New OTA update available")) {
      otaUpdateAvailable = true;
    }
  }
}