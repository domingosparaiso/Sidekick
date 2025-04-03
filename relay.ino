#include "Sidekick.h"
// Relay control
// array [ POWER, SYS#1, SYS#2, SYS#3, SYS#4 ]

#ifdef RELAY_POWER_PIN
  // relay power precisa de timeout para saber quando desligar nos pulson ON/OFF
  long relay_power_timeout = 0;
#endif

#ifdef RELAY_POWER_PIN
  // relay reset precisa de timeout para saber quando desligar depois do reset
  long relay_reset_timeout = 0;
#endif

void relay_init() {
  #ifdef RELAY_POWER_PIN
    resourcesAddItem(String("power"));
    pinMode(RELAY_POWER_PIN, OUTPUT);
    Serial.print("Relay power: ");
    Serial.println(RELAY_POWER_PIN);
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
      digitalWrite(RELAY_POWER_PIN, (RELAY_POWER_LEVEL_ON==HIGH)?LOW:HIGH);
    }
  #endif
  #ifdef RELAY_RESET_PIN
    if(relay_reset_timeout > 0 && relay_reset_timeout > millis()) {
      relay_reset_timeout = 0;
      digitalWrite(RELAY_RESET_PIN, (RELAY_RESET_LEVEL_ON==HIGH)?LOW:HIGH);
    }
  #endif
}

int cmdString2Int(String cmd) {
  if(cmd == "OFF") return(RELAY_OFF);
  if(cmd == "ON") return(RELAY_ON);
  #ifdef RELAY_POWER_PIN
    if(relay_port == RELAY_POWER_PIN) {
      if(cmd == "POWER_OFF") return(RELAY_POWER_OFF);
      if(cmd == "POWER_ON") return(RELAY_POWER_ON);
    }
  #endif
  #ifdef RELAY_RESET_PIN
    if(relay_port == RELAY_RESET_PIN) {
      if(cmd == "RESET") return(RELAY_RESET);
    }
  #endif
  return(ERROR_VALUE);
}

void relay_set(int relay_port, String cmd) {
  int value = cmd2String(cmd)
  int level_on;
  int level_off;

  switch(relay_port) {
    #ifdef RELAY_POWER_PIN
      case RELAY_POWER_PIN:
        level_on = RELAY_POWER_LEVEL_ON;
        break;
    #endif
    #ifdef RELAY_SYS1_PIN
      case RELAY_SYS1_PIN:
        level_on = RELAY_SYS1_LEVEL_ON;
        break;
    #endif
    #ifdef RELAY_SYS2_PIN
      case RELAY_SYS2_PIN:
        level_on = RELAY_SYS2_LEVEL_ON;
        break;
    #endif
    #ifdef RELAY_SYS3_PIN
      case RELAY_SYS3_PIN:
        level_on = RELAY_SYS3_LEVEL_ON;
        break;
    #endif
    #ifdef RELAY_SYS4_PIN
      case RELAY_SYS4_PIN:
        level_on = RELAY_SYS4_LEVEL_ON;
        break;
    #endif
    default:
      level_on = HIGH;
      break;
  }
  level_off = (level_on == HIGH)?LOW:HIGH;
  if(value >= 0) {
    switch(value) {
      case RELAY_OFF:
        digitalWrite(relay_port, level_off);
        break;
      case RELAY_ON:
        digitalWrite(relay_port, level_on);
        break;
      #ifdef RELAY_POWER_PIN
        case RELAY_POWER_OFF:
          digitalWrite(RELAY_POWER_PIN, level_on);
          relay_power_timeout = millis() + TIMEOUT_RELAY_OFF;
          break;
        case RELAY_POWER_ON:
          digitalWrite(RELAY_POWER_PIN, level_on);
          relay_power_timeout = millis() + TIMEOUT_RELAY_ON;
          break;
      #endif
      #ifdef RELAY_RESET_PIN
        case RELAY_RESET:
          digitalWrite(RELAY_RESET_PIN, level_on);
          relay_reset_timeout = millis() + TIMEOUT_RESET;
          break;
      #endif
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

