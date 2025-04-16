#include "Sidekick.h"
#include "wificonfig.h"
// Relay control
// array [ POWER, SYS#1, SYS#2, SYS#3, SYS#4 ]

bool in_relay_check = false;

#ifdef RELAY_POWER_PIN
  // power relay needs a timeout to turn it OFF again
  long relay_power_timeout = 0;
  #ifdef BUTTON_RESET_PIN
    #ifndef RELAY_RESET_PIN
      long relay_power_off_timeout = 0;
      long relay_power_on_timeout = 0;
    #endif
  #endif
#endif

#ifdef RELAY_RESET_PIN
  // reset relay needs a timeout to turn it OFF again
  long relay_reset_timeout = 0;
#endif

#ifdef RELAY_BACK_PIN
  // timeout to clear backlight
  long relay_backligth_timeout = 0;
#endif

void relay_init() {
  resourcesHeader("Relay");
  #ifdef RELAY_POWER_PIN
    resourcesAddItem("power", RELAY_POWER_PIN);
    pinMode(RELAY_POWER_PIN, OUTPUT);
  #endif
  #ifdef RELAY_RESET_PIN
    resourcesAddItem("reset", RELAY_RESET_PIN);
    pinMode(RELAY_RESET_PIN, OUTPUT);
  #endif
  #ifdef RELAY_SYS1_PIN
    resourcesAddItem("sys1", RELAY_SYS1_PIN);
    pinMode(RELAY_SYS1_PIN, OUTPUT);
  #endif
  #ifdef RELAY_SYS2_PIN
    resourcesAddItem("sys2", RELAY_SYS2_PIN);
    pinMode(RELAY_SYS2_PIN, OUTPUT);
  #endif
  #ifdef RELAY_SYS3_PIN
    resourcesAddItem("sys3", RELAY_SYS3_PIN);
    pinMode(RELAY_SYS3_PIN, OUTPUT);
  #endif
  #ifdef RELAY_SYS4_PIN
    resourcesAddItem("sys4", RELAY_SYS4_PIN);
    pinMode(RELAY_SYS4_PIN, OUTPUT);
  #endif
  #ifdef RELAY_BACK_PIN
    resourcesAddItem("backlight", RELAY_BACK_PIN);
    pinMode(RELAY_BACK_PIN, OUTPUT);
  #endif
  resourcesAddArray("relay");
}

void relay_check() {
  if(!in_relay_check) {
    in_relay_check = true;
    long now = millis();
    #ifdef RELAY_POWER_PIN
      if(relay_power_timeout > 0 && relay_power_timeout < now) {
        relay_power_timeout = 0;
        digitalWrite(RELAY_POWER_PIN, (RELAY_POWER_LEVEL_ON==HIGH)?LOW:HIGH);
      }
      #ifdef BUTTON_RESET_PIN
        #ifndef RELAY_RESET_PIN
          if(relay_power_off_timeout > 0 && relay_power_off_timeout < now) {
            relay_power_off_timeout = 0;
            digitalWrite(RELAY_POWER_PIN, (RELAY_POWER_LEVEL_ON==HIGH)?LOW:HIGH);
            relay_power_on_timeout = now + TIMEOUT_RESET_INTERVAL;
          }
          if(relay_power_on_timeout > 0 && relay_power_on_timeout < now) {
            relay_power_on_timeout = 0;
            digitalWrite(RELAY_POWER_PIN, RELAY_POWER_LEVEL_ON);
            relay_power_timeout = now + TIMEOUT_RELAY_ON;
          }
        #endif
      #endif
    #endif
    #ifdef RELAY_RESET_PIN
      if(relay_reset_timeout > 0 && relay_reset_timeout < now) {
        relay_reset_timeout = 0;
        digitalWrite(RELAY_RESET_PIN, (RELAY_RESET_LEVEL_ON==HIGH)?LOW:HIGH);
      }
    #endif
    #ifdef RELAY_BACK_PIN
      // turn off backligth on timeout
      if(relay_backligth_timeout > 0 && relay_backligth_timeout < now) {
        relay_backligth_timeout = 0;
        digitalWrite(RELAY_BACK_PIN, (RELAY_BACK_LEVEL_ON==HIGH)?LOW:HIGH);
      }
    #endif
    in_relay_check = false;
  }
}

int cmdString2Int(int relay_port, String cmd) {
  if(cmd == "OFF") return(RELAY_OFF);
  if(cmd == "ON") return(RELAY_ON);
  #ifdef RELAY_POWER_PIN
    if(relay_port == RELAY_POWER_PIN) {
      if(cmd == "POWER_OFF") return(RELAY_POWER_OFF);
      if(cmd == "POWER_ON") return(RELAY_POWER_ON);
      #ifdef BUTTON_RESET_PIN
        #ifndef RELAY_RESET_PIN
          if(cmd == "POWER_OFF_ON") return(RELAY_POWER_OFF_ON);
        #endif
      #endif
    }
  #endif
  #ifdef RELAY_RESET_PIN
    if(relay_port == RELAY_RESET_PIN) {
      if(cmd == "RESET") return(RELAY_RESET);
    }
  #endif
  return(ERROR_VALUE);
}

