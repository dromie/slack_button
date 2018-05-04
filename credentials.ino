#include <EEPROM.h>

#define MAX_SSID 16
/*
  WIFI CONFIGURATION
*/
String SSID[MAX_SSID]; //max 32
String PASSWORD[MAX_SSID]; // max 64

int eeLength = (32 + 1 + 64 + 1) * MAX_SSID + 3 * 2;

/** Load WLAN credentials from EEPROM */
void loadCredentials() {
  boolean failed = true;
  EEPROM.begin(eeLength);
  char ok[2 + 1];
  EEPROM.get(0, ok);
  if (String(ok) == String("OK")) {
    Serial.print("Header "); Serial.println(ok);

    int pos = 3;
    char ssid[33];
    char password[65];
    for (int i = 0; i < MAX_SSID; ++i) {
      EEPROM.get(pos, ssid);
      pos += 33;
      EEPROM.get(pos, password);
      pos += 65;
      SSID[i] = String(ssid);
      PASSWORD[i] = String(password);
    }
    EEPROM.get(pos, ok);
    if (String(ok) == String("KO")) {
      failed = false;
    }
  }
  if (failed) {
    Serial.println("Failed read!");
    clearAllCredentials();
  } else {
    Serial.print("Succesfull read!");
    Serial.println("Recovered credentials: ");
    listCredentials();
  }
  bumpSerial();
}

String NOPASSWORD = "*no password*";
String MASKEDPASSWORD = "********";

boolean isNoPassword(String s) {
  return (s==NOPASSWORD);
}

boolean isMaskedPassword(String s) {
  return (s==MASKEDPASSWORD);
}

String maskPassword(String pwd) {
  return pwd.length() > 0 ?  MASKEDPASSWORD: NOPASSWORD ;
}

void listCredentials() {
  for (int i = 0; i < MAX_SSID; ++i) {
    Serial.print("Slot "); Serial.print(i); Serial.print(": '");
    Serial.print(SSID[i]); Serial.print("':");
    Serial.println(maskPassword(PASSWORD[i]));
  }
}

/** Store WLAN credentials to EEPROM */
void saveCredentials() {
  EEPROM.begin(eeLength);
  int pos = 0;
  char ok[2 + 1] = "OK";
  char ko[2 + 1] = "KO";
  char ssid[33];
  char pwd[65];
  EEPROM.put(pos, ok);
  pos += 3;
  for (int i = 0; i < MAX_SSID; ++i) {
    SSID[i].toCharArray(ssid, 32);
    EEPROM.put(pos, ssid);
    pos += 33;
    PASSWORD[i].toCharArray(pwd, 64);
    EEPROM.put(pos, pwd);
    pos += 65;
  }
  EEPROM.put(pos, ko);
  EEPROM.commit();
  EEPROM.end();
}

void swapCredentials(int i, int j) {
  String tmp = SSID[i];
  SSID[i] = SSID[j];
  SSID[j] = tmp;
  tmp = PASSWORD[i];
  PASSWORD[i] = PASSWORD[j];
  PASSWORD[j] = tmp;
  bumpSerial();
}

void setCredentials(int index,  String ssid, String pwd) {
  if (index < MAX_SSID && index >= 0) {
    SSID[index] = ssid;
    PASSWORD[index] = pwd;
  }
  bumpSerial();
}

void clearCredentials(int index) {
  if (index < MAX_SSID && index >= 0) {
    SSID[index] = "";
    PASSWORD[index] = "";
  }
  bumpSerial();
}

void clearAllCredentials() {
  for (int i = 0; i < MAX_SSID; ++i) {
    SSID[i] = "";
    PASSWORD[i] = "";
  }
  bumpSerial();

}
void resetCredentials() {
  Serial.println("Clearing EEPROM!");
  EEPROM.begin(eeLength);
  for (int i = 0; i < eeLength; ++i) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  EEPROM.end();
  clearAllCredentials();
}

int serial = 0;

int getCredentialsSerial() {
  return serial;
}

void bumpSerial() {
  ++serial;
}

void test() {
  EEPROM.begin(eeLength);
  int pos = 0;
  for (byte i = 0; i < 100; ++i) {
    EEPROM.write(pos++, i);
  }
  String test = "test";
  char test2[16];
  test.toCharArray(test2, 15);
  Serial.println(test2);
  EEPROM.put(pos, test2);
  EEPROM.commit();
  EEPROM.end();


  for (int i = 0; i < 16; ++i) test2[i] = 0;

  EEPROM.begin(eeLength);
  pos = 0;
  byte c;
  for (byte i = 0; i < 100; ++i) {
    c = EEPROM.read(pos++);
    Serial.println(c);
  }
  EEPROM.get(pos, test2);
  test2[15] = 0;
  Serial.println(test2);
  EEPROM.end();

}

