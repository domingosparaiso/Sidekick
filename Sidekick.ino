#include <EEPROM.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <Update.h>
#include <uri/UriBraces.h>
#include "FS.h"
#include "SPIFFS.h"
//#include <Ticker.h>
// tkSecond.attach(1, everySecond);
#include "config.h"
#include "data.h"
#include "Sidekick.h"

int MODE = MODO_OPERACAO;
WebServer server(80);
WiFiServer telnetServer(23);
int LEDp[2] = { PIN_LED_CONFIG, PIN_LED_STATUS };
int LEDs[2] = { LOW, LOW };
int SIZE_config_data;
union config_union CFG;
String serialNumber = "";

void chgLED(int ledNum) {
  setLED(ledNum, LEDs[ledNum]==HIGH?LOW:HIGH);
}

void setLED(int ledNum, int valor) {
  if(valor == HIGH || valor == LOW) {
    digitalWrite(LEDp[ledNum], valor);
    LEDs[ledNum] = valor;
  }
}

void setup() {
  // Botão de reconfiguração
  pinMode(CONFIG_PORT, INPUT_PULLUP);
  // LEDs Status e Configuração
  pinMode(LEDp[LED_STATUS], OUTPUT);
  setLED(LED_STATUS, LOW);
  pinMode(LEDp[LED_CONFIG], OUTPUT);
  setLED(LED_CONFIG, LOW);

  Serial.begin(115200);
  Serial.println("\nPower ON");
  long unsigned int espmac = ESP.getEfuseMac() >> 24;
  serialNumber = String(espmac, HEX);
  serialNumber.toUpperCase();
  Serial.println(String("Serial: ") + serialNumber);
  
  inicializa_flash();
  load_CFG();

  int reconfig = 0;
  if(SPIFFS.exists("/reconfigure.reset")) {
    reconfig = 1;
    SPIFFS.remove("/reconfigure.reset");
  } else {
    long start_time = millis();
    // Se o botão de configuração está pressionado, aguenta pelo TIMEOUT para entrar em modo de configuração
    while(digitalRead(CONFIG_PORT) == 0 && (millis() - start_time) < (1000 * CONFIG_TIMEOUT)) {
      chgLED(LED_CONFIG);
      delay(100);
    }
    if(digitalRead(CONFIG_PORT) == 0) reconfig = 1;
  }

  // Se o botão ficou pressionado, ativa configuração
  if(reconfig) {
    setLED(LED_CONFIG, HIGH);
    setLED(LED_STATUS, LOW);
    MODE = MODO_CONFIG;
    Serial.println("CONFIG MODE");
    config_setup();
  } else {
    setLED(LED_CONFIG, LOW);
    setLED(LED_STATUS, HIGH);
    Serial.println("START");
    operacao_setup();
    telnetServer.begin();
  }
}

void loop() {
  switch(MODE) {
    case MODO_CONFIG:
      config_loop();
      break;
    case MODO_OPERACAO:
      operacao_loop();
      telnet_loop();
      break;
  }
}

void telnet_loop() {
  WiFiClient telnetClient = telnetServer.available();
  if (telnetClient) { 
    while (telnetClient.connected()) {
      while (telnetClient.available()>0) {
        char c = telnetClient.read();
        Serial.write(c);
      }
      if(Serial.available()>0) {
        char c = Serial.read();
        telnetClient.write(c);
      }
      delay(10);
    }
    telnetClient.stop();
  }
}