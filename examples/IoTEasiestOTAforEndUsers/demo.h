
//Libraries **************************************************************************
#include <Arduino.h>
//#include <WiFi.h>               // We're in IoT land, do as IoT does
//#include <esp_wps.h>            // wifi provisioning via WPS

#include <Preferences.h>        //to save SSID & password pairs to pass to WiFiMulti
#include <WiFiMulti.h>          // To assist with provisioning
//#include <ESPmDNS.h>            // bonjour compliant mulitcastDNS for iOS, MacOS, Linux, Windows.. NO Androids allowed :(

#include <nvs_flash.h>        // uncomment to be able to erase all flash

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>  // Asynchronous web server
#include <WebSocketsServer.h>   // allows OTAUpdate progress bar to send increasing values to browser page.

#include <SPIFFS.h>
#define FORMAT_SPIFFS_IF_FAILED false;

#include <AsyncEspOTA.h>        //library under test !!!

//Initialize the libraries  **********************************************************
#define ESP_WPS_MODE      WPS_TYPE_PBC
#define ESP_MANUFACTURER  "ESPRESSIF"
#define ESP_MODEL_NUMBER  "ESP32"
#define ESP_MODEL_NAME    "ESPRESSIF IOT"
#define ESP_DEVICE_NAME   "ESP STATION"

static esp_wps_config_t config;

const char *ssidPrefix = "AsyncEspOTA_";
const char *APpassword = "123456789";

WiFiMulti wifiMulti;
long int savedTime;

const int wsPort = 1337;            // variableName must match library decalaration..        declared extern in library..  Port value may be different

                                    //VariableName must match library decalaration..         declared extern in library..  
const char *appsURL= "https://raw.githubusercontent.com/linzmeister/AsyncEspOTA/main/DemoRepo/apps.json";

AsyncWebServer webServer(80);       // variableName must match library object declaration..  declared extern in library..  Port value may be different
WebSocketsServer webSocket(wsPort); // variableName must match library object declaration..  declared extern in library..

bool allowAppsURL = true;
bool allowCustomPaths = false;
bool allowLocal = false;

AsyncEspOTA myUpdater;    // connects to wifi on boot
