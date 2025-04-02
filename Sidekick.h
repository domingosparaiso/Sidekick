#ifndef SIDEKICK_H
#define SIDEKICK_H 1

#define VERSAO "0.01-BETA"

#define MAX_BUFFER 20480
// begin -- data.h
#define MAX_SYSTEM   4
#define MAX_DEVICES 26
#define CLIENT_WIFI 0
#define AP_WIFI 1

#define DISPLAY_NONE 0
#define DISPLAY_SERIAL 1 // No display, envia para serial
#define DISPLAY_SSD1306_128X32 2 // OLED [SSD1306] 128x32
#define DISPLAY_SSD1306_128X64 3 // OLED [SSD1306] 128x64
#define DISPLAY_LCD2004 4 // Text LCD [LCD2004] 20x4
#define DISPLAY_LCD1602 5 // Text LCD [LCD1602] 16x2
#define DISPLAY_SSH1106 6 // OLED [SSH1106] 128x64

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

#if DISPLAY_TYPE == DISPLAY_SERIAL
#define STATUS_LIST "WIFI OK", "WIFI Wait", "WIFI Wait", , "WIFI Erro", "Iniciando upload", "Upload em andamento", "Fim de upload", "Falha no upload", "Reboot", "Update firmware", "Formatando FS", "Formato concluido", "Erro na formatacao"
#endif

#define RELAY_OFF       0
#define RELAY_ON        1
#define RELAY_POWER_OFF 2
#define RELAY_POWER_ON  3

#ifdef RELAY_POWER_LEVEL_ON
  #if RELAY_POWER_LEVEL_ON == HIGH
    #define RELAY_POWER_LEVEL_OFF LOW
  #else
    #define RELAY_POWER_LEVEL_OFF HIGH
  #endif
#endif
#ifdef RELAY_RESET_LEVEL_ON
  #if RELAY_RESET_LEVEL_ON == HIGH
    #define RELAY_RESET_LEVEL_OFF LOW
  #else
    #define RELAY_RESET_LEVEL_OFF HIGH
  #endif
#endif
#ifdef RELAY_SYS1_LEVEL_ON
  #if RELAY_SYS1_LEVEL_ON == HIGH
    #define RELAY_SYS1_LEVEL_OFF LOW
  #else
    #define RELAY_SYS1_LEVEL_OFF HIGH
  #endif
#endif
#ifdef RELAY_SYS2_LEVEL_ON
  #if RELAY_SYS2_LEVEL_ON == HIGH
    #define RELAY_SYS2_LEVEL_OFF LOW
  #else
    #define RELAY_SYS2_LEVEL_OFF HIGH
  #endif
#endif
#ifdef RELAY_SYS3_LEVEL_ON
  #if RELAY_SYS3_LEVEL_ON == HIGH
    #define RELAY_SYS3_LEVEL_OFF LOW
  #else
    #define RELAY_SYS3_LEVEL_OFF HIGH
  #endif
#endif
#ifdef RELAY_SYS4_LEVEL_ON
  #if RELAY_SYS4_LEVEL_ON == HIGH
    #define RELAY_SYS4_LEVEL_OFF LOW
  #else
    #define RELAY_SYS4_LEVEL_OFF HIGH
  #endif
#endif

struct rpm_data {
  long counter;
  long millis;
  long rpm;
};


#endif