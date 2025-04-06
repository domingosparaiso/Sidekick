#ifndef CONFIG_H
  #define CONFIG_H 1
  // --- button.ino ----------------------------------------------------------------
  // * Desktop buttons configuration
  #define BUTTON_POWER_PIN 27
  //#define BUTTON_RESET_PIN 2
  //#define BUTTON_RECONFIGURE_PIN 3
  // * Value when read a pressed button (LOW=PULLUP, hIGH=PULLDOWN)
  #define VALUE_BUTTON_POWER LOW
  //#define VALUE_BUTTON_RESET HIGH
  //#define VALUE_BUTTON_RECONFIGURE HIGH
  // -------------------------------------------------------------------------------

  // --- led.ino -------------------------------------------------------------------
  // Read LED status from motherboard
  //#define LED_POWER_PIN 4
  //#define LED_HDD_PIN 5
  // -------------------------------------------------------------------------------

  // --- display.ino ---------------------------------------------------------------
  // * Select your display type
  #define DISPLAY_TYPE DISPLAY_NONE             // No display
  //#define DISPLAY_TYPE DISPLAY_SERIAL           // No display, send message to serial console
  //#define DISPLAY_TYPE DISPLAY_SSD1306_128X32   // OLED [SSD1306] 128x32
  //#define DISPLAY_TYPE DISPLAY_SSD1306_128X64   // OLED [SSD1306] 128x64
  //#define DISPLAY_TYPE DISPLAY_LCD2004          // Text LCD [LCD2004] 20x4
  //#define DISPLAY_TYPE DISPLAY_LCD1602          // Text LCD [LCD1602] 16x2
  //#define DISPLAY_TYPE DISPLAY_SSH1106          // OLED [SSH1106] 128x64
  //
  // * SPI communication port connected to display
  //#define DISPLAY_SDA 6
  //#define DISPLAY_SCL 7
  // -------------------------------------------------------------------------------

  // --- relay.ino -----------------------------------------------------------------
  // Relay power connected at power button place on the motherboard
  #define RELAY_POWER_PIN 16
  #define RELAY_POWER_LEVEL_ON HIGH
  // Relay power connected at reset button place on the motherboard
  //#define RELAY_RESET_PIN 9
  //#define RELAY_RESET_LEVEL_ON LOW
  
  // Relays SYS# connected to turn on system coolers
  //#define RELAY_SYS1_PIN 10
  //#define RELAY_SYS1_LEVEL_ON LOW
  //#define RELAY_SYS2_PIN 11
  //#define RELAY_SYS2_LEVEL_ON LOW
  //#define RELAY_SYS3_PIN 12
  //#define RELAY_SYS3_LEVEL_ON LOW
  //#define RELAY_SYS4_PIN 13
  //#define RELAY_SYS4_LEVEL_ON LOW

  // Relay supply backligth to display
  //#define RELAY_BACK_PIN 13
  //#define RELAY_BACK_LEVEL_ON LOW
  // Time to turn off backlight in ms
  //#define BACKLIGHT_TIMEOUT 10000  

    // Timeout to release power button when turn on PC
  #define TIMEOUT_RELAY_ON 2000
  // Timeout to release power button when turn off PC
  #define TIMEOUT_RELAY_OFF 6000
  // Timeout to between power button cicle when turn off/turn on PC (used when we has no reset relay)
  #define TIMEOUT_RESET_INTERVAL 1000
  // Timeout to release the reset button
  #define TIMEOUT_RESET 1000
  // -------------------------------------------------------------------------------

  // --- rpm.ino -------------------------------------------------------------------
  // Connection to measurement pin from CPU and System coolers, used to calculate RPMs
  //#define RPM_CPU_PIN 14
  //#define RPM_SYS1_PIN 15
  //#define RPM_SYS2_PIN 16
  //#define RPM_SYS3_PIN 17
  //#define RPM_SYS4_PIN 18
  // -------------------------------------------------------------------------------

  // --- temperature.ino -----------------------------------------------------------
  // Port to communicate with DA18B20 temperature sensors using 1Wire protocol.
  // With running hardware we can enter configuration web page and link all
  // temperarure sensors with their locations (cpu, sys1, sys2, ...)
  //#define TEMP_WIRE_PIN 19
  // -------------------------------------------------------------------------------

  // --- uart.ino ------------------------------------------------------------------
  // Serial RS-232 communication (used to get remote OS info)
  //#define UART_TX_PIN 22;
  //#define UART_RX_PIN 23;
  // -------------------------------------------------------------------------------

  // --- sidekick.ino --------------------------------------------------------------
  // Onboard LED or Active Status LED PIN
  #define LED_PIN 2
  // Level to turn onboard LED on
  #define LED_LEVEL_ON HIGH
  // -------------------------------------------------------------------------------

  // --- data.ino ------------------------------------------------------------------
  // when no config was found, the system start into a failsafe mode, using this default values
  // default SSID when in AP name (reconfigure mode)
  #define DEFAULT_AP_SSID "SIDEKICK_AP"
  // default password to connect into AP
  #define DEFAULT_AP_PASS "password"
  // default password used to enter configuration
  #define DEFAULT_PASSWORD "password"
  // -------------------------------------------------------------------------------

  // --- datetime.ino --------------------------------------------------------------
  // Date and time configuration
  // Timezone (https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h)
  //          (https://github.com/mcxiaoke/ESPDateTime/blob/master/src/DateTimeTZ.h)
  #define TIMEZONE "<-03>3"
  // NTP servers - list of NTP server to be used
  #define NTPSERVERS "a.st1.ntp.br", "b.st1.ntp.br", "c.st1.ntp.br", "d.st1.ntp.br"
  #define DATE_FORMAT "%d/%m/%y"
  #define TIME_FORMAT "%M:%M:%S"
  // -------------------------------------------------------------------------------
  

#endif // <EOF>