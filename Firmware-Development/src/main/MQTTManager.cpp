#include "MQTTManager.hpp"
#include "OTAManager.hpp"

WiFiClient wifiClient;
PubSubClient client(wifiClient);


//SUBSCRIBE TOPICS
char mqttServer[] = SERVER_IP;
char clientId[] = CLIENT_ID;
char chunkTopic1Sub[] = CHUNK_TOPIC_1_SUB;
char chunkTopic2Sub[] = CHUNK_TOPIC_2_SUB;
char chunkTopic3Sub[] = CHUNK_TOPIC_3_SUB;
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

bool profileRecivedFlag = false;
bool ledConfigRecieved = false;
bool otaUpdateAvailable = false;

void conc() {
  char* macStrrecived = BLEManager::getmacStr();
  char* topics[] = {controltopic, devicetopic, profiletopic, otatopic};

  for (int i = 0; i < 4; i++) {
    if (strlen(topics[i]) + strlen(macStrrecived) < sizeof(topics[i])) {
      strcat(topics[i], macStrrecived);
    } else {
      Serial.println("Buffer overflow risk!");
    }
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
    char* topics[] = {
    chunkTopic1Sub,
    chunkTopic2Sub,
    chunkTopic3Sub,
    profiletopic,
    controltopic,
    otatopic,
    otaupdate
    };

    for (int i = 0; i < sizeof(topics) / sizeof(topics[0]); i++) {
        client.subscribe(topics[i]);
    }

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
    profileRecivedFlag = write_val;
  } else if (read == true) {
    return (profileRecivedFlag);
  }
  return (profileRecivedFlag);
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

void MQTTManager::handleControlTopics(const String &message)
{

  if (JSONManager::parseJSON(message, "power").startsWith("true"))
  {
    LED::getInstance().config.power = true;
  }
  else if (JSONManager::parseJSON(message, "power").startsWith("false"))
  {
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

void MQTTManager::handleProfileTopic(const String &message)
{
  profileRecivedFlag = true;
  LED::getInstance().config.profileName = JSONManager::parseJSON(message, "profileName");
  Serial.println("Profile Name: " + LED::getInstance().config.profileName);
}

void MQTTManager::handleOtaTopic(const String &message)
{
  ota.version = JSONManager::parseJSON(message, "ota.version");
  ota.url = JSONManager::parseJSON(message, "ota.url");

  Serial.println("Version: " + ota.version);
  Serial.println("URL: " + ota.url);
}

void MQTTManager::handleOtaUpdate(const String &message){
  if (JSONManager::parseJSON(message, "message").startsWith("New OTA update available")) {
    otaUpdateAvailable = true;
  }
}

void MQTTManager::handleChunkTopic3Sub(const String &message){
  if (receivedChunks == totalChunks) {
    Serial.println("All chunks received. Parsing JSON...");
    // TODO: Handle the parsed JSON data
  } else {
    Serial.println("Error: Incomplete file transfer!");
  }
}

void MQTTManager::handleChunkTopic2Sub(const String &message){
  totalChunks = message.toInt();
  receivedChunks = 0;
  fileBuffer = "";  // Reset buffer
  Serial.print("Expecting ");
  Serial.print(totalChunks);
  Serial.println(" chunks.");
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

  if (String(topic) == chunkTopic2Sub) {
    handleChunkTopic2Sub(message);
  }

  else if (String(topic).startsWith("controller/filetransfer/chunk/")) {
    fileBuffer += message;  // Append chunk
    receivedChunks++;
    Serial.print("Received chunk ");
    Serial.print(receivedChunks);
    Serial.print(" of ");
    Serial.println(totalChunks);
  }
  else if (String(topic) == chunkTopic3Sub) {
    handleChunkTopic3Sub(message);
  }
  else if (String(topic) == controltopic) {
    handleControlTopics(message);
  }

  else if (String(topic) == profiletopic)
  {
    handleProfileTopic(message);
  }
  else if (String(topic) == otatopic)
  {
    handleOtaTopic(message);
  }
  else if (String(topic) == otaupdate)
  {
    handleOtaUpdate(message);
  }
}