void relay_set(int relay_port, int value) {
  int level_on;
  int level_off;
  String result = "Error";

  if(value != ERROR_VALUE) {
    switch(relay_port) {
      #ifdef RELAY_POWER_PIN
        case RELAY_POWER_PIN:
          level_on = RELAY_POWER_LEVEL_ON;
          break;
      #endif
      #ifdef RELAY_RESET_PIN
        case RELAY_RESET_PIN:
          level_on = RELAY_RESET_LEVEL_ON;
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
      #ifdef RELAY_BACK_PIN
        case RELAY_BACK_PIN:
          level_on = RELAY_BACK_LEVEL_ON;
          break;
      #endif
      default:
        level_on = HIGH;
        break;
    }
    level_off = (level_on == HIGH)?LOW:HIGH;
    switch(value) {
      case RELAY_OFF:
        digitalWrite(relay_port, level_off);
        break;
      case RELAY_ON:
        digitalWrite(relay_port, level_on);
        #ifdef RELAY_BACK_PIN
          if(relay_port == RELAY_BACK_PIN) relay_backligth_timeout = millis() + BACKLIGHT_TIMEOUT;
        #endif
        break;
      case RELAY_POWER_OFF:
        digitalWrite(relay_port, level_on);
        relay_power_timeout = millis() + TIMEOUT_RELAY_OFF;
        break;
      case RELAY_POWER_ON:
        digitalWrite(relay_port, level_on);
        relay_power_timeout = millis() + TIMEOUT_RELAY_ON;
        result = String(relay_port) + ":" + String(level_on) + ":" + String(relay_power_timeout);
        break;
      #ifdef BUTTON_RESET_PIN
        #ifndef RELAY_RESET_PIN
          case RELAY_POWER_OFF_ON:
            digitalWrite(relay_port, level_on);
            relay_power_off_timeout = millis() + TIMEOUT_RELAY_OFF;
            break;
        #endif
      #endif
      #ifdef RELAY_RESET_PIN
        case RELAY_RESET:
          digitalWrite(RELAY_RESET_PIN, RELAY_RESET_LEVEL_ON);
          relay_reset_timeout = millis() + TIMEOUT_RESET;
          break;
      #endif
    }
    result = "OK";
  }
  send_result_json(result);
}

void relay_register() {
  #ifdef RELAY_POWER_PIN
    server.on("/relay/power", HTTP_GET, []() { relay_set(RELAY_POWER_PIN, cmdString2Int(RELAY_POWER_PIN, server.arg("cmd"))); });
  #endif
  #ifdef RELAY_RESET_PIN
    server.on("/relay/reset", HTTP_GET, []() { relay_set(RELAY_RESET_PIN, cmdString2Int(RELAY_RESET_PIN, server.arg("cmd"))); });
  #endif
  #ifdef RELAY_SYS1_PIN
    server.on("/relay/sys1", HTTP_GET, []() { relay_set(RELAY_SYS1_PIN, cmdString2Int(RELAY_SYS1_PIN, server.arg("cmd"))); });
  #endif
  #ifdef RELAY_SYS2_PIN
    server.on("/relay/sys2", HTTP_GET, []() { relay_set(RELAY_SYS2_PIN, cmdString2Int(RELAY_SYS2_PIN, server.arg("cmd"))); });
  #endif
  #ifdef RELAY_SYS3_PIN
    server.on("/relay/sys3", HTTP_GET, []() { relay_set(RELAY_SYS3_PIN, cmdString2Int(RELAY_SYS3_PIN, server.arg("cmd"))); });
  #endif
  #ifdef RELAY_SYS4_PIN
    server.on("/relay/sys4", HTTP_GET, []() { relay_set(RELAY_SYS4_PIN, cmdString2Int(RELAY_SYS4_PIN, server.arg("cmd"))); });
  #endif
  server.on("/relay/LOW", HTTP_GET, []() { digitalWrite(0,LOW); server.send(200, "plain/text", "LOW"); });
  server.on("/relay/HIGH", HTTP_GET, []() { digitalWrite(0,HIGH); server.send(200, "plain/text", "HIGH"); });
  #ifdef RELAY_BACK_PIN
    server.on("/backlight/off", HTTP_GET, []() { relay_set(RELAY_BACK_PIN, RELAY_OFF); });
    server.on("/backlight/on", HTTP_GET, []() { relay_set(RELAY_BACK_PIN, RELAY_ON); });
  #endif
}

