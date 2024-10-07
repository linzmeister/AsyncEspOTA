
//Libraries **************************************************************************
#include <Arduino.h>
#include <WiFi.h>         // We're in IoT land, do as IoT does
//#include <esp_wps.h>    // wifi provisioning via WPS

#include <H4.h>
#include<H4AsyncWebServer.h>

#include<FS.h>
#include<SPIFFS.h>

#define HAL_FS SPIFFS
#define MY_BOARD "ESP32 Dev Board"
#define DEVICE "FULL_HOUSE"

#include <nvs_flash.h>        // uncomment to be able to erase all flash

// #in clude <WebSocketsServer.h>   // allows OTAUpdate progress bar to send increasing values to browser page.

#include <SPIFFS.h>
#define FORMAT_SPIFFS_IF_FAILED false;

#include <H4AsyncEspOTA.h>        //library under test !!!

const char *ssidPrefix = "AsyncEspOTA_";
const char *APpassword = "123456789";

//WiFiMulti wifiMulti;
long int savedTime;

const int wsPort = 1337;            // variableName must match library decalaration..        declared extern in library..  Port value may be different

                                    //VariableName must match library decalaration..         declared extern in library..  
const char *appsURL= "https://raw.githubusercontent.com/linzmeister/AsyncEspOTA/main/DemoRepo/apps.json";

H4 h4(115200);
H4AsyncWebServer webServer(80);       // variableName must match library object declaration..  declared extern in library..  Port value may be different

// WebSocketsServer webSocket(wsPort); // variableName must match library object declaration..  declared extern in library..

bool allowAppsURL = true;
bool allowCustomPaths = false;
bool allowLocal = false;

//AsyncEspOTA myUpdater;    // connects to wifi on boot
