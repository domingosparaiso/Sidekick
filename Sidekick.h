#ifndef SIDEKICK_H
  #define SIDEKICK_H 1

  #define VERSION "0.01b"

  #define CONSOLE_BAUD 115200
  #define TIMEOUT_CONSOLE 5000
  #define WAIT_TIME_TO_REBOOT 5000

  #define STATUS_CONFIG_WIFI_OK    0 // WiFi connected
  #define STATUS_CONFIG_WIFI_1     1 // Connecting WiFi, sprite 1/2
  #define STATUS_CONFIG_WIFI_2     2 // Connecting WiFi, sprite 2/2
  #define STATUS_CONFIG_WIFI_ERROR 3 // WiFi connection error
  #define STATUS_UPLOAD_FILE_START 4 // starting upload
  #define STATUS_UPLOAD_FILE_RUN   5 // on upload
  #define STATUS_UPLOAD_FILE_OK    6 // end of upload
  #define STATUS_UPLOAD_FILE_ERROR 7 // upload error
  #define STATUS_REBOOT            8 // reboot sidekick
  #define STATUS_UPDATE_FIRMWARE   9 // start firmware update
  #define STATUS_FORMAT_FS        10 // start storage (SPIFFS) formating
  #define STATUS_FORMAT_OK        11 // end format, OK
  #define STATUS_FORMAT_ERROR     12 // end format, error

  #if DISPLAY_TYPE == DISPLAY_NONE
    #define DISPLAY_NAME DISPLAY_NAME_NONE
  #endif
  #if DISPLAY_TYPE == DISPLAY_SERIAL
    #define STATUS_LIST "WIFI OK", "WIFI Wait \\", "WIFI Wait /", "WIFI Error", "Start upload", "On Upload", "End upload", "Error upload", "Reboot", "Update firmware", "Format FS", "Format OK", "Format Error"
    #define DISPLAY_NAME DISPLAY_NAME_SERIAL
  #endif
  #if DISPLAY_TYPE == DISPLAY_SSD1306_128X32
    #define DISPLAY_NAME DISPLAY_NAME_SSD1306_128X32
  #endif
  #if DISPLAY_TYPE == DISPLAY_SSD1306_128X64
    #define DISPLAY_NAME DISPLAY_NAME_SSD1306_128X64
  #endif
  #if DISPLAY_TYPE == DISPLAY_SSH1106
    #define DISPLAY_NAME DISPLAY_NAME_SSH1106
  #endif
  #if DISPLAY_TYPE == DISPLAY_LCD2004
    #define DISPLAY_NAME DISPLAY_NAME_LCD2004
  #endif
  #if DISPLAY_TYPE == DISPLAY_LCD1602
    #define DISPLAY_NAME DISPLAY_NAME_LCD1602
  #endif

  #define ERROR_VALUE -1
  #define RELAY_OFF 0
  #define RELAY_ON 1
  #define RELAY_POWER_OFF 2
  #define RELAY_POWER_ON 3
  #define RELAY_RESET 4
  #define RELAY_POWER_OFF_ON 5

  #if LED_LEVEL_ON == LOW
    #define LED_LEVEL_OFF HIGH
  #else
    #define LED_LEVEL_OFF LOW
  #endif
  #define OFF 0
  #define ON 1
  #define FLASH 2
  #define INIT 3

  #define WIFI_CLI 0
  #define WIFI_AP 1
  #define TIME_WAIT_WIFI 100
  #define RECONNECT_CLI 3

#endif // <EOF>