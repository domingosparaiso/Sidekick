#include "display.h"
#include "config.h"
#include "Sidekick.h"
#include "storage.h"
// Included here (instead of only in the files that use it) so that Arduino's
// auto-generated function prototypes -- inserted at the top of the merged sketch --
// see the SidekickRequest/SidekickHTTPMethod/WebServer types before they are
// referenced (e.g. by authOn() in cookie.ino).
#include "wificonfig.h"

bool block_loop = false;
String serialNumber = "";
String resourcesJson = "";
String resourcesList = "";

// Change status LED (usually onboard LED)
void activity(int mode) {
  #ifdef LED_PIN
    switch(mode) {
      case INIT:
        pinMode(LED_PIN, OUTPUT);
        break;
      case ON:
        // Turn LED ON
        digitalWrite(LED_PIN, LED_LEVEL_ON);
        break;
      case OFF:
        // Turn LED OFF
        digitalWrite(LED_PIN, LED_LEVEL_OFF);
        break;
      case FLASH:
        // Change LED (ON/OFF)
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        break;
    }
  #endif
}

void setup() {
  #ifdef ESP32
    long unsigned int espmac = ESP.getEfuseMac() >> 24;  
    serialNumber = String(espmac, HEX);
  #endif
  #ifdef ESP8266
    serialNumber = String(ESP.getChipId(), HEX);
  #endif
  serialNumber.toUpperCase();

  activity(INIT);
  delay(2000);

  // [display.ino] initialize serial over USB connection to display console messages
  console_init();
  delay(500);  

  // Display init messages
  console_log("\nPower ON\nBoard: ");
  console_log(String(ARDUINO_BOARD));
  console_log("\nCPU Frequency: ");
  console_log(String(F_CPU / 1000000));
  console_log(" MHz\nSerial Number: ");
  console_log(String(serialNumber));
  console_log("\n\nSidekick ");
  console_log(String(VERSION));
  console_log("\n");

  // Initialize resource list
  resourcesJson = String(   "{\"board\": \"")        + String(ARDUINO_BOARD)
                + String("\", \"mhz\":\"")           + String(F_CPU / 1000000)
                + String("\", \"display\":\"")       + String(DISPLAY_NAME)
                + String("\", \"version\": \"")      + String(VERSION)
                + String("\", \"serialNumber\": \"") + String(serialNumber)
                + String("\", \"serialconsole\": \"")
#ifdef HOST_SERIAL
                + "yes (" + String(HOST_BAUD) + ")\"";
#else
                + "no\"";
#endif

  // [data.ino] Initialize SPIFFS data.ino
  storage_init();
  
  // [display.ino] Initialize primary display
  display_init();
  
  // [data.ino] Load storage configuration
  load_CFG();

  // [led.ino] Initialize read LED configuration
  led_init();

  // [button.ino] Initialize button configuration
  button_init();
  
  // [relay.ino] Initialize relay configuration
  relay_init();

  // [rpm.ino] Initialize RPM configuration
  rpm_init();

  // [temperature.ino] Initialize temperature configuration
  temperature_init();

  // [console.ino] Initialize the HOST_SERIAL<->WebSocket console bridge (ESP32 only)
  hostconsole_init();

  // end of resource list
  resourcesJson += "}";

  // [wifi.ino] Try to connect with Wifi using CLI mode, use AP mode when fail or reconfigure mode
  connect_wifi();

  // Initialize date/time using NTP server
  datetime_init();

  // [server.ino] Call webserver setup function
  server_setup();

  // Ready to use
  console_log("System ready!\n");
}

void loop() {
  if(!block_loop) {
    // [server.ino] chain to server loop functions
    server_loop();
    // [wifi.ino] check if wifi is on, and try reconnect when lost connection
    check_wifi();
  }
}