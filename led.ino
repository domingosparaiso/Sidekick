#include "Sidekick.h"
// Read LED pins

void led_init() {
  #ifdef LED_POWER_PIN
    resourcesAddItem(String("power"));
    pinMode(LED_POWER_PIN, INPUT);
    digitalWrite(LED_POWER_PIN, LOW);
  #endif
  #ifdef LED_HDD_PIN
  resourcesAddItem(String("hdd"));
    pinMode(LED_HDD_PIN, INPUT);
    digitalWrite(LED_HDD_PIN, LOW);
  #endif
  resourcesAddArray(String("led"));
}

#ifdef LED_POWER_PIN
  bool led_power() {
    return(digitalRead(LED_POWER_PIN) == HIGH);
  }
#endif

#ifdef LED_HDD_PIN
  bool led_hdd() {
    return(digitalRead(LED_HDD_PIN) == HIGH);
  }
#endif

void led_register() {
  #ifdef LED_POWER_PIN
  server.on("/led/power", HTTP_GET, []() {
    server.send(200, "application/json", "{ \"led_power\": \"" + String(led_power()?"ON":"OFF") + "\"}");
  });
  #endif
  #ifdef LED_HDD_PIN
  server.on("/led/hdd", HTTP_GET, []() {
    server.send(200, "application/json", "{ \"led_hdd\": \"" + String(led_hdd()?"ON":"OFF") + "\"}");
  });
  #endif
}