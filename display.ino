#include "Sidekick.h"
#include "wificonfig.h"
// OLED communication

bool console_ok = false;

void display_init() {
  #ifdef RELAY_BACK_PIN
    relay_set(RELAY_BACK_PIN, RELAY_ON);
  #endif
  console_log("Init display device... [OK]\n");
  delay(1000);
  activity(FLASH);
}

void display_print(int linha, int coluna, String msg) {
  #if DISPLAY_TYPE == DISPLAY_SERIAL
    Serial.println(msg);
  #endif
  #ifdef RELAY_BACK_PIN
    relay_set(RELAY_BACK_PIN, RELAY_ON);
  #endif
}

void display_status(int status) {
  #if DISPLAY_TYPE == DISPLAY_SERIAL
    String msg[] = {STATUS_LIST};
    Serial.print("Status: ");
    Serial.println(msg[status]);
  #endif
  #ifdef RELAY_BACK_PIN
    relay_set(RELAY_BACK_PIN, RELAY_ON);
  #endif
}

bool console_init() {
  console_ok = false;
  #ifdef CONSOLE_SERIAL
    long timeout = millis() + TIMEOUT_CONSOLE;
    CONSOLE_SERIAL.begin(CONSOLE_BAUD);
    while (!CONSOLE_SERIAL && millis() < timeout);
    console_ok = (!CONSOLE_SERIAL)?false:true;
  #endif
  return(console_ok);
}

void console_log(String msg) {
  activity(FLASH);
  #ifdef CONSOLE_SERIAL
    if(console_ok) CONSOLE_SERIAL.print(msg);
  #endif
}