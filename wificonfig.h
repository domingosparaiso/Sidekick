#pragma once

#ifdef ESP32
  #include <WebServer.h>
  WebServer server(80);
#endif

#ifdef ESP8266
  #include <ESP8266WebServer.h>
  ESP8266WebServer server(80);
#endif

int wifi_mode = WIFI_MODE_CLI;
