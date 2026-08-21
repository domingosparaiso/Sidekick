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
  String(DEFAULT_USER).toCharArray(CFG.data.userName,32);
  String(DEFAULT_PASSWORD).toCharArray(CFG.data.password, 32);
  String(DEFAULT_SSID).toCharArray(CFG.data.CLI.wifi.SSID,32);
  String(DEFAULT_PASS).toCharArray(CFG.data.CLI.wifi.password,32);
  String(DEFAULT_AP_SSID).toCharArray(CFG.data.AP.SSID,32);
  String(DEFAULT_AP_PASS).toCharArray(CFG.data.AP.password,32);
  String(DEFAULT_HOSTNAME).toCharArray(CFG.data.hostname,32);
  CFG.data.CLI.DHCP = 0;
  CFG.data.CLI.IP[0]=192;CFG.data.CLI.IP[1]=168;CFG.data.CLI.IP[2]=1;CFG.data.CLI.IP[3]=50;
  CFG.data.CLI.GW[0]=192;CFG.data.CLI.GW[1]=168;CFG.data.CLI.GW[2]=1;CFG.data.CLI.GW[3]=10;
  CFG.data.CLI.MASK[0]=255;CFG.data.CLI.MASK[1]=255;CFG.data.CLI.MASK[2]=255;CFG.data.CLI.MASK[3]=0;
  CFG.data.CLI.DNS[0]=8;CFG.data.CLI.DNS[1]=8;CFG.data.CLI.DNS[2]=8;CFG.data.CLI.DNS[3]=8;
}

// Read configuration file and place the values into the CFG struct
void load_CFG() {
  console_log("Load configuration...");
  activity(FLASH);
  File storage = LittleFS.open("/config.bin", "r");
  if (storage) {
    uint32_t nBytes = storage.readBytes((char*)CFG.raw, sizeof(config_data));
    console_log(" (load)...");
  } else {
    console_log(" (failsafe mode)...");
    set_failsafe_CFG();
  }
  storage.close();
  activity(FLASH);
  delay(500);
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