#ifdef ESP32
  #include <WiFi.h>
  #include <WiFiAP.h>
  #include <WebServer.h>
#endif

#ifdef ESP8266
  #include <ESP8266WiFi.h>
  #include <WiFiClient.h>
  #include <ESP8266WebServer.h>
#endif

#include "wificonfig.h"
#include "Sidekick.h"

long next_check_wifi = 0;

void connect_wifi() {
  next_check_wifi = millis() + TIMEOUT_CHECK_WIFI;
  int retry_connect = RECONNECT_CLI;
  while(wifi_mode == WIFI_MODE_CLI && --retry_connect >= 0 && !connect_wifi_cli());
  if(WiFi.status() != WL_CONNECTED) connect_wifi_ap();
}

bool connect_wifi_cli() {
  int wait_wifi = TIME_WAIT_WIFI;
  int status_wifi = STATUS_CONFIG_WIFI_1;
  // If no SSID configured into storage, exit and try the AP mode
  if(strlen(CFG.data.CLI.wifi.SSID)==0) {
    wifi_mode = WIFI_MODE_AP;
    return(false);
  }

  console_log("--- SSID: ");
  console_log(String(CFG.data.CLI.wifi.SSID));
  console_log("\nConnecting WiFi");
  WiFi.begin(CFG.data.CLI.wifi.SSID, CFG.data.CLI.wifi.password);
  while (WiFi.status() != WL_CONNECTED && --wait_wifi >= 0) {
    console_log(".");
    display_status(status_wifi);
    status_wifi = (status_wifi==STATUS_CONFIG_WIFI_1)?STATUS_CONFIG_WIFI_2:STATUS_CONFIG_WIFI_1;
    delay(500);
  }
  if(WiFi.status() != WL_CONNECTED) {
    console_log(" [Error]\n");
    return(false);
  }
  console_log(" [OK]\n");
  display_status(STATUS_CONFIG_WIFI_OK);
  if(!CFG.data.CLI.DHCP) {
    IPAddress ip(CFG.data.CLI.IP[0], CFG.data.CLI.IP[1], CFG.data.CLI.IP[2], CFG.data.CLI.IP[3]);
    IPAddress gateway(CFG.data.CLI.GW[0], CFG.data.CLI.GW[1], CFG.data.CLI.GW[2], CFG.data.CLI.GW[3]);
    IPAddress subnet(CFG.data.CLI.MASK[0], CFG.data.CLI.MASK[1], CFG.data.CLI.MASK[2], CFG.data.CLI.MASK[3]);
    IPAddress dns(CFG.data.CLI.DNS[0], CFG.data.CLI.DNS[1], CFG.data.CLI.DNS[2], CFG.data.CLI.DNS[3]);
    WiFi.config(ip, dns, gateway, subnet);
  }
  console_log("Connected: [");
  console_log(WiFi.localIP().toString());
  console_log("]... [OK]\n");
  display_print(1, 1, F("WiFi CLI"));
  display_print(2, 1, WiFi.localIP().toString());
  return(true);
}

void connect_wifi_ap() {
  console_log("Configure WiFi into AP mode...");
  IPAddress ip(192, 168, 100, 1);
  IPAddress gateway(192, 168, 100, 1);
  IPAddress subnet(255, 255, 255, 0);
  if(strlen(CFG.data.AP.SSID)==0 || strlen(CFG.data.AP.password)==0) {
    strcpy(CFG.data.AP.SSID, DEFAULT_AP_SSID);
    strcpy(CFG.data.AP.password, DEFAULT_AP_PASS);
  }
  console_log(" [OK]\n");
  console_log("SSID: ");
  console_log(String(CFG.data.AP.SSID));
  console_log("\nPassword: ");
  console_log(String(CFG.data.AP.password));
  console_log("\nStart WiFi into AP Mode...");
  if (!WiFi.softAP(CFG.data.AP.SSID, CFG.data.AP.password)) {
    console_log("[Error]\n");
    display_print(1, 1, F("Wifi AP Error"));
    display_print(2, 1, F("<reboot>"));
    delay(10000);
    console_log("<reboot>");
    display_status(STATUS_REBOOT);
    ESP.restart();
  }
  WiFi.softAPConfig(ip, gateway, subnet);
  display_print(1, 1, CFG.data.AP.SSID);
  display_print(2, 1, CFG.data.AP.password);
  wifi_mode = WIFI_MODE_AP;
  IPAddress myIP = WiFi.softAPIP();
  console_log(String("\nConnected: [") + myIP.toString() + String("]... [OK]\n"));
  display_print(1, 1, F("WiFi AP"));
  display_print(2, 1, WiFi.localIP().toString());
}

void check_wifi() {
  if(next_check_wifi < millis()) {
    block_loop = true;
    if(WiFi.status() != WL_CONNECTED) {
      console_log("Lost WiFi connection\n");
      connect_wifi();
    }
    next_check_wifi = millis() + TIMEOUT_CHECK_WIFI;
    block_loop = false;
  }
}