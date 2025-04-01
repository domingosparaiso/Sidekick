#include <EEPROM.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <WebServer.h>
#include <Update.h>
#include <uri/UriBraces.h>
#include "FS.h"
#include "SPIFFS.h"
#include "config.h"
#include "Sidekick.h"
#include "data.h"
#include "wifi.h"
#define LED_PIN 2

WebServer server(80);
int SIZE_config_data;
union config_union CFG;
String serialNumber = "";
int wifi_method = CLIENT_WIFI;

void changeLED() {
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
}

void setup() {
  delay(2000);
  Serial.begin(115200);
	while (!Serial && millis() < 5000);
  delay(500);
  Serial.println(F("\nPower ON"));
	Serial.print(F("\Board "));
	Serial.println(ARDUINO_BOARD);
	Serial.print(F("CPU Frequency = "));
	Serial.print(F_CPU / 1000000);
	Serial.println(F(" MHz"));
  pinMode(LED_PIN, OUTPUT);
  long unsigned int espmac = ESP.getEfuseMac() >> 24;
  serialNumber = String(espmac, HEX);
  serialNumber.toUpperCase();
  Serial.println(String(F("Device Serial: ")) + serialNumber);
  inicializa_flash();  // data.ino (OK)
  display_init();
  delay(1000);
  Serial.println(F("Init flash device... [OK]"));
  load_CFG(); // data.ino
  delay(1000);
  Serial.println(F("Load configuration... [OK]"));
  check_wifi(); // wifi.ino
  server_setup(); // server.ino
  button_init();
  led_init();
  relay_init();
  rpm_init();
  temperature_init();
  Serial.println(F("System ready!\n"));
}

void loop() {
  check_wifi(); // wifi.ino
  server_loop(); // server.ino
}

void wifi_connect(int connection_type) {
  if(connection_type == CLIENT_WIFI) {
    if(strlen(CFG.data.CLI.wifi.SSID)==0) {
      strcpy(CFG.data.CLI.wifi.SSID,"SOTELE#2Ghz");
      strcpy(CFG.data.CLI.wifi.password,"mamusKa#76");
      CFG.data.CLI.IP[0]=192;CFG.data.CLI.IP[1]=168;CFG.data.CLI.IP[2]=1;CFG.data.CLI.IP[3]=88;
      CFG.data.CLI.GW[0]=192;CFG.data.CLI.GW[1]=168;CFG.data.CLI.GW[2]=1;CFG.data.CLI.GW[3]=10;
      CFG.data.CLI.MASK[0]=255;CFG.data.CLI.MASK[1]=255;CFG.data.CLI.MASK[2]=255;CFG.data.CLI.MASK[3]=0;
    }
    WiFi.begin(CFG.data.CLI.wifi.SSID, CFG.data.CLI.wifi.password);
    Serial.print(F("SSID: "));
    Serial.println(CFG.data.CLI.wifi.SSID);
    Serial.print(F("PASS: "));
    Serial.println(CFG.data.CLI.wifi.password);
    Serial.print(F("Conectando rede wifi "));
    Serial.print(CFG.data.CLI.wifi.SSID);
    int status_oled = OLED_CONFIG_WIFI_1;
    int retry_count = 50;
    while (WiFi.status() != WL_CONNECTED) {
      oled_status(status_oled);
      status_oled = (status_oled==OLED_CONFIG_WIFI_1)?OLED_CONFIG_WIFI_2:OLED_CONFIG_WIFI_1;
      Serial.print(".");
      delay(500);
      if(retry_count-- < 0) break;
    }
    Serial.println("");
    if(WiFi.status() != WL_CONNECTED) {
      Serial.println(F("Conexao wifi... [Falha]"));
      oled_status(OLED_CONFIG_WIFI_ERROR);
    } else {
      oled_status(OLED_CONFIG_WIFI_OK);
      if(!CFG.data.CLI.DHCP) {
        IPAddress ip(CFG.data.CLI.IP[0], CFG.data.CLI.IP[1], CFG.data.CLI.IP[2], CFG.data.CLI.IP[3]);
        IPAddress gateway(CFG.data.CLI.GW[0], CFG.data.CLI.GW[1], CFG.data.CLI.GW[2], CFG.data.CLI.GW[3]);
        IPAddress subnet(CFG.data.CLI.MASK[0], CFG.data.CLI.MASK[1], CFG.data.CLI.MASK[2], CFG.data.CLI.MASK[3]);
        IPAddress dns(CFG.data.CLI.DNS[0], CFG.data.CLI.DNS[1], CFG.data.CLI.DNS[2], CFG.data.CLI.DNS[3]);
        WiFi.config(ip, dns, gateway, subnet);
      }
      Serial.print(F("Conectado: "));
      Serial.print(WiFi.localIP().toString());
      Serial.println(F("... [OK]"));
      oled_print(1, F("Conectado"));
      oled_print(2, WiFi.localIP().toString());
    }
  } else {
    Serial.println(F("Iniciando wifi AP"));
    IPAddress AP_LOCAL_IP(192, 168, 100, 1);
    IPAddress AP_GATEWAY_IP(192, 168, 100, 1);
    IPAddress AP_NETWORK_MASK(255, 255, 255, 0);
    if(strlen(CFG.data.AP.SSID)==0 || strlen(CFG.data.AP.password)==0) {
      strcpy(CFG.data.AP.SSID,"CONFIG_INIT");
      strcpy(CFG.data.AP.password, "password");
    }
    Serial.println(F("Wifi AP"));
    Serial.print(F("SSID: "));
    Serial.println(CFG.data.AP.SSID);
    Serial.print(F("Password: "));
    Serial.println(CFG.data.AP.password);
    if (!WiFi.softAP(CFG.data.AP.SSID, CFG.data.AP.password)) {
      Serial.println(F("Erro definindo Wifi AP"));
      Serial.println(F("Entrando em modo de recuperacao..."));
      strcpy(CFG.data.AP.SSID,"CONFIG_INIT");
      strcpy(CFG.data.AP.password, "password");
      if (!WiFi.softAP(CFG.data.AP.SSID, CFG.data.AP.password)) {
        Serial.println(F("Erro definindo Wifi AP em modo recuperacao"));
        Serial.println(F("O sistema sera reiniciado em 10s"));
        oled_print(1,F("Wifi AP Erro"));
        oled_print(2,F("<reboot>"));
        delay(10000);
        Serial.println(F("<reboot>"));
        oled_status(OLED_REBOOT);
        ESP.restart();
      }
    }
    WiFi.softAPConfig(AP_LOCAL_IP, AP_GATEWAY_IP, AP_NETWORK_MASK);
    oled_print(1, CFG.data.AP.SSID);
    oled_print(2, CFG.data.AP.password);
    wifi_method = AP_WIFI;
    Serial.print(F("Conectado: "));
    Serial.print(WiFi.localIP());
    Serial.println(F("... [OK]"));
    oled_print(1, F("Conectado"));
    oled_print(2, WiFi.localIP().toString());
  }
}

void check_wifi() {
  if(wifi_method == CLIENT_WIFI) {
    if(WiFi.status() != WL_CONNECTED) {
      int retry_connect = 3;
      while(WiFi.status() != WL_CONNECTED && retry_connect-- > 0) {
        wifi_connect(CLIENT_WIFI);
      }
      if(WiFi.status() != WL_CONNECTED) {
        wifi_connect(AP_WIFI);
      }
    }
  } 
}