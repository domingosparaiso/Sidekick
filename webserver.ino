#include <LittleFS.h>
#include "Sidekick.h"
#include "wificonfig.h"
// Request adapter: the only place that translates the uniform SidekickRequest
// API used everywhere else into calls on the actual webserver library in use
// (ESP8266WebServer on ESP8266, ESPAsyncWebServer on ESP32).

String req_arg(SidekickRequest request, const String &name) {
  #ifdef ESP8266
    return request->arg(name);
  #endif
  #ifdef ESP32
    if (request->hasParam(name)) return request->getParam(name)->value();
    if (request->hasParam(name, true)) return request->getParam(name, true)->value();
    return String();
  #endif
}

bool req_hasArg(SidekickRequest request, const String &name) {
  #ifdef ESP8266
    return request->hasArg(name);
  #endif
  #ifdef ESP32
    return request->hasParam(name) || request->hasParam(name, true);
  #endif
}

int req_args(SidekickRequest request) {
  #ifdef ESP8266
    return request->args();
  #endif
  #ifdef ESP32
    return request->params();
  #endif
}

String req_argName(SidekickRequest request, int i) {
  #ifdef ESP8266
    return request->argName(i);
  #endif
  #ifdef ESP32
    return request->getParam(i)->name();
  #endif
}

String req_argValue(SidekickRequest request, int i) {
  #ifdef ESP8266
    return request->arg(i);
  #endif
  #ifdef ESP32
    return request->getParam(i)->value();
  #endif
}

bool req_hasHeader(SidekickRequest request, const String &name) {
  return request->hasHeader(name);
}

String req_header(SidekickRequest request, const String &name) {
  #ifdef ESP8266
    return request->header(name);
  #endif
  #ifdef ESP32
    const AsyncWebHeader* h = request->getHeader(name);
    return h ? h->value() : String();
  #endif
}

void req_send(SidekickRequest request, int code, const String &contentType, const String &content, ReqHeader* headers, int headerCount) {
  #ifdef ESP8266
    for (int i = 0; i < headerCount; i++) request->sendHeader(headers[i].name, headers[i].value);
    request->send(code, contentType, content);
  #endif
  #ifdef ESP32
    AsyncWebServerResponse *response = request->beginResponse(code, contentType, content);
    for (int i = 0; i < headerCount; i++) response->addHeader(headers[i].name, headers[i].value);
    request->send(response);
  #endif
}

void req_sendFile(SidekickRequest request, const String &path, const String &contentType) {
  #ifdef ESP8266
    File f = LittleFS.open(path, "r");
    if (f) {
      request->streamFile(f, contentType);
      f.close();
    }
  #endif
  #ifdef ESP32
    request->send(LittleFS, path, contentType);
  #endif
}

// Register a public (unauthenticated) endpoint
void req_on(const String &uri, SidekickHTTPMethod method, SidekickHandler handler) {
  #ifdef ESP8266
    server.on(uri, method, [handler]() { handler(&server); });
  #endif
  #ifdef ESP32
    server.on(uri.c_str(), method, [handler](AsyncWebServerRequest *request) { handler(request); });
  #endif
}

// Register a public (unauthenticated) upload endpoint: onComplete runs once the
// whole body/upload has been received, onChunk runs for every piece of data.
void req_onUpload(const String &uri, SidekickHTTPMethod method, SidekickHandler onComplete, SidekickUploadHandler onChunk) {
  #ifdef ESP8266
    server.on(uri, method,
      [onComplete]() { onComplete(&server); },
      [onChunk]() {
        HTTPUpload &upload = server.upload();
        static size_t bytesSoFar = 0;
        if (upload.status == UPLOAD_FILE_START) {
          bytesSoFar = 0;
          onChunk(&server, upload.filename, 0, nullptr, 0, false);
        } else if (upload.status == UPLOAD_FILE_WRITE) {
          onChunk(&server, upload.filename, bytesSoFar, upload.buf, upload.currentSize, false);
          bytesSoFar += upload.currentSize;
        } else if (upload.status == UPLOAD_FILE_END) {
          onChunk(&server, upload.filename, bytesSoFar, nullptr, 0, true);
        }
      }
    );
  #endif
  #ifdef ESP32
    server.on(uri.c_str(), method,
      [onComplete](AsyncWebServerRequest *request) { onComplete(request); },
      [onChunk](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
        onChunk(request, filename, index, data, len, final);
      }
    );
  #endif
}

void req_onNotFound(SidekickHandler handler) {
  #ifdef ESP8266
    server.onNotFound([handler]() { handler(&server); });
  #endif
  #ifdef ESP32
    server.onNotFound([handler](AsyncWebServerRequest *request) { handler(request); });
  #endif
}
