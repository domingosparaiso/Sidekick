WebSocketsServer webSocket = WebSocketsServer(8000);
char buffer[64];

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    //case WStype_DISCONNECTED:
    //  break;
    case WStype_CONNECTED:
      {
//        String result = String("{\n  \"id\":") + serialNumber + String(",\n  \"ed\":\"") + inputStatesString() + String("\"\n}\n");
        String result = String("{\"inputs\":\"") + inputStatesString() + String("\"}");
        result.toCharArray(buffer, 64, 0);
        //IPAddress ip = webSocket.remoteIP(num);
        // send message to client
        webSocket.sendTXT(num, buffer);
      }
      break;
    //case WStype_TEXT:
      //USE_SERIAL.printf("[%u] get Text: %s\n", num, payload);
      // send message to client
      //webSocket.sendTXT(num, "message here");
      // send data to all connected clients
      // webSocket.broadcastTXT("message here");
    //  break;
    //case WStype_BIN:
      //USE_SERIAL.printf("[%u] get binary length: %u\n", num, length);
      //hexdump(payload, length);
      // send message to client
      // webSocket.sendBIN(num, payload, length);
    //  break;
		//case WStype_ERROR:			
		//case WStype_FRAGMENT_TEXT_START:
		//case WStype_FRAGMENT_BIN_START:
		//case WStype_FRAGMENT:
		//case WStype_FRAGMENT_FIN:
		//	break;
    }
}

void sendWebsocketStatus(String estadoAtual) {
  String result = String("{\n  \"inputs\":\"") + estadoAtual + String("\"\n}\n");
  result.toCharArray(buffer, 64, 0);
  webSocket.broadcastTXT(buffer);
}

void websocket_setup() {
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void websocket_loop() {
  webSocket.loop();
}