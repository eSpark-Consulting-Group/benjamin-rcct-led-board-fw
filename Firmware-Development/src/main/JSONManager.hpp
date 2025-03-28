#pragma once

#include <Arduino_JSON.h>
#include "WiFi.h"
#include <set>

#include "BLEManager.hpp"
#include "MQTTManager.hpp"
#include "WiFiManager.hpp"
#include "LED.hpp"
#include "FlashManager.hpp"
#include "config.hpp"

typedef struct
{
  String key;           // Key name for the sub-JSON
  int numProps;         // Number of properties inside sub-JSON
  const String *keys;   // Array of keys for sub-JSON
  const String *values; // Array of values for sub-JSON
} SubJSON;

class JSONManager
{
public:
  static int gettotalSSIDChunks();
  static String *getSSIDChunkNumber();
  static void makeSSIDJSON(int n);
  static String parseJSON(String inputJSON, String property);
  static String jsonExtractFromString(String &response);
  static String makeJSON(int numProps, const String *keys, const String *values,
                         int numSubJSONs = 0, const SubJSON *subJSONs = nullptr);

private:
  static String convertToString(JSONVar value);
  static void addSubJSON(JSONVar &parent, const String &key, int numProps, const String *keys, const String *values);
};