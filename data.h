struct wifi_config {
  char SSID[32];
  char password[32];
};

struct wifi_client {
  wifi_config wifi;
  bool DHCP;
  uint8_t IP[4];
  uint8_t MASK[4];
  uint8_t GW[4];
  uint8_t DNS[4];
};

struct config_data {
  char serverName[32];
  char password[32];
  wifi_config AP;
  wifi_client CLI;
  uint8_t digitalInput[4];
  uint8_t relayOutput[4];
};

union config_union {
  byte raw[sizeof(config_data)];
  config_data data;
};