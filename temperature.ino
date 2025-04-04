#include "Sidekick.h"
// Temperature DS18B20 sensors
// array [ CPU, SYS#1, SYS#2, SYS#3, SYS#4 ]

void temperature_init() {
  console_log("Temperature init: ");
  #ifdef TEMP_CPU_PIN
    resourcesAddItem("cpu", TEMP_CPU_PIN);
  #endif
  #ifdef TEMP_SYS1_PIN
    resourcesAddItem("sys1", TEMP_SYS1_PIN);
  #endif
  #ifdef TEMP_SYS2_PIN
    resourcesAddItem("sys2", TEMP_SYS2_PIN);
  #endif
  #ifdef TEMP_SYS3_PIN
    resourcesAddItem("sys3", TEMP_SYS3_PIN);
  #endif
  #ifdef TEMP_SYS4_PIN
    resourcesAddItem("sys4", TEMP_SYS4_PIN);
  #endif
  resourcesAddArray("temperature");
}

void temperature_register() {
#ifdef TEMP_CPU_PIN
  // server.on("/temperature/cpu", HTTP_GET, []() { server.send(200, "application/json", "{ \"temperature_cpu\": \"" + String(get_temperature(TEMP_CPU_PIN)) + "\"}"); });
#endif
}