/* Serial Commands */

const String CMD_RESET  = "reset";
const String CMD_RESTART  = "restart";
const String CMD_TEST  = "test";
const String CMD_WIFI = "wifi:";
const String CMD_STATUS = "status";
const String CMD_PORTAL = "portal:";


void cmd_reset() {
  Serial.println("Reseting board");
  resetCredentials();
  sent = false;
  //HWResetFunc();
}




// wifi:list; 
// wifi:set:4:SSID:PWD;
// wifi:set:3:SSID:; //for open wifi
// wifi:clear:4;
// wifi:swap:i,j;
// wifi:save;
// wifi:load;





void cmd_wifi(String wifi) {
  Serial.println("Wifi: "+wifi);
  if (wifi=="list") {
    Serial.println("Listing wifi APs");
    listCredentials();
  } else if (wifi=="save") {
    saveCredentials();
  } else if (wifi=="load") {
    loadCredentials();
  } else if (wifi.startsWith("clear:")) {
    clearCredentials(wifi.substring(6).toInt());
    Serial.println("cleared");
  } else if (wifi.startsWith("swap:")) {
    int c =  wifi.indexOf(',');
    if (c!=-1) {
      int i = wifi.substring(5,c).toInt();
      int j = wifi.substring(c+1).toInt();
      swapCredentials(i,j);
      Serial.println("Swaped");
    }
  } else if (wifi.startsWith("set:")) {
    String rest = wifi.substring(4);
    Serial.println("set rest:"+rest);
    int c = rest.indexOf(':');
    if (c!=-1) {
      int index = rest.substring(0,c).toInt();
      rest = rest.substring(c+1);
      int c = rest.indexOf(':');
      if (c!=-1) {
        String ssid = rest.substring(0,c);
        String password = rest.substring(c+1);
        Serial.print("index: ");Serial.println(index);
        Serial.println(" SSID: "+ ssid+" PASSWORD:"+password);
        setCredentials(index,ssid,password);
        Serial.println("Saved");
      }
    }
  }
}

void cmd_status() {
  Serial.print("Portal: ");Serial.println(portal);
  Serial.print("Restart reason: ");Serial.println(ESP.getResetReason());
  Serial.print("Mac: ");Serial.println(WiFi.macAddress());
  Serial.print("Status: ");Serial.println(WiFi.status());
  if (WiFi.status() == WL_CONNECTED ) {
    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());
  }
  Serial.print("Message sent: ");Serial.println(sent);
  Serial.print("Free Heap: ");Serial.println(ESP.getFreeHeap());
}


// portal:start;


void cmd_portal(String command) {
  Serial.println("portal command: "+command);
  if (command == "start") {
    portal = true;
    Serial.println("Starting portal....");
  }
}


void parseCommand(String command) {
  Serial.println("parsing Command:'"+command+"'");
  if (command==CMD_RESET) {
    cmd_reset();
  } else if (command == CMD_RESTART) {
    ESP.restart();
  } else if (command.startsWith(CMD_WIFI)) {
    cmd_wifi(command.substring(CMD_WIFI.length()));
  } else if (command == CMD_STATUS) {
    cmd_status();
  } else if (command == CMD_TEST) {
    test();
  } else if (command.startsWith(CMD_PORTAL)) {
    cmd_portal(command.substring(CMD_PORTAL.length()));
  }
}

char cmd[128];
int p = 0;


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


