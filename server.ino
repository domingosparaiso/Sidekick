#ifdef ESP32
  #include <Update.h>
#endif
#ifdef ESP8266
  #include <WiFiClient.h>
#endif
#include <LittleFS.h>
#include "Sidekick.h"
#include "wificonfig.h"
#include "html.h"
#include "storage.h"
#include "cookie.h"

uint8_t otaDone = 0;
File uploadFile;
long timeout_reboot = 0;

// Completion handler for "/update": fires once the firmware body was fully received
void handleUpdateEnd(SidekickRequest request) {
  ReqHeader headers[3];
  int n = 0;
  headers[n++] = { "Connection", "close" };
  if (Update.hasError()) {
    req_send(request, 502, "text/plain", "Update Error.", headers, n);
  } else {
    headers[n++] = { "Refresh", "10" };
    headers[n++] = { "Location", "/" };
    req_send(request, 307, "text/html", "", headers, n);
    ESP.restart();
  }
}

// Upload chunk handler for "/update": index==0 is the first chunk, final==true is the last one
void handleUpdateChunk(SidekickRequest request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  if(!checkCookie(request)) return;
  display_status(STATUS_UPDATE_FIRMWARE);
  if (index == 0) {
    display_status(STATUS_UPLOAD_FILE_START);
    size_t fsize = 0;
    if (req_hasArg(request, "size")) fsize = req_arg(request, "size").toInt();
    if (!Update.begin(fsize)) {
      otaDone = 0;
    }
  }
  if (len) {
    if (Update.write(data, len) != len) {
      display_status(STATUS_UPLOAD_FILE_RUN);
    } else {
      otaDone = 100 * Update.progress() / Update.size();
      display_print(2, 1, String(otaDone));
    }
  }
  if (final) {
    if (Update.end(true)) {
      display_status(STATUS_UPLOAD_FILE_OK);
    } else {
      display_status(STATUS_UPLOAD_FILE_ERROR);
      otaDone = 0;
    }
  }
}

// Zero-pad a number to 2 digits
String pad2(unsigned long value) {
  String result = String(value);
  if(result.length() < 2) result = "0" + result;
  return result;
}

// Format the time elapsed since boot as "Xd HH:MM:SS"
String uptime_string() {
  unsigned long totalSeconds = millis() / 1000;
  unsigned long days = totalSeconds / 86400;
  unsigned long hours = (totalSeconds % 86400) / 3600;
  unsigned long minutes = (totalSeconds % 3600) / 60;
  unsigned long seconds = totalSeconds % 60;
  return String(days) + "d " + pad2(hours) + ":" + pad2(minutes) + ":" + pad2(seconds);
}

