#include <EEPROM.h>
#include <FS.h>
#include <LittleFS.h>
#include "Sidekick.h"
#include "storage.h"

String resourcesStart = "";
int SIZE_config_data;

// Initialize the SPIFFS in flas memory, if we has no SPIFFS yet, format it!
void storage_init() {
  if(!LittleFS.begin()){
    console_log("Formating storage...");
    LittleFS.format();
    console_log(" [OK]\n");
  }
  console_log("Init storage device... [OK]\n");
}

// Save all configuration values to a file into storage
void save_CFG() {
  File storage = LittleFS.open("/config.bin", "w");
  if (storage) {
    console_log("<SAVE CONFIG>\n");
    storage.write(CFG.raw, sizeof(config_data));
  }
  storage.close();
  LittleFS.end();
  LittleFS.begin();
}

// Failsafe values, used when we has no configuration yet
void set_failsafe_CFG() {
  String("sidekick").toCharArray(CFG.data.serverName,32);
  String(DEFAULT_PASSWORD).toCharArray(CFG.data.password, 32);
  String(DEFAULT_AP_SSID).toCharArray(CFG.data.AP.SSID,32);
  String(DEFAULT_AP_PASS).toCharArray(CFG.data.AP.password,32);
}

// Read configuration file and place the values into the CFG struct
void load_CFG() {
  console_log("Load configuration...");
  activity(FLASH);
  File storage = LittleFS.open("/config.bin", "r");
  if (storage) {
    uint32_t nBytes = storage.readBytes((char*)CFG.raw, sizeof(config_data));
    console_log(" (load) ");
  } else {
    console_log(" (failsafe mode)...");
    set_failsafe_CFG();
  }
  storage.close();
  activity(FLASH);
  delay(1000);
  console_log(" [OK]\n");
}

// Retrieve a file contents
String getFile(String filename) {
  File storage = LittleFS.open(filename, "r");
  String contents = "";
  while (storage.available()){
    contents += char(storage.read());
  }
  storage.close();
  return(contents);
}

void resourcesHeader(String header_name) {
  resourcesStart = header_name + " Init: ";
}

// Add a new resource value
void resourcesAddValue(String item_name, String item_value) {
  resourcesJson += ",\"" + item_name + "\":\"" + item_value + "\"";
}

// Add a new item value, used when create an array resource
void resourcesAddItem(String item_value, int item_pin) {
  if(resourcesList != "") {
    resourcesList += ",";
  } else {
    console_log(resourcesStart);
  }
  resourcesList += "\"" + item_value + "\"";
  console_log("(");
  console_log(item_value);
  console_log("=");
  console_log(String(item_pin));
  console_log(") ");
}

// Add an array resource and empty the item list
void resourcesAddArray(String array_name) {
  if(resourcesList.length() > 0) {
    resourcesJson += ",\"" + array_name + "\":[" + resourcesList + "]";
    console_log("[OK]\n");
  }
  resourcesList = "";
}