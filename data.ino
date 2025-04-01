#include "Sidekick.h"
#include "data.h"

// Inicializa a flash para armazenar arquivos
void inicializa_flash() {
  if(!SPIFFS.begin(true)){
    SPIFFS.format();
  }
}

// Salva as configurações feitas no arquivo de configuração
void save_CFG() {
  File storage = SPIFFS.open("/config.bin", "wb");
  if (storage) {
    storage.write(CFG.raw, sizeof(config_data));
  }
  storage.close();
}

// Le arquivo de configuração e coloca os valore na estrutura de dados
void load_CFG() {
  File storage = SPIFFS.open("/config.bin", "rb");
  if (storage) {
    uint32_t nBytes = storage.readBytes((char*)CFG.raw, sizeof(config_data));
  }
  storage.close();
}

// Retorna o conteúdo de um arquivo
String getFile(String filename) {
  File storage = SPIFFS.open(filename, "r");
  String contents = "";
  while (storage.available()){
    contents += char(storage.read());
  }
  storage.close();
  return(contents);
}