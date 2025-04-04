#include "Sidekick.h"
// Buttons control

bool in_button_check = false;
bool last_status_button_power = false;
bool last_status_button_reset = false;
bool last_status_button_reconfigure = false;

void button_init() {
  #ifdef BUTTON_POWER_PIN
    resourcesAddItem("power");
    Serial.print("Button power PIN: ");
    Serial.println(BUTTON_POWER_PIN);
    #if VALUE_BUTTON_POWER == HIGH
      pinMode(BUTTON_POWER_PIN, INPUT_PULLUP);
    #endif
    #if VALUE_BUTTON_POWER == LOW
      pinMode(BUTTON_POWER_PIN, INPUT_PULLDOWN);
    #endif
    #ifndef VALUE_BUTTON_POWER
      pinMode(BUTTON_POWER_PIN, INPUT);
      #define VALUE_BUTTON_POWER HIGH
    #endif
  #endif
  #ifdef BUTTON_RESET_PIN
    resourcesAddItem("reset");
    Serial.print("Button reset PIN: ");
    Serial.println(BUTTON_POWER_PIN);
    #if VALUE_BUTTON_RESET == HIGH
      pinMode(BUTTON_RESET_PIN, INPUT_PULLUP);
    #endif
    #if VALUE_BUTTON_RESET == LOW
      pinMode(BUTTON_RESET_PIN, INPUT_PULLDOWN);
    #endif
    #ifndef VALUE_BUTTON_RESET
      pinMode(BUTTON_RESET_PIN, INPUT);
      #define VALUE_BUTTON_RESET HIGH
    #endif
  #endif
  #ifdef BUTTON_RECONFIGURE_PIN
    resourcesAddItem("reconfigure");
    Serial.print("Button reconfigure PIN: ");
    Serial.println(BUTTON_POWER_PIN);
    #if VALUE_BUTTON_RECONFIGURE == HIGH
      pinMode(BUTTON_RECONFIGURE_PIN, INPUT_PULLUP);
    #endif
    #if VALUE_BUTTON_RECONFIGURE == LOW
      pinMode(BUTTON_RECONFIGURE_PIN, INPUT_PULLDOWN);
    #endif
    #ifndef VALUE_BUTTON_RECONFIGURE
      pinMode(BUTTON_RECONFIGURE_PIN, INPUT);
      #define VALUE_BUTTON_RECONFIGURE HIGH
    #endif
  #endif  
  resourcesAddArray("button");
}

#ifdef BUTTON_POWER_PIN
  bool button_power() {    
    if(digitalRead(BUTTON_POWER_PIN) == VALUE_BUTTON_POWER) {
      if(!last_status_button_power) {
        last_status_button_power = true;
        return true;
      }
    } else {
      last_status_button_power = false;
    }
    return false;
  }
#endif

#ifdef BUTTON_RESET_PIN
  bool button_reset() {
    if(digitalRead(BUTTON_RESET_PIN) == VALUE_BUTTON_RESET) {
      if(!last_status_button_reset) {
        last_status_button_reset = true;
        return true;
      }
    } else {
      last_status_button_reset = false;
    }
    return false;
  }
#endif

#ifdef BUTTON_RECONFIGURE_PIN
  bool button_reconfigure() {
    if(digitalRead(BUTTON_RECONFIGURE_PIN) == VALUE_BUTTON_RECONFIGURE) {
      if(!last_status_button_reconfigure) {
        last_status_button_reconfigure = true;
        return true;
      }
    } else {
      last_status_button_reconfigure = false;
    }
    return false;
  }
#endif

void button_register() {
  #ifdef BUTTON_POWER_PIN
    server.on("/button/power", HTTP_GET, []() { button_power_action(); send_result_json("OK"); });
    server.on("/button/get/power", HTTP_GET, []() { server.send(200, "application/json", "{ \"button_power\": \"" + String(button_power()?"ON":"OFF") + "\"}"); });
  #endif
  #ifdef BUTTON_RESET_PIN
    server.on("/button/reset", HTTP_GET, []() { button_reset_action(); send_result_json("OK"); });
    server.on("/button/get/reset", HTTP_GET, []() { server.send(200, "application/json", "{ \"button_reset\": \"" + String(button_reset()?"ON":"OFF") + "\"}"); });
  #endif
  #ifdef BUTTON_RECONFIGURE_PIN
    server.on("/button/reconfigure", HTTP_GET, []() { button_reconfigure_action(); send_result_json("OK"); });
    server.on("/button/get/reconfigure", HTTP_GET, []() { server.send(200, "application/json", "{ \"button_reconfigure\": \"" + String(button_reconfigure()?"ON":"OFF") + "\"}"); });
  #endif
}

void button_power_action() {
  Serial.println("<Botao POWER>");
  #ifdef RELAY_POWER_PIN
    #ifdef LED_POWER_PIN
      if(led_power()) {
        relay_set(RELAY_POWER_PIN, RELAY_POWER_OFF);
      } else {
        relay_set(RELAY_POWER_PIN, RELAY_POWER_ON);
      }
    #else
      relay_set(RELAY_POWER_PIN, RELAY_POWER_ON);
    #endif
  #endif
 }

void button_reset_action() {
  Serial.println("<Botao RESET>");
  #ifdef RELAY_RESET_PIN
    relay_set(RELAY_RESET_PIN, RELAY_RESET);
  #else
    #ifdef RELAY_POWER_PIN
      relay_set(RELAY_POWER_PIN, RELAY_POWER_OFF_ON);
    #endif
  #endif
}

void button_reconfigure_action() {
  Serial.println("<Botao RECONFIGURE>");
}

void button_check() {
  if(!in_button_check) {
    in_button_check = true;
    #ifdef BUTTON_POWER_PIN
      if(button_power()) button_power_action();
    #endif
    #ifdef BUTTON_RESET_PIN
      if(button_reset()) button_reset_action();
    #endif
    #ifdef BUTTON_RECONFIGURE_PIN
      if(button_reset()) button_reconfigure_action();
    #endif
    in_button_check = false;
  }
}