void inicializa_flash() {
  if(!SPIFFS.begin(true)){
    SPIFFS.format();
  }
}

void save_CFG() {
  File storage = SPIFFS.open("/config.bin", "wb");
  if (storage) {
    storage.write(CFG.raw, sizeof(config_data));
    configurePorts();
  }
  storage.close();
}

void configurePorts() {
  int pin = 0;
  // Reconfigurando as entradas e saídas
  for(int c=0; c<MAX_BUTTONS; c++) {
    pin = CFG.data.digitalInput[c];
    if(pin > 0) {

      // Enable PullUp
      // pinMode(pin, INPUT_PULLUP);

      // Disable PullUp
      pinMode(pin, INPUT);
      digitalWrite(pin, LOW);
    }
  }
  for(int c=0; c<MAX_RELAYS; c++) {
    pin = CFG.data.relayOutput[c];
    if(pin > 0) pinMode(pin, OUTPUT);
  }
  delay(100);
}

void load_CFG() {
  //int eeAddress = 0;
  File storage = SPIFFS.open("/config.bin", "rb");
  if (storage) {
    uint32_t nBytes = storage.readBytes((char*)CFG.raw, sizeof(config_data));
    configurePorts();
  }
  storage.close();
}

String getFile(String filename) {
  File storage = SPIFFS.open(filename, "r");
  String contents = "";
  while (storage.available()){
    contents += char(storage.read());
  }
  storage.close();
  return(contents);
}