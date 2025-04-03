#include "Sidekick.h"
// Temperature DS18B20 sensors
// array [ CPU, SYS#1, SYS#2, SYS#3, SYS#4 ]

void temperature_init() {
  #ifdef TEMP_CPU_PIN
    resourcesAddItem(String("cpu"));
  #endif
  #ifdef TEMP_SYS1_PIN
    resourcesAddItem(String("sys1"));
  #endif
  #ifdef TEMP_SYS2_PIN
    resourcesAddItem(String("sys2"));
  #endif
  #ifdef TEMP_SYS3_PIN
    resourcesAddItem(String("sys3"));
  #endif
  #ifdef TEMP_SYS4_PIN
    resourcesAddItem(String("sys4"));
  #endif
  resourcesAddArray(String("temperature"));
}

void temperature_register() {
#ifdef TEMP_CPU_PIN
  // server.on("/temperature/cpu", HTTP_GET, []() { server.send(200, "application/json", "{ \"temperature_cpu\": \"" + String(get_temperature(TEMP_CPU_PIN)) + "\"}"); });
#endif
}