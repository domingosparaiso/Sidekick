#include "html.h"
#define MAX_BUFFER 20480
/*
    Please provide the definition for the Soft AP SSID and Password.
    The password should consist of at least 8 characters.
    Alternatively, if no password is specified, the network will be open.
*/
// Manual IP Configuration for Soft AP
IPAddress AP_LOCAL_IP(192, 168, 100, 1);
IPAddress AP_GATEWAY_IP(192, 168, 100, 1);
IPAddress AP_NETWORK_MASK(255, 255, 255, 0);
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
  long start_time = millis();
  int stat_led = LOW;
  size_t fsize = UPDATE_SIZE_UNKNOWN;
  if (server.hasArg("size")) {
    fsize = server.arg("size").toInt();
  }
  HTTPUpload &upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
      setLED(LED_CONFIG, HIGH);
      setLED(LED_STATUS, HIGH);
    if (!Update.begin(fsize)) {
      otaDone = 0;
      setLED(LED_CONFIG, LOW);
      setLED(LED_STATUS, HIGH);
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
      setLED(LED_CONFIG, LOW);
      setLED(LED_STATUS, HIGH);
    } else {
      otaDone = 100 * Update.progress() / Update.size();
      if((millis() - start_time) > 500) {
        setLED(LED_STATUS, stat_led);
        stat_led=(stat_led==HIGH)?LOW:HIGH;
        setLED(LED_CONFIG, stat_led);
        start_time = millis();
      }
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (Update.end(true)) {
      setLED(LED_STATUS, LOW);
      for(int c=0;c<40;c++) {
        setLED(LED_CONFIG, LOW);
        delay(25);
        setLED(LED_CONFIG, HIGH);
        delay(25);
      }
    } else {
      setLED(LED_CONFIG, LOW);
      setLED(LED_STATUS, HIGH);
      otaDone = 0;
    }
  }
}

void reconfigure()
{
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
    CFG.data.digitalInput[c-1] = (uint8_t) server.arg(String("digitalInput_")+seq).toInt(); // Entradas digitais
    CFG.data.relayOutput[c-1] = (uint8_t) server.arg(String("relayOutput_")+seq).toInt();   // Relés
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

void configServerInit() {
  // Página para upload que inicializa da atualização
  server.on("/upload", HTTP_GET, []() {
    server.send(200, "text/html", uploadHtml);
  });

  // Reiniciar o dispositivo sem alterações
  server.on("/reboot", HTTP_GET, []() {
    server.send(200, "text/html", "<html><body><h1>Reboot in progress...</h1></body></html>");
    ESP.restart();
  });

  // Enviar a informação sobre os botões pressionados
  server.on("/buttons", HTTP_GET, []() {
    char result[6] = "";
    for(int c=0;c<4;c++) {
      result[c]=(digitalRead(CFG.data.digitalInput[c])==1)?'0':'1';
    }
    result[4]=(digitalRead(CONFIG_PORT)==1)?'0':'1';
    server.send(200, "text/html", result);
  });

  // Testar os LEDs STA e CFG
  server.on(UriBraces("/test_led/{}"), []() {
    String ledstr = server.pathArg(0);
    if(ledstr == "1") chgLED(LED_STATUS);
    if(ledstr == "2") chgLED(LED_CONFIG);
    char ledst = (LEDs[LED_STATUS] == HIGH)?'1':'0';
    char ledcf = (LEDs[LED_CONFIG] == HIGH)?'1':'0';
    char result[3] = "--";
    result[0] = ledst;
    result[1] = ledcf;
    server.send(200, "text/plain", result);
  });

  // Testar os relés
  server.on(UriBraces("/test_relay/{}"), []() {
    String relaystr = server.pathArg(0);
    int relaynum = relaystr.toInt();
    if(relaynum >=1 && relaynum <= MAX_RELAYS) {
      pulse_relay(relaynum, RELAY_PULSE_ON);
    }
    server.send(200, "text/plain", "");
  });

  server.on("/config", HTTP_POST, reconfigure);

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
                        "\"digitalInput\": [\"" +    String(CFG.data.digitalInput[0]) +   "\"," +
                                           "\"" +    String(CFG.data.digitalInput[1]) +   "\"," +
                                           "\"" +    String(CFG.data.digitalInput[2]) +   "\"," +
                                           "\"" +    String(CFG.data.digitalInput[3]) +   "\"]," +
                        "\"relayOutput\": [\"" +     String(CFG.data.relayOutput[0]) +    "\"," +
                                          "\"" +     String(CFG.data.relayOutput[1]) +    "\"," +
                                          "\"" +     String(CFG.data.relayOutput[2]) +    "\"," +
                                          "\"" +     String(CFG.data.relayOutput[3]) +    "\"]," +
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
    if(SPIFFS.format()) {
      server.send(200, "text/html", "<html><body>Armazenamento Flash formatado.<hr><a href=/home>Home</a></body></html>");
    } else {
      server.send(200, "text/html", "<html><body>Falha ao formatar o armazenamento Flash.<hr><a href=/home>Home</a></body></html>");
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

  server.on("/power.html", [] () { server.send(200, "text/html", "<html><body>Reinicie para acessar a p&aacute;gina principal</body></html>"); });
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

void config_setup() {
    setLED(LED_CONFIG, HIGH);
    if(strlen(CFG.data.AP.SSID)==0 || strlen(CFG.data.AP.password)==0) {
      strcpy(CFG.data.AP.SSID,"CONFIG_INIT");
      strcpy(CFG.data.AP.password, "password");
    }

    #ifdef DEVWIFI
    // Rede local para desenvolvimento
    WiFi.begin("SOTELE#2Ghz", "mamusKa#76");

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }

    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    #else
    WiFi.softAPConfig(AP_LOCAL_IP, AP_GATEWAY_IP, AP_NETWORK_MASK);

    WiFi.softAPsetHostname(CFG.data.AP.SSID);

    Serial.print("SSID: ");
    Serial.println(CFG.data.AP.SSID);
    Serial.print("Password: ");
    Serial.println(CFG.data.AP.password);

    // To initiate the Soft AP, pause the program if the initialization process encounters an error.
    if (!WiFi.softAP(CFG.data.AP.SSID, CFG.data.AP.password))
    {
        setLED(LED_STATUS, LOW);
        setLED(LED_CONFIG, HIGH);
        while (1) {
          chgLED(LED_STATUS);
          chgLED(LED_CONFIG);
          delay(500);
          chgLED(LED_STATUS);
          chgLED(LED_CONFIG);
          delay(1000);
        }
    }
    Serial.println("WIFI: OK");
    #endif
    configServerInit();
}

void config_loop() {
  server.handleClient();
  server_check_relay();
}