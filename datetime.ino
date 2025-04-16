#include "Sidekick.h"
#include "wificonfig.h"
#include <ESPDateTime.h>

void datetime_init() {
  String ntpservers[] = { NTPSERVERS };
  char ntpserver[128] = "";
  bool ntp_ok = false;
  int count_ntps = (sizeof(ntpservers)/sizeof(ntpservers[0]));
  int i = 0;
  if(wifi_mode == WIFI_MODE_AP) {
    console_log("Can't get datetime in AP Mode\n");
    return;
  }
  console_log("Timezone: ");
  console_log(TIMEZONE);
  while(!ntp_ok && i<count_ntps) {
    DateTime.setTimeZone(TIMEZONE);
    console_log("\nNTP server: ");
    console_log(ntpservers[i]);
    ntpservers[i].toCharArray(ntpserver, 128);
    DateTime.setServer(ntpserver);
    DateTime.begin();
    if (DateTime.isTimeValid()) {
      console_log(" [OK]");
      ntp_ok = true;
    } else {
      console_log(" [Fail]");
    }
    i++;
  }
  console_log("\nDatetime...");
  if(!ntp_ok) {
    console_log(" [Error]\n");
  } else {
    console_log(" [OK]\n");
    console_log("Date/Time: ");
    console_log(date_get());
    console_log(" ");
    console_log(time_get());
    console_log("\n");
  }
}

String date_get() {
  return(String(DateTime.format(DATE_FORMAT)));
}

String time_get() {
  return(String(DateTime.format(TIME_FORMAT)));
}