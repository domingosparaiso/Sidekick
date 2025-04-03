#include "Sidekick.h"
#include "html.h"

uint8_t otaDone = 0;
File uploadFile;

void handleUpdateEnd() {
  server.sendHeader("Connection", "close");
  if (Update.hasError()) {
    server.send(502, "text/plain", Update.errorString());
  } else {
    server.sendHeader("Refresh", "10");
    server.sendHeader("Location", "/");
    server.send(307);
    ESP.restart();
  }
}

void handleUpdate() {
  size_t fsize = UPDATE_SIZE_UNKNOWN;
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

void reconfigure() {
  server.arg("serverName").toCharArray(CFG.data.serverName, 32);
  server.arg("password").toCharArray(CFG.data.password, 32);
  server.arg("AP_SSID").toCharArray(CFG.data.AP.SSID,32);
  server.arg("AP_password").toCharArray(CFG.data.AP.password,32);
  server.arg("CLI_wifi_SSID").toCharArray(CFG.data.CLI.wifi.SSID, 32);
  server.arg("CLI_wifi_password").toCharArray(CFG.data.CLI.wifi.password, 32);
  CFG.data.CLI.DHCP = (server.arg("CLI_DHCP")=="dhcp");
  for(int c=1; c<=4; c++) {
    String seq = String(c);
    if(!CFG.data.CLI.DHCP) {
      CFG.data.CLI.IP[c-1] = (uint8_t) server.arg(String("CLI_IP_")+seq).toInt(); //endereço IP
      CFG.data.CLI.MASK[c-1] = (uint8_t) server.arg(String("CLI_MASK_")+seq).toInt(); //mascara subrede
      CFG.data.CLI.GW[c-1] = (uint8_t) server.arg(String("CLI_GW_")+seq).toInt(); //gateway
      CFG.data.CLI.DNS[c-1] = (uint8_t) server.arg(String("CLI_DNS_")+seq).toInt(); //dns
    }
  }
  save_CFG();
  server.send(200, "text/html", "");
}

void handleFileUpload() {
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    // Make sure paths always start with "/"
    if (!filename.startsWith("/")) { filename = "/" + filename; }
    //uploadFile = fileSystem->open(filename, "w");
    if(filename == "/config.ini") return;
    uploadFile = SPIFFS.open((String("/") + upload.filename).c_str(), FILE_WRITE); 
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

String charToStr(uint8_t opt) {
  String tabela = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  return(tabela.substring(opt, opt+1));
}

void configServerInit() {

  // Página para upload que inicializa da atualização
  server.on("/upload", HTTP_GET, []() {
    server.send(200, "text/html", uploadHtml);
  });

  // Reiniciar o dispositivo sem alterações
  server.on("/reboot", HTTP_GET, []() {
    server.send(200, "text/html", "<html><body><h1>Reboot in progress...</h1></body></html>");
    display_status(STATUS_REBOOT);
    ESP.restart();
  });

  server.on("/config", HTTP_POST, reconfigure);

  button_register();
  led_register();
  relay_register();
  rpm_register();
  temperature_register();

  server.on("/resources.json", []() {
    server.send(200, "application/json", resourcesJson);
  });
 
  server.on("/config.json", []() {
    String DHCPcfg = (CFG.data.CLI.DHCP)?String("dhcp"):String("fixo");
    String result =   "{ \"serverName\": \"" +       String(CFG.data.serverName) +         "\"," +
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
                        "\"version\": \"" +          String(VERSAO) +                     "\"," +
                        "\"serialNumber\": \"" +     serialNumber +                       "\" }";
    server.send(200, "application/json", result);
  });

  // Realiza a atualização do firmware
  server.on(
    "/update", HTTP_POST,
    []() {
      handleUpdateEnd();
    },
    []() {
      display_status(STATUS_UPDATE_FIRMWARE);
      handleUpdate();
    }
  );

  // apagar arquivos do SPIFFS
  server.on("/delete", []() {
    for (uint8_t i = 0; i < server.args(); i++) {
      if(String(server.argName(i)) == "arq") {
        String fname = String(server.arg(i));
        if(!fname.startsWith("/")) {
          fname = "/" + fname;
        }
        if(fname != "/config.ini") {
          SPIFFS.remove(fname);
        }
      }
    }
    server.send(200, "text/plain", "");
  });

  // formatar o SPIFFS
  server.on("/format", []() {
    display_status(STATUS_FORMAT_FS);
    if(SPIFFS.format()) {
      server.send(200, "text/html", "<html><body>Armazenamento Flash formatado.<hr><a href=/home>Home</a></body></html>");
      display_status(STATUS_FORMAT_OK);
    } else {
      server.send(200, "text/html", "<html><body>Falha ao formatar o armazenamento Flash.<hr><a href=/home>Home</a></body></html>");
      display_status(STATUS_FORMAT_ERROR);
    }
  });

  // gerenciamento de arquivos
  server.on("/fs", []() {
    String fsIndex = String(fileHtml);
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    String result = "<table>";
    while (file) {
      if(String(file.name()) != "config.ini") {
        result += String("<tr><td class='fdel' onclick='fdel(\"" + String(file.name()) + "\")'>[del]</td><td>") + String(file.name()) + String("</td><td>") + String(file.size()) + String("</td></tr>");
      }
      file = root.openNextFile();
    }
    result += "</table></body></html>";
    server.send(200, "text/html", fsIndex + result);
  });

  // upload de arquivos
  server.on("/fs-upload", HTTP_POST, []() {
      server.sendHeader("Connection", "close");
      server.send(200, "text/plain", "OK");
    }
    , handleFileUpload);

  // arquivos salvos no SPIFFS
  server.serveStatic("/", SPIFFS, "/");

  // url não encontrada, abre o setup.html, se não existir, mostra um link para gerenciador de arquivos
  server.onNotFound([]() {
    String indexHtmlFS = "<html><body><h1>Arquivo 'setup.html' n&atilde;o encontrado.</h1><hr><a href=/fs>Arquivos</a></body></html>";
    if(SPIFFS.exists("/setup.html")) {
      File storage = SPIFFS.open("/setup.html", "r");
      if(storage) {
        indexHtmlFS = storage.readString();
        storage.close();
      }
    }
    server.send(200, "text/html", indexHtmlFS);
  });

  // enable CORS header in webserver results
  server.enableCORS(true);

  // inicia o servidor web
  server.begin();
}

void send_result_json(String result) {
  server.send(200, "application/json", "{ \"result\": \"" + result + "\"}");
}

void server_setup() {
  Serial.println("Setup do servidor");
  configServerInit();
  Serial.println("Setup do servidor... [OK]");
}

void server_loop() {
  server.handleClient();
  button_check();
  relay_check();
  delay(200);
}