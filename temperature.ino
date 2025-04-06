#include "Sidekick.h"
#include <DS18B20.h>

#ifdef TEMP_WIRE_PIN
  DS18B20 ds(TEMP_WIRE_PIN);
#endif
// Temperature DS18B20 sensors
// array [ CPU, SYS#1, SYS#2, SYS#3, SYS#4 ]

uint8_t temperature_address[5][8];
int temperature_map[5] = {0,0,0,0,0};

void temperature_init() {
  int devcount = 0;
  #ifdef TEMP_WIRE_PIN
    devcount = ds.getNumberOfDevices();
  #endif
  console_log("Temperature sensors: ");
  console_log(String(devcount));
  console_log(" devices\n");
  console_log("Temperature init: ");
  #ifdef TEMP_WIRE_PIN
    devcount = 0;
    while (ds.selectNext() && devcount < 5) {
      ds.getAddress(temperature_address[devcount]);
      console_log("[");
      console_log(String(devcount+1));
      console_log("] <");
      for (uint8_t i = 0; i < 8; i++) {
        console_log(((i==0)?"":".") + String(temperature_address[devcount][i]));
      }
      console_log("> ");
      String temp_model = "";
      switch (ds.getFamilyCode()) {
        case MODEL_DS18S20:
          temp_model = "DS18S20/DS1820";
          break;
        case MODEL_DS1822:
          temp_model = "DS1822";
          break;
        case MODEL_DS18B20:
          temp_model = "DS18B20";
          break;
        default:
          temp_model = "Unrecognized Device";
          break;
      }
      devcount++;
      console_log(temp_model + String("\n"));
      resourcesAddItem(temp_model, devcount);
    }
  #endif
  resourcesAddArray("temperature");
}

#ifdef TEMP_WIRE_PIN
  void temperature_config(int pos, int map) {
    if(map >=1 && map <=5 && pos >= 1 && pos <=5) temperature_map[map] = pos;
  }

  String temperature_get(String location) {
    String result = "-";
      int pos = location.toInt();
      int map = 0;
      if(pos >= 1 && pos <= 5) {
        map = temperature_map[pos-1];
        if(map > 0) {
          ds.select(temperature_address[temperature_map[map-1]]);
          result = String(ds.getTempC());
        }
      }
    return(result);
  }
#endif

void temperature_register() {
  #ifdef TEMP_WIRE_PIN
    server.on("/temperature", HTTP_GET, []() { 
      String location = server.arg("location")
      server.send(200, "application/json", "{ \"temperature\": \"" + String(temperature_get(location)) + "\"}");
    });
    server.on("/temperature/map", HTTP_GET, []() {
      String devs[5] = { "cpu", "sys1", "sys2", "sys3", "sys4" };
      String pos = server.arg("pos");
      String map = server.arg("map");
      int int_map = 0;
      int int_pos = pos.toInt();
      for(int i=0; i<5; i++) {
        if(map == devs[i]) int_map = i+1;
      }
      if(int_map >= 1 && pos >=1 && pos <= 5) {
        temperature_config(pos.toInt(), int_map);
        send_result_json("OK");
      } else {
        send_result_json("Error");
      }
    });
  #endif
}