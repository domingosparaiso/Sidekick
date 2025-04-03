#ifndef SIDEKICK_H
  #define SIDEKICK_H 1

  #define VERSAO "0.01-BETA"

  // #define MAX_BUFFER 20480
  #define CLIENT_WIFI 0
  #define AP_WIFI 1

  #define STATUS_CONFIG_WIFI_OK    0 // conectado no wifi
  #define STATUS_CONFIG_WIFI_1     1 // animação 1/2, conectando wifi
  #define STATUS_CONFIG_WIFI_2     2 // animação 2/2, conectando wifi
  #define STATUS_CONFIG_WIFI_ERROR 3 // erro conectando no wifi
  #define STATUS_UPLOAD_FILE_START 4 // iniciando upload
  #define STATUS_UPLOAD_FILE_RUN   5 // durante upload
  #define STATUS_UPLOAD_FILE_OK    6 // fim de upload
  #define STATUS_UPLOAD_FILE_ERROR 7 // erro no upload
  #define STATUS_REBOOT            8 // reboot sidekick
  #define STATUS_UPDATE_FIRMWARE   9 // iniciando update firmware
  #define STATUS_FORMAT_FS        10 // iniciando formatação do SPIFFS
  #define STATUS_FORMAT_OK        11 // fim de formatação OK
  #define STATUS_FORMAT_ERROR     12 // fim de formatação com erro

  #if DISPLAY_TYPE == DISPLAY_NONE
    #define DISPLAY_NAME DISPLAY_NAME_NONE
  #endif
  #if DISPLAY_TYPE == DISPLAY_SERIAL
    #define STATUS_LIST "WIFI OK", "WIFI Wait \\", "WIFI Wait /", "WIFI Erro", "Iniciando upload", "Upload em andamento", "Fim de upload", "Falha no upload", "Reboot", "Update firmware", "Formatando FS", "Formato concluido", "Erro na formatacao"
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

  #define RELAY_OFF 0
  #define RELAY_ON 1
  #define RELAY_POWER_OFF 2
  #define RELAY_POWER_ON 3
  #define RELAY_RESET 4
  #define ERROR_VALUE -1

#endif // <EOF>