String config_json() {
  int countmap = 0;
  #ifdef TEMP_WIRE_PIN
    countmap = temperature_count;
  #endif
  String DHCPcfg = (CFG.data.CLI.DHCP)?String("dhcp"):String("fixo");
  String result =   "{ \"userName\": \"" +       String(CFG.data.userName) +         "\"," +
                      "\"hostname\": \"" +         String(CFG.data.hostname) +         "\"," +
                      "\"CLI_wifi_SSID\":\"" +     String(CFG.data.CLI.wifi.SSID) +     "\"," +
                      "\"CLI_wifi_password\":\"" + String(CFG.data.CLI.wifi.password) + "\"," +
                      "\"CLI_DHCP\":\"" +          DHCPcfg +                            "\"," +
                      "\"CLI_IP\": [\"" +          String(CFG.data.CLI.IP[0]) +         "\"," +
                                      "\"" +        String(CFG.data.CLI.IP[1]) +         "\"," +
                                      "\"" +        String(CFG.data.CLI.IP[2]) +         "\"," +
                                      "\"" +        String(CFG.data.CLI.IP[3]) +         "\"]," +
                      "\"CLI_MASK\": [\"" +        String(CFG.data.CLI.MASK[0]) +       "\"," +
                                      "\"" +        String(CFG.data.CLI.MASK[1]) +       "\"," +
                                      "\"" +        String(CFG.data.CLI.MASK[2]) +       "\"," +
                                      "\"" +        String(CFG.data.CLI.MASK[3]) +       "\"]," +
                      "\"CLI_GW\": [\"" +          String(CFG.data.CLI.GW[0]) +         "\"," +
                                      "\"" +        String(CFG.data.CLI.GW[1]) +         "\"," +
                                      "\"" +        String(CFG.data.CLI.GW[2]) +         "\"," +
                                      "\"" +        String(CFG.data.CLI.GW[3]) +         "\"]," +
                      "\"CLI_DNS\": [\"" +         String(CFG.data.CLI.DNS[0]) +        "\"," +
                                      "\"" +        String(CFG.data.CLI.DNS[1]) +        "\"," +
                                      "\"" +        String(CFG.data.CLI.DNS[2]) +        "\"," +
                                      "\"" +        String(CFG.data.CLI.DNS[3]) +        "\"]," +
                      "\"AP_SSID\": \"" +          String(CFG.data.AP.SSID) +           "\"," +
                      "\"AP_password\": \"" +      String(CFG.data.AP.password) +       "\"," +
                      "\"password\": \"" +         String(CFG.data.password) +          "\"," +
                      "\"countmap\": \"" +         String(countmap) +                   "\"," +
                      "\"map\": [\"" +             String(CFG.data.maptemp[0]) +        "\"," +
                                      "\"" +        String(CFG.data.maptemp[1]) +        "\"," +
                                      "\"" +        String(CFG.data.maptemp[2]) +        "\"," +
                                      "\"" +        String(CFG.data.maptemp[3]) +        "\"," +
                                      "\"" +        String(CFG.data.maptemp[4]) +        "\"]," +
                      "\"timeout\": \"" +          String(CFG.data.timeout_backlight) + "\"}";
  return(result);
}

void reconfigure(SidekickRequest request) {
  String seq;
  req_arg(request, "userName").toCharArray(CFG.data.userName, 32);
  req_arg(request, "hostname").toCharArray(CFG.data.hostname, 32);
  req_arg(request, "password").toCharArray(CFG.data.password, 32);
  req_arg(request, "AP_SSID").toCharArray(CFG.data.AP.SSID,32);
  req_arg(request, "AP_password").toCharArray(CFG.data.AP.password,32);
  req_arg(request, "CLI_wifi_SSID").toCharArray(CFG.data.CLI.wifi.SSID, 32);
  req_arg(request, "CLI_wifi_password").toCharArray(CFG.data.CLI.wifi.password, 32);
  CFG.data.CLI.DHCP = (req_arg(request, "CLI_DHCP")=="dhcp");
  if(!CFG.data.CLI.DHCP) {
    for(int c=1; c<=4; c++) {
      seq = String(c);
      CFG.data.CLI.IP[c-1] = (uint8_t) req_arg(request, String("CLI_IP_")+seq).toInt(); //endereço IP
      CFG.data.CLI.MASK[c-1] = (uint8_t) req_arg(request, String("CLI_MASK_")+seq).toInt(); //mascara subrede
      CFG.data.CLI.GW[c-1] = (uint8_t) req_arg(request, String("CLI_GW_")+seq).toInt(); //gateway
      CFG.data.CLI.DNS[c-1] = (uint8_t) req_arg(request, String("CLI_DNS_")+seq).toInt(); //dns
    }
  }
  for(int c=1; c<=5; c++) {
    CFG.data.maptemp[c-1] = 0;
    seq = String(c);
    if(req_arg(request, "TMAP_"+seq).length() > 0) {
      CFG.data.maptemp[c-1] = req_arg(request, "TMAP_"+seq).toInt();
    }

  }
  CFG.data.timeout_backlight = 0;
  if(req_arg(request, "timeout").length() > 0) CFG.data.timeout_backlight = req_arg(request, "timeout").toInt();
  save_CFG();
  req_send(request, 200, "text/html", "<html><body style='margin:0;padding:0;border:0'><div id=m>Configuration Saved</div></body><script>setTimeout(()=>{getElementeById('m').innerHTML='';},10000);</script></html>");
}

