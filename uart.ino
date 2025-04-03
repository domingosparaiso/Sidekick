#include "Sidekick.h"
// Serial UART communication

void uart_init() {
  #ifdef UART_TX_PIN
  #ifdef UART_RX_PIN
  resourcesAddValue(String("uart"), String("serial")); 
  #endif
  #endif
}