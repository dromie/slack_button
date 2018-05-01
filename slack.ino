#include <ESP8266WiFi.h>


/*
 WIFI CONFIGURATION
 */
String SSID[] = { "VendegPanni" };
String pwd[] = { "Nullacska0" };

/* Serial Commands */

const String CMD_RESET  = "reset";
const String CMD_WIFI = "wifi:";
const String CMD_MAC = "mac";

/*
 SLACK CONFIGURATION
 */
const String slack_token = "T4LFAT7RV/B4K4LV7CH/sMnkeMvQMYYAuofZOt13m68G";
const String slack_hook_url = "/services/" + slack_token;
const String slack_icon_url = "<SLACK_ICON_URL>";
const String slack_message = "test";
const String slack_username = "<SLACK_USERNAME>";

void(* HWResetFunc) (void) = 0; //declare reset function @ address 0

void setup()
{
  Serial.begin(115200);
  Serial.flush();
  Serial.println();
  portalSetup();
  WiFi.begin(SSID[0].c_str(), pwd[0].c_str());
  Serial.println("Mac:");
  Serial.println(WiFi.macAddress());
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}



bool postMessageToSlack(String msg)
{
  const char* host = "hooks.slack.com";
  Serial.print("Connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClientSecure client;
  const int httpsPort = 443;
  if (!client.connect(host, httpsPort)) {
    Serial.println("Connection failed :-(");
    return false;
  }

  // We now create a URI for the request

  Serial.print("Posting to URL: ");
  Serial.println(slack_hook_url);

  //String postData="payload={\"link_names\": 1, \"icon_url\": \"" + slack_icon_url + "\", \"username\": \"" + slack_username + "\", \"text\": \"" + msg + "\"}";
  String postData="payload={\"text\": \"" + msg + "\"}";
  // This will send the request to the server
  client.print(String("POST ") + slack_hook_url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Content-Type: application/x-www-form-urlencoded\r\n" +
               "Connection: close" + "\r\n" +
               "Content-Length:" + postData.length() + "\r\n" +
               "\r\n" + postData);
  Serial.println("Request sent");
  String line = client.readStringUntil('\n');
  Serial.printf("Response code was: ");
  Serial.println(line);
  if (line.startsWith("HTTP/1.1 200 OK")) {
    return true;
  } else {
    return false;
  }
}

bool sent = false;
char cmd[16];
int p = 0;

void cmd_reset() {
  Serial.println("Reseting board");
  sent = false;
  HWResetFunc();
}

void cmd_wifi(String wifi) {
  Serial.println("Wifi: "+wifi);
}

void cmd_mac() {
  Serial.println("Mac:");
  Serial.println(WiFi.macAddress());
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void parseCommand(String command) {
  Serial.println("parsing Command:'"+command+"'");
  if (command==CMD_RESET) {
    cmd_reset();
  } else if (command.startsWith(CMD_WIFI)) {
    cmd_wifi(command.substring(5));
  } else if (command == CMD_MAC) {
    cmd_mac();
  }
}

void serialTerminalLoop() {
  if (Serial.available()) {
    char c = Serial.read();
    if (c!=10 && c!=13 && c!=';') {
      cmd[p]=c;
      ++p;
      cmd[p]=0; 
    }
    if (c==';') {
      cmd[p] = 0;
      parseCommand(String(cmd));
      p = 0;
    }
  }
  
}

void loop()
{
  serialTerminalLoop();
  if (!sent) {
    sent = postMessageToSlack(slack_message);
    Serial.println("Sent:" + sent);
    delay(1000);
  }
  serverLoop();
} 