// Upload chunk handler for "/fs-upload": index==0 is the first chunk, final==true is the last one
void handleFileUploadChunk(SidekickRequest request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  if(!checkCookie(request)) return;
  if (!filename.startsWith("/")) { filename = "/" + filename; }
  if (index == 0) {
    if(filename == "/config.bin") {
      uploadFile = File();
    } else {
      uploadFile = LittleFS.open(filename, "w");
    }
  }
  if (uploadFile && len) {
    uploadFile.write(data, len);
  }
  if (final && uploadFile) {
    uploadFile.close();
  }
}

void configServerInit() {

  // Public login endpoints (must work without a session cookie)
  loginServerInit();

  // Main application page, gated behind a valid session cookie
  authOn("/", HTTP_GET, serveIndexPage);
  authOn("/index.html", HTTP_GET, serveIndexPage);

  // Never expose the raw config file (holds userName/password) over HTTP
  req_on("/config.bin", HTTP_GET, [](SidekickRequest request) { req_send(request, 403, "text/plain", "Forbidden"); });

  // Upload page to flash a new firmware
  authOn("/upload", HTTP_GET, [](SidekickRequest request) {
    req_send(request, 200, "text/html", uploadHtml);
  });

  // Logout
  authOn("/logout", HTTP_GET, [](SidekickRequest request) {
	  clearCookie(request);
    req_send(request, 200, "application/json", "{\"status\":\"OK\"}");
  });

  // Reboot the device
  authOn("/reboot", HTTP_GET, [](SidekickRequest request) {
    req_send(request, 200, "text/html", "<html><body><h1>Reboot in progress...</h1></body></html>");
    display_status(STATUS_REBOOT);
    console_log("Reboot in progress...");
    timeout_reboot = millis() + WAIT_TIME_TO_REBOOT;
  });

  // Backend to update config parameters
  authOn("/setup", HTTP_POST, reconfigure);

  // Register all server endpoints from resources
  button_register();
  led_register();
  relay_register();
  rpm_register();
  temperature_register();
  hostconsole_register();

  // return a json with all resources
  authOn("/resources.json", HTTP_GET, [](SidekickRequest request) {
    String result = resourcesJson.substring(0, resourcesJson.length() - 1) + ",\"uptime\":\"" + uptime_string() + "\"}";
    req_send(request, 200, "application/json", result);
  });

  // return a json with actual configuration
  authOn("/config.json", HTTP_GET, [](SidekickRequest request) {
    req_send(request, 200, "application/json", config_json());
  });

  // Firmware update
  authOnUpload("/update", HTTP_POST, handleUpdateEnd, handleUpdateChunk);

  // Delete files from storage
  authOn("/delete", HTTP_GET, [](SidekickRequest request) {
    for (int i = 0; i < req_args(request); i++) {
      if(req_argName(request, i) == "arq") {
        String fname = req_argValue(request, i);
        if(!fname.startsWith("/")) {
          fname = "/" + fname;
        }
        if(fname != "/config.bin") {
          LittleFS.remove(fname);
        }
      }
    }
    req_send(request, 200, "text/plain", "");
  });

  // storage format
  authOn("/format", HTTP_GET, [](SidekickRequest request) {
    display_status(STATUS_FORMAT_FS);
    console_log("<STORAGE FORMAT>");
    if(LittleFS.format()) {
      req_send(request, 200, "text/html", "<html><head><link rel='stylesheet' href='style.css'></head><body>Flash storage formated.<hr><a href=/home>Home</a></body></html>");
      display_status(STATUS_FORMAT_OK);
    } else {
      req_send(request, 200, "text/html", "<html><head><link rel='stylesheet' href='style.css'></head><body>Error at Flash storage format.<hr><a href=/home>Home</a></body></html>");
      display_status(STATUS_FORMAT_ERROR);
    }
  });

  // Storage file management
  authOn("/fs", HTTP_GET, [](SidekickRequest request) {
    if(LittleFS.exists("/fs.html")) {
      req_sendFile(request, "/fs.html", "text/html");
      return;
    }
    req_send(request, 200, "text/html", String(fileHtml));
  });

  // Files list (used by '/fs')
  authOn("/filelist", HTTP_GET, [](SidekickRequest request) {
    String result = "{ \"list\": [";
    String sep = "";
    #ifdef ESP8266
      Dir root = LittleFS.openDir("/");
      while (root.next()) {
        File file = root.openFile("r");
        if(String(root.fileName()) != "config.bin") {
          result += sep + String("{ \"file\": \"") + String(root.fileName()) + String("\", \"size\": ") + String(file.size()) + String("}");
          sep = ",";
        }
        file.close();
      }
    #endif
    #ifdef ESP32
      File root = LittleFS.open("/");
      if (!root || !root.isDirectory()) {
        result += "<tr><td>Not found</td></tr>";
      } else {
        File file = root.openNextFile();
        while (file) {
          if(String(file.name()) != "config.bin") {
            result += sep + String("{ \"file\": \"") + String(file.name()) + String("\", \"size\": ") + String(file.size()) + String("}");
            sep = ",";
          }
          file = root.openNextFile();
        }
      }
    #endif
    result += "]}";
    req_send(request, 200, "application/json", result);
  });

  // File upload
  authOnUpload("/fs-upload", HTTP_POST, [](SidekickRequest request) {
    ReqHeader headers[1] = { { "Connection", "close" } };
    req_send(request, 200, "text/plain", "OK", headers, 1);
  }, handleFileUploadChunk);

  // Retrieve files from storage
  server.serveStatic("/", LittleFS, "/");

  // url not found, open index.html instead send a 404 error, if index.html doesn't exists, send a message to upload one
  req_onNotFound([](SidekickRequest request) {
    if(!checkCookie(request)) { serveLoginPage(request); return; }
    serveIndexPage(request);
  });

  // enable CORS header in webserver results
  #ifdef ESP8266
    server.enableCORS(true);
  #endif
  #ifdef ESP32
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  #endif

  // Start webserver
  server.begin();
}

