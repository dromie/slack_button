#ifndef WIFICLIENTMYMULTI_H_
#define WIFICLIENTMYMULTI_H_

#include "ESP8266WiFi.h"
#include <vector>

#ifdef DEBUG_ESP_WIFI
#ifdef DEBUG_ESP_PORT
#define DEBUG_WIFI_MULTI(...) DEBUG_ESP_PORT.printf( __VA_ARGS__ )
#endif
#endif

#ifndef DEBUG_WIFI_MULTI
#define DEBUG_WIFI_MULTI(...)
#endif

struct MyWifiAPEntry {
    char * ssid;
    char * passphrase;
    int failedConnects;
    private:
    //MyWifiAPEntry(MyWifiAPEntry &copy) {};
};

typedef std::vector<MyWifiAPEntry> MyWifiAPlist;

class ESP8266WiFiMyMulti {
    public:
        ESP8266WiFiMyMulti();
        ~ESP8266WiFiMyMulti();

        bool addAP(const char* ssid, const char *passphrase = NULL);
        typedef std::function<bool(void)> TConnectivityTestFunction;

        wl_status_t run(void);
        wl_status_t run(TConnectivityTestFunction method);
    private:
        MyWifiAPlist APlist;
        bool APlistAdd(const char* ssid, const char *passphrase = NULL);
        void APlistClean(void);

};

#endif /* WIFICLIENTMYMULTI_H_ */
