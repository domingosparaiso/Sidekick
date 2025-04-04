#include "Sidekick.h"
// OLED communication

bool console_ok = false;

void display_init() {
  console_log("Init display device... [OK]\n");
  delay(1000);
  activity(FLASH);
}

void display_print(int linha, int coluna, String msg) {
  #if DISPLAY_TYPE == DISPLAY_SERIAL
    Serial.println(msg);
  #endif
}

void display_status(int status) {
  #if DISPLAY_TYPE == DISPLAY_SERIAL
    String msg[] = {STATUS_LIST};
    Serial.print("Status: ");
    Serial.println(msg[status]);
  #endif
}

bool console_init() {
  long timeout = millis() + TIMEOUT_CONSOLE;
  Serial.begin(CONSOLE_BAUD);
  while (!Serial && millis() < timeout);
  console_ok = (!Serial)?false:true;
  return(console_ok);
}

void console_log(String msg) {
  activity(FLASH);
  if(console_ok) Serial.print(msg);
}