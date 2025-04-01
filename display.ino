#include "Sidekick.h"
// OLED communication

void display_init() {
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