// Check if a reboot request is pending
void reboot_check() {
 if(timeout_reboot > 0 && timeout_reboot < millis()) {
  timeout_reboot = 0;
  console_log("\n<REBOOT>\n\n");
  delay(500);
  ESP.restart();
 }
}

// Send a result to browser as json (no-op if there is no active request, e.g. when called from the polling loop)
void send_result_json(SidekickRequest request, String result) {
  if(!request) return;
  req_send(request, 200, "application/json", "{ \"result\": \"" + result + "\"}");
}

// Webserver setup
void server_setup() {
  const char* headerkeys[] = {"Cookie"};
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
  console_log("--- Server setup\n");
  initCookies();
  #ifdef ESP8266
    // ESP8266WebServer discards most headers by default, must opt-in to keep the session cookie.
    // ESPAsyncWebServer (ESP32) always keeps every parsed request header, no equivalent call needed.
    server.collectHeaders(headerkeys, headerkeyssize);
  #endif
  configServerInit();
  Serial.println("Server setup... [OK]");
}

// Webserver loop
void server_loop() {
  #ifdef ESP8266
    // ESPAsyncWebServer (ESP32) handles clients in the background, no manual pump needed.
    server.handleClient();
  #endif
  // check pressed buttons
  button_check();
  // check release relay timeouts
  relay_check();
  // check pending reboot request
  reboot_check();
  // pump the Console UART<->WebSocket bridge (ESP32 only)
  hostconsole_loop();
}
