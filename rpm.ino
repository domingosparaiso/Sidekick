#include "Sidekick.h"
// RPM control
// array [ CPU, SYS#1, SYS#2, SYS#3, SYS#4 ]
#include <Ticker.h>

Ticker tickerSecond;

#ifdef RPM_CPU_PIN
volatile rpm_data FAN_CPU;
void IRAM_ATTR handle_RPM_CPU() { FAN_CPU.counter++; }
#endif
#ifdef RPM_SYS1_PIN
volatile rpm_data FAN_SYS1;
void IRAM_ATTR handle_RPM_SYS1() { FAN_SYS1.counter++; }
#endif
#ifdef RPM_SYS2_PIN
volatile rpm_data FAN_SYS2;
void IRAM_ATTR handle_RPM_SYS2() { FAN_SYS2.counter++; }
#endif
#ifdef RPM_SYS3_PIN
volatile rpm_data FAN_SYS3;
void IRAM_ATTR handle_RPM_SYS3() { FAN_SYS3.counter++; }
#endif
#ifdef RPM_SYS4_PIN
volatile rpm_data FAN_SYS4;
void IRAM_ATTR handle_RPM_SYS4() { FAN_SYS4.counter++; }
#endif

void rpm_init() {
  console_log("RPM init: ");
  tickerSecond.attach(1, updateRPMs);  
  #ifdef RPM_CPU_PIN
    resourcesAddItem("cpu", RPM_CPU_PIN);
    pinMode(RPM_CPU_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(RPM_CPU_PIN), handle_RPM_CPU, CHANGE);
    FAN_CPU.millis = millis();
  #endif
  #ifdef RPM_SYS1_PIN
    resourcesAddItem("sys1", RPM_SYS1_PIN);
    pinMode(RPM_SYS1_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(RPM_SYS1_PIN), handle_RPM_SYS1, CHANGE);
    FAN_SYS1.millis = millis();
  #endif
  #ifdef RPM_SYS2_PIN
    resourcesAddItem("sys2", RPM_SYS2_PIN);
    pinMode(RPM_SYS2_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(RPM_SYS2_PIN), handle_RPM_SYS2, CHANGE);
    FAN_SYS2.millis = millis();
  #endif
  #ifdef RPM_SYS3_PIN
    resourcesAddItem("sys3", RPM_SYS3_PIN);
    pinMode(RPM_SYS3_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(RPM_SYS3_PIN), handle_RPM_SYS3, CHANGE);
    FAN_SYS3.millis = millis();
  #endif
  #ifdef RPM_SYS4_PIN
    resourcesAddItem("sys4", RPM_SYS4_PIN);
    pinMode(RPM_SYS4_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(RPM_SYS4_PIN), handle_RPM_SYS4, CHANGE);
    FAN_SYS4.millis = millis();
  #endif
  resourcesAddArray("rpm");
}

void update_rpm(volatile rpm_data& FAN, long millis_count){
  long counter;
  long tempo;
  counter = FAN.counter;
  FAN.counter = 0;
  tempo = millis_count - FAN.millis;
  FAN.millis = 0;
  if(tempo > 0) FAN.rpm = (counter/tempo);
}

void updateRPMs() {
  long millis_count = millis();
  #ifdef RPM_CPU_PIN
    update_rpm(FAN_CPU, millis_count);
  #endif
  #ifdef RPM_SYS1_PIN
    update_rpm(FAN_SYS1, millis_count);
  #endif
  #ifdef RPM_SYS2_PIN
    update_rpm(FAN_SYS2, millis_count);
  #endif
  #ifdef RPM_SYS3_PIN
    update_rpm(FAN_SYS3, millis_count);
  #endif
  #ifdef RPM_SYS4_PIN
    update_rpm(FAN_SYS4, millis_count);
  #endif
}

void rpm_register() {
  #ifdef RPM_CPU_PIN
  server.on("/rpm/cpu", HTTP_GET, []() { server.send(200, "application/json", "{ \"rpm_cpu\": \"" + String(FAN_CPU.rpm) + "\"}"); });
  #endif  
  #ifdef RPM_SYS1_PIN
  server.on("/rpm/sys1", HTTP_GET, []() { server.send(200, "application/json", "{ \"rpm_sys1\": \"" + String(FAN_SYS1.rpm) + "\"}"); });
  #endif
  #ifdef RPM_SYS2_PIN
  server.on("/rpm/sys2", HTTP_GET, []() { server.send(200, "application/json", "{ \"rpm_sys2\": \"" + String(FAN_SYS2.rpm) + "\"}"); });
  #endif
  #ifdef RPM_SYS3_PIN
  server.on("/rpm/sys3", HTTP_GET, []() { server.send(200, "application/json", "{ \"rpm_sys3\": \"" + String(FAN_SYS3.rpm) + "\"}"); });
  #endif
  #ifdef RPM_SYS4_PIN
  server.on("/rpm/sys4", HTTP_GET, []() { server.send(200, "application/json", "{ \"rpm_sys4\": \"" + String(FAN_SYS4.rpm) + "\"}"); });
  #endif
}