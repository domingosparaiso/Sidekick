void wifi_connect() {
  setLED(LED_CONFIG, LOW);
  setLED(LED_STATUS, HIGH);
  WiFi.begin(CFG.data.CLI.wifi.SSID, CFG.data.CLI.wifi.password);
  while (WiFi.status() != WL_CONNECTED) {
    chgLED(LED_STATUS);
    delay(500);
  }
  setLED(LED_STATUS, HIGH);  
  if(!CFG.data.CLI.DHCP) {
    IPAddress ip(CFG.data.CLI.IP[0], CFG.data.CLI.IP[1], CFG.data.CLI.IP[2], CFG.data.CLI.IP[3]);
    IPAddress gateway(CFG.data.CLI.GW[0], CFG.data.CLI.GW[1], CFG.data.CLI.GW[2], CFG.data.CLI.GW[3]);
    IPAddress subnet(CFG.data.CLI.MASK[0], CFG.data.CLI.MASK[1], CFG.data.CLI.MASK[2], CFG.data.CLI.MASK[3]);
    IPAddress dns(CFG.data.CLI.DNS[0], CFG.data.CLI.DNS[1], CFG.data.CLI.DNS[2], CFG.data.CLI.DNS[3]);
    WiFi.config(ip, dns, gateway, subnet);
  }
  /*
  String myIP = WiFi.localIP().toString();
  File storage = SPIFFS.open("/ip.txt", "w");
  if (storage) {
    storage.print(myIP);
  }
  storage.close();
  */
}

void check_wifi() {
  if(WiFi.status() != WL_CONNECTED) wifi_connect();
}