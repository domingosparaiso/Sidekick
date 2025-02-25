long relay_millis[] = { 0, 0, 0, 0 };

void pulse_relay(int relayNum, int relayStatus) {
  if(relayStatus == RELAY_PULSE_ON) {
    relay_millis[relayNum-1] = millis() + TIMEOUT_RELAY_ON;
  } else {
    relay_millis[relayNum-1] = millis() + TIMEOUT_RELAY_OFF;
  }
  digitalWrite(CFG.data.relayOutput[relayNum-1], HIGH);
}

void server_check_relay() {
  for(int i=0; i<MAX_RELAYS; i++) {
    if(relay_millis[i] > 0) {
      if((millis() > relay_millis[i])) {
        relay_millis[i] = 0;
        digitalWrite(CFG.data.relayOutput[i], LOW);
      }
    }
  }
}

void relayChange(int relayNum, int relayStatus) {
  if(relayStatus == RELAY_PULSE_ON || relayStatus == RELAY_PULSE_OFF) {
    pulse_relay(relayNum, relayStatus);
  } else {
    digitalWrite(CFG.data.relayOutput[relayNum-1], (relayStatus==RELAY_ON)?HIGH:LOW);
  }
  server.send(200, "plain/text", "");
}

void server_setup() {
  server.on("/relay/1/ON", [] () { relayChange(1, RELAY_ON); });
  server.on("/relay/2/ON", [] () { relayChange(2, RELAY_ON); });
  server.on("/relay/3/ON", [] () { relayChange(3, RELAY_ON); });
  server.on("/relay/4/ON", [] () { relayChange(4, RELAY_ON); });
  server.on("/relay/1/OFF", [] () { relayChange(1, RELAY_OFF); });
  server.on("/relay/2/OFF", [] () { relayChange(2, RELAY_OFF); });
  server.on("/relay/3/OFF", [] () { relayChange(3, RELAY_OFF); });
  server.on("/relay/4/OFF", [] () { relayChange(4, RELAY_OFF); });
  server.on("/relay/1/PULSE_ON", [] () { relayChange(1, RELAY_PULSE_ON); });
  server.on("/relay/2/PULSE_ON", [] () { relayChange(2, RELAY_PULSE_ON); });
  server.on("/relay/3/PULSE_ON", [] () { relayChange(3, RELAY_PULSE_ON); });
  server.on("/relay/4/PULSE_ON", [] () { relayChange(4, RELAY_PULSE_ON); });
  server.on("/relay/1/PULSE_OFF", [] () { relayChange(1, RELAY_PULSE_OFF); });
  server.on("/relay/2/PULSE_OFF", [] () { relayChange(2, RELAY_PULSE_OFF); });
  server.on("/relay/3/PULSE_OFF", [] () { relayChange(3, RELAY_PULSE_OFF); });
  server.on("/relay/4/PULSE_OFF", [] () { relayChange(4, RELAY_PULSE_OFF); });
  server.on("/inputs", [] () {
    String estadoAtual = inputStatesString();
    server.send(200, "application/json", "{ \"inputs\": \"" + estadoAtual + "\" }");
  });
  server.on("/version", [] () {
    String estadoAtual = inputStatesString();
    server.send(200, "application/json", "{ \"serialNumber\": \"" + serialNumber + "\", \"version\": \"" + String(VERSAO) + "\", \"inputs\": \"" + estadoAtual + "\" }");
  });
  server.on("/setup.html", [] () { server.send(200, "text/html", "<html><body>Reinicie no modo de configura&ccedil;&atilde;o</body></html>"); });
  server.serveStatic("/", SPIFFS, "/");
  server.on("/", [] () { 
    String indexHtmlFS = "<html><body>Arquivo n&atilde;o encontrado, reconfigure o dispositivo</body></html>";
    if(SPIFFS.exists("/power.html")) {
      File storage = SPIFFS.open("/power.html", "r");
      if(storage) {
        indexHtmlFS = storage.readString();
        storage.close();
      }
    }
    server.send(200, "text/html", indexHtmlFS);
  });
  server.onNotFound([]() { 
    String indexHtmlFS = "<html><body>Arquivo n&atilde;o encontrado, reconfigure o dispositivo</body></html>";
    if(SPIFFS.exists("/power.html")) {
      File storage = SPIFFS.open("/power.html", "r");
      if(storage) {
        indexHtmlFS = storage.readString();
        storage.close();
      }
    }
    server.send(200, "text/html", indexHtmlFS);
  });  
  server.enableCORS(true);
  server.begin();
}

void server_check_cfg() {
  if(digitalRead(CONFIG_PORT) == 0) {
    long start_time = millis();
    while(digitalRead(CONFIG_PORT) == 0 && (millis() - start_time) < (1000 * CONFIG_TIMEOUT)) {
      chgLED(LED_CONFIG);
      delay(100);
    }
    if(digitalRead(CONFIG_PORT) == 0) {
      File storage = SPIFFS.open("/reconfigure.reset", "w");
      if (storage) {
        storage.print("reconfig");
      }
      storage.close();
      ESP.restart();
    }
    setLED(LED_CONFIG, LOW);
  }
}

void server_loop() {
  server.handleClient();
  server_check_relay();
  server_check_cfg();
}