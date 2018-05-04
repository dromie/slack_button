#include <ESP8266WiFiMulti.h>

bool sent = false;
bool portal = false;
int tries = 0;

ESP8266WiFiMulti wifiMulti;
int last_serial = 0;

void setup()
{
  Serial.begin(115200);
  Serial.flush();
  Serial.println();
  loadCredentials();
  last_serial = getCredentialsSerial();
  for (int i = 0; i < MAX_SSID; ++i) {
    if (SSID[i].length() > 0) {
      wifiMulti.addAP(SSID[i].c_str(), PASSWORD[i].c_str());
    }
  }
}

void businessLoop() {
  wl_status_t status = wifiMulti.run();
  if (status == WL_NO_SSID_AVAIL && WiFi.scanComplete() == WIFI_SCAN_RUNNING) {
    Serial.println("Scanning");
    delay(1000);
  } else {
    if (status != WL_CONNECTED) {
      if (tries > 5) {
        Serial.println("Connect failed....Giving up....Starting portal");
        portal = true;
      } else {
        ++tries;
        delay(1000);
      }
    } else if (!sent && WiFi.status() == WL_CONNECTED) {
      sent = postMessageToSlack(slack_message);
      Serial.println("Sent:" + sent);
      delay(1000);
    }
  }
}


void loop()
{
  serialTerminalLoop();
  if (portal) {
    portalLoop();
  } else {
    businessLoop();
  }
}

