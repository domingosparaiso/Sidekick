#include "Sidekick.h"
#include "wificonfig.h"
// Console: bridges HOST_SERIAL (a UART wired to a host device, or the ESP32's own
// USB serial) to the web UI over a WebSocket, so it can be used as a remote ANSI
// terminal. Ported from the standalone USB-Javascript-Wifi sketch.
// Needs a WebSocket, so it only exists on ESP32 (ESPAsyncWebServer); ESP8266 builds
// keep the synchronous ESP8266WebServer, which has no WebSocket support.
// Named hostconsole_* (not console_*) to avoid clashing with display.ino's
// console_init()/console_log(), which is Sidekick's own unrelated debug logger.
//
// The WebSocket event callback is kept as an inline lambda (not a named top-level
// function) even though its body is already ESP32-only: Arduino's auto-generated
// prototypes are built from a raw text scan that ignores #ifdef, so a named function
// whose signature mentions AsyncWebSocket/AwsEventType/etc. would still get a bogus
// forward declaration on the ESP8266 build, where those types don't exist.

#ifdef ESP32
AsyncWebSocket hostConsoleSocket("/console-ws");
#endif

// Setup HOST_SERIAL and announce the resource, called from Sidekick.ino::setup()
void hostconsole_init() {
  #ifdef ESP32
    resourcesAddValue("console", "websocket");
    HOST_SERIAL.begin(HOST_BAUD);
    hostConsoleSocket.onEvent([](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
      if (type == WS_EVT_DATA) {
        AwsFrameInfo *info = (AwsFrameInfo *)arg;
        // Assume small messages (keystrokes) arrive in a single complete frame
        if (info->final && info->index == 0 && info->len == len) {
          HOST_SERIAL.write(data, len);
        }
      }
    });
  #endif
}

// Attach the WebSocket to the webserver, called from configServerInit()
void hostconsole_register() {
  #ifdef ESP32
    server.addHandler(&hostConsoleSocket);
  #endif
}

// Forward anything received on HOST_SERIAL to connected browsers, called from server_loop()
void hostconsole_loop() {
  #ifdef ESP32
    static uint8_t buf[512];
    size_t len = 0;
    while (HOST_SERIAL.available() && len < sizeof(buf)) {
      buf[len++] = (uint8_t) HOST_SERIAL.read();
    }
    if (len > 0) hostConsoleSocket.binaryAll(buf, len);
    hostConsoleSocket.cleanupClients();
  #endif
}
