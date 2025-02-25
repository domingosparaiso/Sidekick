// int inputStates[] = { -1, -1, -1, -1 };

String inputStatesString() {
  String result = "";
  for(int i=0; i<MAX_BUTTONS; i++) {
    int valorDigital = digitalRead(CFG.data.digitalInput[i]);
    result += String((valorDigital==1)?'0':'1');
  }
  return(result);
}

void operacao_setup() {
  Serial.begin(115200);
  wifi_connect();
  Serial.println("WIFI: OK");
  server_setup();
  websocket_setup();
  Serial.println("Services: OK");
  lastCheckList = digitalRead(CFG.data.digitalInput[2]);
}

void operacao_loop() {
  check_wifi();
  server_loop();
  websocket_loop();
  client_loop();
  delay(10);
}
