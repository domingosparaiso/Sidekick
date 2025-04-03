#include "Sidekick.h"
// Relay control
// array [ POWER, SYS#1, SYS#2, SYS#3, SYS#4 ]

#ifdef RELAY_POWER_PIN
// relay power precisa de timeout para saber quando desligar nos pulson ON/OFF
long relay_power_timeout = 0;
#endif

void relay_init() {
  #ifdef RELAY_POWER_PIN
    resourcesAddItem(String("power"));
    pinMode(RELAY_POWER_PIN, OUTPUT);
  #endif
  #ifdef RELAY_RESET_PIN
    resourcesAddItem(String("reset"));
    pinMode(RELAY_RESET_PIN, OUTPUT);
  #endif
  #ifdef RELAY_SYS1_PIN
    resourcesAddItem(String("sys1"));
    pinMode(RELAY_SYS1_PIN, OUTPUT);
  #endif
  #ifdef RELAY_SYS2_PIN
    resourcesAddItem(String("sys2"));
    pinMode(RELAY_SYS2_PIN, OUTPUT);
  #endif
  #ifdef RELAY_SYS3_PIN
    resourcesAddItem(String("sys3"));
    pinMode(RELAY_SYS3_PIN, OUTPUT);
  #endif
  #ifdef RELAY_SYS4_PIN
    resourcesAddItem(String("sys4"));
    pinMode(RELAY_SYS4_PIN, OUTPUT);
  #endif
  resourcesAddArray(String("relay"));
}

void relay_check() {
  #ifdef RELAY_POWER_PIN
    if(relay_power_timeout > 0 && relay_power_timeout > millis()) {
      relay_power_timeout = 0;
      digitalWrite(RELAY_POWER_PIN, RELAY_POWER_LEVEL_ON);
    }
  #endif
}

void relay_set(int relay_port, String cmd) {
  int value = -1;
  if(cmd == "OFF") value = RELAY_OFF;
  if(cmd == "ON") value = RELAY_ON;
  if(relay_port == RELAY_POWER_PIN) {
    if(cmd == "POWER_OFF") value = RELAY_POWER_OFF;
    if(cmd == "POWER_ON") value = RELAY_POWER_ON;
  }
  if(value >= 0) {
    switch(value) {
      case RELAY_OFF:
        digitalWrite(relay_port, RELAY_POWER_LEVEL_OFF);
        break;
      case RELAY_ON:
        digitalWrite(relay_port, RELAY_POWER_LEVEL_ON);
        break;
      case RELAY_POWER_OFF:
        digitalWrite(relay_port, RELAY_POWER_LEVEL_ON);
        relay_power_timeout = millis() + TIMEOUT_RELAY_OFF;
        break;
      case RELAY_POWER_ON:
        digitalWrite(relay_port, RELAY_POWER_LEVEL_ON);
        relay_power_timeout = millis() + TIMEOUT_RELAY_ON;
        break;
    }
    send_result_json("OK");
  } else {
    send_result_json("Error");
  }
}

void relay_register() {
  #ifdef RELAY_POWER_PIN
    server.on("/relay/power", HTTP_GET, []() { relay_set(RELAY_POWER_PIN, server.arg("cmd")); });
  #endif
  #ifdef RELAY_RESET_PIN
    server.on("/relay/reset", HTTP_GET, []() { relay_set(RELAY_RESET_PIN, server.arg("cmd")); });
  #endif
  #ifdef RELAY_SYS1_PIN
    server.on("/relay/sys1", HTTP_GET, []() { relay_set(RELAY_SYS1_PIN, server.arg("cmd")); });
  #endif
  #ifdef RELAY_SYS2_PIN
    server.on("/relay/sys2", HTTP_GET, []() { relay_set(RELAY_SYS2_PIN, server.arg("cmd")); });
  #endif
  #ifdef RELAY_SYS3_PIN
    server.on("/relay/sys3", HTTP_GET, []() { relay_set(RELAY_SYS3_PIN, server.arg("cmd")); });
  #endif
  #ifdef RELAY_SYS4_PIN
    server.on("/relay/sys4", HTTP_GET, []() { relay_set(RELAY_SYS4_PIN, server.arg("cmd")); });
  #endif 
}

