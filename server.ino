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

void handleUpdateEnd() {
  server.sendHeader("Connection", "close");
  if (Update.hasError()) {
    server.send(502, "text/plain", "Update Error.");
  } else {
    server.sendHeader("Refresh", "10");
    server.sendHeader("Location", "/");
    server.send(307);
    ESP.restart();
  }
}

void handleUpdate() {
  if(!checkCookie()) return;
  size_t fsize = 0;
  if (server.hasArg("size")) {
    fsize = server.arg("size").toInt();
  }
  HTTPUpload &upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
      display_status(STATUS_UPLOAD_FILE_START);
    if (!Update.begin(fsize)) {
      otaDone = 0;
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
      display_status(STATUS_UPLOAD_FILE_RUN);
    } else {
      otaDone = 100 * Update.progress() / Update.size();
      display_print(2, 1, String(otaDone));
    }
  } else if (upload.status == UPLOAD_FILE_END) {
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

void reconfigure() {
  String seq;
  server.arg("userName").toCharArray(CFG.data.userName, 32);
  server.arg("hostname").toCharArray(CFG.data.hostname, 32);
  server.arg("password").toCharArray(CFG.data.password, 32);
  server.arg("AP_SSID").toCharArray(CFG.data.AP.SSID,32);
  server.arg("AP_password").toCharArray(CFG.data.AP.password,32);
  server.arg("CLI_wifi_SSID").toCharArray(CFG.data.CLI.wifi.SSID, 32);
  server.arg("CLI_wifi_password").toCharArray(CFG.data.CLI.wifi.password, 32);
  CFG.data.CLI.DHCP = (server.arg("CLI_DHCP")=="dhcp");
  if(!CFG.data.CLI.DHCP) {
    for(int c=1; c<=4; c++) {
      seq = String(c);
      CFG.data.CLI.IP[c-1] = (uint8_t) server.arg(String("CLI_IP_")+seq).toInt(); //endereço IP
      CFG.data.CLI.MASK[c-1] = (uint8_t) server.arg(String("CLI_MASK_")+seq).toInt(); //mascara subrede
      CFG.data.CLI.GW[c-1] = (uint8_t) server.arg(String("CLI_GW_")+seq).toInt(); //gateway
      CFG.data.CLI.DNS[c-1] = (uint8_t) server.arg(String("CLI_DNS_")+seq).toInt(); //dns
    }
  }
  for(int c=1; c<=5; c++) {
    CFG.data.maptemp[c-1] = 0;
    seq = String(c);
    if(server.arg("TMAP_"+seq).length() > 0) {
      CFG.data.maptemp[c-1] = server.arg("TMAP_"+seq).toInt();
    }
    
  }
  CFG.data.timeout_backlight = 0;
  if(server.arg("timeout").length() > 0) CFG.data.timeout_backlight = server.arg("timeout").toInt();
  save_CFG();
  server.send(200, "text/html", "<html><body style='margin:0;padding:0;border:0'><div id=m>Configuration Saved</div></body><script>setTimeout(()=>{getElementeById('m').innerHTML='';},10000);</script></html>");
}

void handleFileUpload() {
  if(!checkCookie()) return;
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    // Make sure paths always start with "/"
    if (!filename.startsWith("/")) { filename = "/" + filename; }
    //uploadFile = fileSystem->open(filename, "w");
    if(filename == "/config.bin") return;
    uploadFile = LittleFS.open((String("/") + upload.filename).c_str(), "w"); 
    if (!uploadFile) {
      return;
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (uploadFile) {
      //size_t bytesWritten = uploadFile.write(upload.buf, upload.currentSize);
      //if (bytesWritten != upload.currentSize) { return replyServerError(F("WRITE FAILED")); }
      if(uploadFile.write(upload.buf, upload.currentSize) != upload.currentSize) {
        //
      }
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (uploadFile) {
      uploadFile.close();
    }
  }
}

void configServerInit() {

  // Public login endpoints (must work without a session cookie)
  loginServerInit();

  // Main application page, gated behind a valid session cookie
  authOn("/", HTTP_GET, serveIndexPage);
  authOn("/index.html", HTTP_GET, serveIndexPage);

  // Never expose the raw config file (holds userName/password) over HTTP
  server.on("/config.bin", HTTP_GET, []() { server.send(403, "text/plain", "Forbidden"); });

  // Upload page to flash a new firmware
  authOn("/upload", HTTP_GET, []() {
    server.send(200, "text/html", uploadHtml);
  });

  // Reboot the device
  authOn("/reboot", HTTP_GET, []() {
    server.send(200, "text/html", "<html><body><h1>Reboot in progress...</h1></body></html>");
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

  // return a json with all resources
  authOn("/resources.json", HTTP_GET, []() {
    //console_log("GET> resources.json\n");
    String result = resourcesJson.substring(0, resourcesJson.length() - 1) + ",\"uptime\":\"" + uptime_string() + "\"}";
    server.send(200, "application/json", result);
  });

  // return a json with actual configuration
  authOn("/config.json", HTTP_GET, []() {
    server.send(200, "application/json", config_json());
  });

  // Firmware update
  server.on(
    "/update", HTTP_POST,
    []() {
      if(!checkCookie()) { serveLoginPage(); return; }
      handleUpdateEnd();
    },
    []() {
      display_status(STATUS_UPDATE_FIRMWARE);
      handleUpdate();
    }
  );

  // Delete files from storage
  authOn("/delete", HTTP_GET, []() {
    for (uint8_t i = 0; i < server.args(); i++) {
      if(String(server.argName(i)) == "arq") {
        String fname = String(server.arg(i));
        if(!fname.startsWith("/")) {
          fname = "/" + fname;
        }
        if(fname != "/config.bin") {
          LittleFS.remove(fname);
        }
      }
    }
    server.send(200, "text/plain", "");
  });

  // storage format
  authOn("/format", HTTP_GET, []() {
    display_status(STATUS_FORMAT_FS);
    console_log("<STORAGE FORMAT>");
    if(LittleFS.format()) {
      server.send(200, "text/html", "<html><head><link rel='stylesheet' href='style.css'></head><body>Flash storage formated.<hr><a href=/home>Home</a></body></html>");
      display_status(STATUS_FORMAT_OK);
    } else {
      server.send(200, "text/html", "<html><head><link rel='stylesheet' href='style.css'></head><body>Error at Flash storage format.<hr><a href=/home>Home</a></body></html>");
      display_status(STATUS_FORMAT_ERROR);
    }
  });

  // Storage file management
  authOn("/fs", HTTP_GET, []() {
    String fsIndex = String(fileHtml);
    String result = "<table>";
    #ifdef ESP8266
      Dir root = LittleFS.openDir("/");
      while (root.next()) {
        File file = root.openFile("r");
        if(String(root.fileName()) != "config.bin") {
          result += String("<tr><td class='fdel' onclick='fdel(\"" + String(root.fileName()) + "\")'>[del]</td><td>") + String(root.fileName()) + String("</td><td>") + String(file.size()) + String("</td></tr>");
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
            result += String("<tr><td class='fdel' onclick='fdel(\"" + String(file.name()) + "\")'>[del]</td><td>") + String(file.name()) + String("</td><td>") + String(file.size()) + String("</td></tr>");
          }
          file = root.openNextFile();
        }
      }
    #endif
    result += "</table></div></body></html>";
    server.send(200, "text/html", fsIndex + result);
  });

  // File upload
  server.on("/fs-upload", HTTP_POST, []() {
      if(!checkCookie()) { serveLoginPage(); return; }
      server.sendHeader("Connection", "close");
      server.send(200, "text/plain", "OK");
    }
    , handleFileUpload);

  // Retrieve files from storage
  server.serveStatic("/", LittleFS, "/");

  // url not found, open index.html instead send a 404 error, if index.html doesn't exists, send a message to upload one
  server.onNotFound([]() {
    if(!checkCookie()) { serveLoginPage(); return; }
    serveIndexPage();
  });

  // enable CORS header in webserver results
  server.enableCORS(true);

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

// Send a result to browser as json
void send_result_json(String result) {
  server.send(200, "application/json", "{ \"result\": \"" + result + "\"}");
}

// Webserver setup
void server_setup() {
  const char* headerkeys[] = {"Cookie"};
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);  
  console_log("--- Server setup\n");
  initCookies();
  server.collectHeaders(headerkeys, headerkeyssize);  
  configServerInit();
  Serial.println("Server setup... [OK]");
}

// Webserver loop
void server_loop() {
  // handle webserver clients
  server.handleClient();
  // check pressed buttons
  button_check();
  // check release relay timeouts
  relay_check();
  // check pending reboot request
  reboot_check();
}