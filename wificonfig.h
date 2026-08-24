#pragma once

#include <functional>

// ESP32 uses ESPAsyncWebServer (async), ESP8266 keeps the synchronous
// ESP8266WebServer. Every other file talks to the webserver only through
// the SidekickRequest/req_* adapter declared below, so this is the only
// place that needs to know which library is actually in use.
#ifdef ESP32
  #include <WiFi.h>
  #include <WiFiAP.h>
  #include <AsyncTCP.h>
  #include <ESPAsyncWebServer.h>
  AsyncWebServer server(80);
  typedef AsyncWebServerRequest* SidekickRequest;
  // ESPAsyncWebServer (ESP32Async fork) has no plain HTTPMethod enum; HTTP_GET/HTTP_POST/...
  // are WebRequestMethod values that implicitly convert to this composite type.
  typedef WebRequestMethodComposite SidekickHTTPMethod;
#endif

#ifdef ESP8266
  #include <ESP8266WiFi.h>
  #include <WiFiClient.h>
  #include <ESP8266WebServer.h>
  ESP8266WebServer server(80);
  typedef ESP8266WebServer* SidekickRequest;
  typedef HTTPMethod SidekickHTTPMethod;
#endif

int wifi_mode = WIFI_STA;

// A handler that receives the request adapter, used uniformly on both platforms
typedef std::function<void(SidekickRequest)> SidekickHandler;

// Unified upload chunk callback: index==0 marks the first chunk (open), final==true
// marks the last one (close); data/len may be empty on start/end-only calls.
typedef std::function<void(SidekickRequest, String filename, size_t index, uint8_t *data, size_t len, bool final)> SidekickUploadHandler;

// One response header to add before send(); used because ESPAsyncWebServer needs
// headers attached to the response object before it is sent, unlike the sendHeader()
// then send() two-step used by the synchronous WebServer libraries.
struct ReqHeader {
  const char* name;
  String value;
};

// --- Request adapter, implemented in webserver.ino ------------------------------
String req_arg(SidekickRequest request, const String &name);
bool req_hasArg(SidekickRequest request, const String &name);
int req_args(SidekickRequest request);
String req_argName(SidekickRequest request, int i);
String req_argValue(SidekickRequest request, int i);
bool req_hasHeader(SidekickRequest request, const String &name);
String req_header(SidekickRequest request, const String &name);
void req_send(SidekickRequest request, int code, const String &contentType, const String &content, ReqHeader* headers = nullptr, int headerCount = 0);
void req_sendFile(SidekickRequest request, const String &path, const String &contentType);
void req_on(const String &uri, SidekickHTTPMethod method, SidekickHandler handler);
void req_onUpload(const String &uri, SidekickHTTPMethod method, SidekickHandler onComplete, SidekickUploadHandler onChunk);
void req_onNotFound(SidekickHandler handler);

// relay.ino: declared here (with its default request=nullptr) so every caller sees the
// same default — button.ino/display.ino call it with just 2 args from outside a request.
void relay_set(int relay_port, int value, SidekickRequest request = nullptr);
