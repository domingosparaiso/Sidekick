#include "Sidekick.h"
// Serial UART communication

void uart_init() {
  #ifdef UART_TX_PIN
  #ifdef UART_RX_PIN
  resourcesAddValue(String("uart"), String("serial"));
  console_log("UART Init: TX=" + String(UART_TX_PIN) + " RX=" + String(UART_RX_PIN) + " ... [OK]\n");
  #endif
  #endif
}