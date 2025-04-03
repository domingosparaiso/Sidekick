#ifndef CONFIG_H
  #define CONFIG_H 1
  // --- button.c ------------------------------------------------------------------
  // * Configuração da leitura dos botões do gabinete
  #define BUTTON_POWER_PIN 1
  //#define BUTTON_RESET_PIN 2
  //#define BUTTON_RECONFIGURE_PIN 3
  // * Valor lido quando o botão for pressionado (LOW=PULLUP, hIGH=PULLDOWN)
  #define VALUE_BUTTON_POWER HIGH
  //#define VALUE_BUTTON_RESET HIGH
  //#define VALUE_BUTTON_RECONFIGURE HIGH
  // -------------------------------------------------------------------------------

  // --- led.c ---------------------------------------------------------------------
  //#define LED_POWER_PIN 4
  //#define LED_HDD_PIN 5
  // -------------------------------------------------------------------------------

  // --- display.c -----------------------------------------------------------------
  // * Escolher o tipo de display em uso
  #define DISPLAY_TYPE DISPLAY_NONE             // No display
  //#define DISPLAY_TYPE DISPLAY_SERIAL           // No display, envia para serial
  //#define DISPLAY_TYPE DISPLAY_SSD1306_128X32     // OLED [SSD1306] 128x32
  //#define DISPLAY_TYPE DISPLAY_SSD1306_128X64   // OLED [SSD1306] 128x64
  //#define DISPLAY_TYPE DISPLAY_LCD2004          // Text LCD [LCD2004] 20x4
  //#define DISPLAY_TYPE DISPLAY_LCD1602          // Text LCD [LCD1602] 16x2
  //#define DISPLAY_TYPE DISPLAY_SSH1106          // OLED [SSH1106] 128x64
  //
  // * Portas de comunicação SPI com o display
  //#define DISPLAY_SDA 6
  //#define DISPLAY_SCL 7
  // -------------------------------------------------------------------------------

  // --- relay ---------------------------------------------------------------------
  // Relay power conectado à placa mãe nos pinos do botão power
  #define RELAY_POWER_PIN 2
  #define RELAY_POWER_LEVEL_ON LOW
  // Relay reset  conectado à placa mãe nos pinos do botão reset
  //#define RELAY_RESET_PIN 9
  //#define RELAY_RESET_LEVEL_ON LOW
  
  // Relays SYS# conectados na alimentação dos cooler do sistema
  //#define RELAY_SYS1_PIN 10
  //#define RELAY_SYS1_LEVEL_ON LOW
  //#define RELAY_SYS2_PIN 11
  //#define RELAY_SYS2_LEVEL_ON LOW
  //#define RELAY_SYS3_PIN 12
  //#define RELAY_SYS3_LEVEL_ON LOW
  //#define RELAY_SYS4_PIN 13
  //#define RELAY_SYS4_LEVEL_ON LOW

  // Tempo em ms que o relay fica fechado para ligar o computador (só usado com o power)
  #define TIMEOUT_RELAY_ON 500
  // Tempo em ms que o relay fica fechado para desligar o computador (só usado com o power)
  #define TIMEOUT_RELAY_OFF 5000
  // -------------------------------------------------------------------------------

  // --- rpm -----------------------------------------------------------------------
  // Medida de RPM dos coolers da CPU e do coolers do sistema
  //#define RPM_CPU_PIN 14
  //#define RPM_SYS1_PIN 15
  //#define RPM_SYS2_PIN 16
  //#define RPM_SYS3_PIN 17
  //#define RPM_SYS4_PIN 18
  // -------------------------------------------------------------------------------

  // --- temperature.c -------------------------------------------------------------
  // Sensores de temperatura DS18B20 usando protocolo 1Wire.
  // Apesar de ser possível usar mais de um sensor no mesmo pino, precisaria saber
  // o endereço de cada um para associar com a localização correta, então usamos
  // um único sensor por pino e não temos dúvidas sobre qual sensor usar
  //#define TEMP_CPU_PIN 19
  //#define TEMP_SYS1_PIN 20
  //#define TEMP_SYS2_PIN 21
  //#define TEMP_SYS3_PIN 22
  //#define TEMP_SYS4_PIN 23
  // -------------------------------------------------------------------------------

  // --- uart.c --------------------------------------------------------------------
  // Comunicação serial RS-232 (console)
  //#define UART_TX_PIN 22;
  //#define UART_RX_PIN 23;
  // -------------------------------------------------------------------------------

#endif // <EOF>