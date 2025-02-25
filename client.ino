String lastInputDigital = "";
int lastCheckList = -1;

// Informa via websocket que uma entrada digital mudou
void client_loop() {
  String estadoAtual = inputStatesString();
  int valCheckList;
  if(!estadoAtual.equals(lastInputDigital)) {
    lastInputDigital = estadoAtual;
    // send Digital Input
    sendWebsocketStatus(estadoAtual);
  }
}
