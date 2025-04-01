#include "Sidekick.h"
// Read LED pins

void led_init() {
  #ifdef LED_POWER_PIN
    pinMode(LED_POWER_PIN, INPUT);
    digitalWrite(LED_POWER_PIN, LOW);
  #endif
  #ifdef LED_HDD_PIN
    pinMode(LED_HDD_PIN, INPUT);
    digitalWrite(LED_HDD_PIN, LOW);
  #endif

}

#ifdef LED_POWER_PIN
  bool led_power() {
    return(digitalRead(PIN_POWER_LED) == HIGH);
  }
#endif

#ifdef LED_HDD_PIN
  bool led_hdd() {
    return(digitalRead(PIN_HDD_LED) == HIGH);
  }
#endif

void led_register() {
  #ifdef LED_POWER_PIN
  server.on("/led/power", HTTP_GET, []() {
    server.send(200, "application/json", "{ \"led_power\": \"" + led_power()?"ON":"OFF" + "\"}");
  });
  #endif
  #ifdef LED_HDD_PIN
  server.on("/led/hdd", HTTP_GET, []() {
    server.send(200, "application/json", "{ \"led_hdd\": \"" + led_hdd()?"ON":"OFF" + "\"}");
  });
  #endif
}