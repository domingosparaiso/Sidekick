#include "Sidekick.h"
// Buttons control

void button_init() {
  #ifdef BUTTON_POWER_PIN
    resourcesAddItem("power");
    #if VALUE_BUTTON_POWER == HIGH
      pinMode(BUTTON_POWER_PIN, INPUT_PULLUP);
      Serial.print("Button power: ");
      Serial.println(BUTTON_POWER_PIN);
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
    return(digitalRead(BUTTON_POWER_PIN) == VALUE_BUTTON_POWER);
  }
#endif

#ifdef BUTTON_RESET_PIN
  bool button_reset() {
    return(digitalRead(BUTTON_RESET_PIN) == VALUE_BUTTON_RESET);
  }
#endif

#ifdef BUTTON_RECONFIGURE_PIN
  bool button_reconfigure() {
    return(digitalRead(BUTTON_RECONFIGURE_PIN) == VALUE_BUTTON_RECONFIGURE);
  }
#endif


void button_register() {
  #ifdef BUTTON_POWER_PIN
  server.on("/button/power", HTTP_GET, []() {
    server.send(200, "application/json", "{ \"button_power\": \"" + String(button_power()?"ON":"OFF") + "\"}");
  });
  #endif
  #ifdef BUTTON_RESET_PIN
  server.on("/button/reset", HTTP_GET, []() {
    server.send(200, "application/json", "{ \"button_reset\": \"" + String(button_reset()?"ON":"OFF") + "\"}");
  });
  #endif
  #ifdef BUTTON_RECONFIGURE_PIN
  server.on("/button/reconfigure", HTTP_GET, []() {
    server.send(200, "application/json", "{ \"button_reconfigure\": \"" + String(button_reconfigure()?"ON":"OFF") + "\"}");
  });
  #endif
}

void button_check() {
  #ifdef BUTTON_POWER_PIN
    if(button_power()) {
      #ifdef RELAY_POWER_PIN
        #ifdef LED_POWER_PIN
          if(led_power()) {
            relay_set(RELAY_POWER_PIN, String("POWER_OFF"));
          } else {
            relay_set(RELAY_POWER_PIN, String("POWER_ON"));
          }
        #else
          relay_set(RELAY_POWER_PIN, String("POWER_ON"));
        #endif
      #endif
    }
  #endif
}