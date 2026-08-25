#include "Sidekick.h"
#include "wificonfig.h"
#include "cookie.h"
// Read LED pins from PC motherboard, used to check if the PC is on and if HDD still in use

// Push led status to the web UI over a WebSocket, only when a value actually changes,
// instead of the browser polling /led/power and /led/hdd every second.
// Needs a WebSocket, so it only exists on ESP32 (ESPAsyncWebServer); ESP8266 builds
// keep the synchronous ESP8266WebServer, which has no WebSocket support. The /led/*
// HTTP endpoints below still work on ESP8266, they're just not pushed automatically.
#ifdef ESP32
AsyncWebSocket ledSocket("/led-ws");
#endif

// Setup LED pins
void led_init() {
  resourcesHeader("LED");
  #ifdef LED_POWER_PIN
    resourcesAddItem("power", LED_POWER_PIN);
    pinMode(LED_POWER_PIN, INPUT);
    digitalWrite(LED_POWER_PIN, LOW);
  #endif
  #ifdef LED_HDD_PIN
  resourcesAddItem("hdd", LED_HDD_PIN);
    pinMode(LED_HDD_PIN, INPUT);
    digitalWrite(LED_HDD_PIN, LOW);
  #endif
  resourcesAddArray("led");
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
  authOn("/led/power", HTTP_GET, [](SidekickRequest request) {
    req_send(request, 200, "application/json", "{ \"led_power\": \"" + String(led_power()?"ON":"OFF") + "\"}");
  });
  #endif
  #ifdef LED_HDD_PIN
  authOn("/led/hdd", HTTP_GET, [](SidekickRequest request) {
    req_send(request, 200, "application/json", "{ \"led_hdd\": \"" + String(led_hdd()?"ON":"OFF") + "\"}");
  });
  #endif
  #ifdef ESP32
    server.addHandler(&ledSocket);
  #endif
}

// Poll the LED pins and broadcast to connected clients only when a value flips,
// called from server_loop() (ESP32 only, see comment at the top of this file)
void led_loop() {
  #ifdef ESP32
    #ifdef LED_POWER_PIN
      static bool firstPower = true;
      static bool lastPower = false;
      bool power = led_power();
      if(firstPower || power != lastPower) {
        ledSocket.textAll("{ \"led_power\": \"" + String(power?"ON":"OFF") + "\"}");
        lastPower = power;
        firstPower = false;
      }
    #endif
    #ifdef LED_HDD_PIN
      static bool firstHdd = true;
      static bool lastHdd = false;
      bool hdd = led_hdd();
      if(firstHdd || hdd != lastHdd) {
        ledSocket.textAll("{ \"led_hdd\": \"" + String(hdd?"ON":"OFF") + "\"}");
        lastHdd = hdd;
        firstHdd = false;
      }
    #endif
    ledSocket.cleanupClients();
  #endif
}