#pragma once

#ifndef __AsyncEspOTA_H__
#define __AsyncEspOTA_H__

#include <Arduino.h>

#include <Preferences.h>        // Store an unknown # of SSID & password pairs

#include <WiFi.h>				// it's IoT... why not
#include <esp_wps.h>			// Allow WPS router configurations
#include <WiFiMulti.h>			// Connect to any saved SSID in Preferences and within radio range

#include "wifiSettings.h"		// http://[insert host]/wifiSettings.html page:
								// Read preferences, 
								// scan for local WiFi networks every 15 seconds, 
								// connect via WPS, 
								// save Preferences

// #include <FS.h>
#include <SPIFFS.h>
#define FORMAT_SPIFFS_IF_FAILED false;

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>	// Allow asynchronous web communications 
#include <WebSocketsServer.h>	// Allow progress bar & restarting feedback during OTA updates

#include "index.h"				// http://[insert host]/Firmware.html page
								// developer can choose to permit any or all of 3 datasources:
									// Pre-programmed internet locations
									// user selectable (inter)network locations
									// Browser's local file system
								// Populate page with OTA capable Apps
								// Get available versions for each app
								// start OTA update
								// provide prwifiMultiogress feedback and return browser to home page when OTA complete.
								
#include <HTTPUpdate.h>			// Perform the OTA update
#include <Update.h>				// Gain access to the OTA progressCallbackFunction

#ifndef HTTP_UPLOAD_BUFLEN
#define HTTP_UPLOAD_BUFLEN 1436
#endif

//uncomment following line to enable debug mode
#define __debug


extern  WiFiMulti wifiMulti;

		
const char* const nameSpaces[] = {	// Preferences Name spaces
	"wlan-0", 
	"wlan-1", 
	"wlan-2", 
	"wlan-3", 
	"wlan-4", 
	"wlan-5", 
	"wlan-6", 		// do I need to declare these?
	"wlan-7", 		// could the savedWlans in javascript  control last index??
	"wlan-8", 
	"wlan-9",
	"wlan-10",
	"wlan-11",
	"wlan-12",
	"wlan-13",
	"wlan-14",
	"wlan-15",
	"wlan-16",
	"wlan-17",
	"wlan-18",
	"wlan-19",
	"wlan-20",
	"wlan-21",
	"wlan-22",
	"wlan-23",
	"wlan-24",
	"wlan-25",
	"wlan-26"};

//void OnWiFiEvent(WiFiEvent_t event, system_event_info_t info);
//void onWiFiEvent(WiFiEvent_t event, arduino_event_info_t info);
void onWiFiEvent(WiFiEvent_t event);
		
//Change the definition of the WPS mode from WPS_TYPE_PBC to WPS_TYPE_PIN in the case that you are using pin type WPS
#define ESP_WPS_MODE      WPS_TYPE_PBC
#define ESP_MANUFACTURER  "ESPRESSIF"
#define ESP_MODEL_NUMBER  "ESP32"
#define ESP_MODEL_NAME    "ESPRESSIF IOT"
#define ESP_DEVICE_NAME   "ESP STATION"

extern const char *appsURL;
extern AsyncWebServer webServer;
extern WebSocketsServer webSocket;
extern const int wsPort;


enum  socketDataType {
	SCANSOCKET,
	SAVEDSOCKET,
	PROGRESSSOCKET,
	DISCONNECTED
};

extern bool allowAppsURL;
extern bool allowCustomPaths;
extern bool allowLocal;

// So we can send progress data to our webpage via a webSocketsServer
// needs to remain outside class to keep function signature correct
void updateProgress(size_t progress, size_t total);

/*
enum HTTPUploadStatus { UPLOAD_FILE_START, UPLOAD_FILE_WRITE, UPLOAD_FILE_END,
                        UPLOAD_FILE_ABORTED };

typedef struct {
  HTTPUploadStatus status;
  String  filename;
  String  name;
  String  type;
  size_t  totalSize;    // file size  
  size_t  currentSize;  // size of data currently in buf
  uint8_t buf[HTTP_UPLOAD_BUFLEN];
} HTTPUpload;
*/

class AsyncEspOTA {
	public:
		AsyncEspOTA();
		void begin();
		void loop();
	
		// Note: The following part is all about acting as a proxy. 
		// The esp32 will retrieve information on our behalf and then the front side will check periodically if we have something new

		// These are flags to notify the main loop to download information
		#define StateNoData      0  // no data
		#define StateDataUpdated 1  // data updated
		#define StateNeedUpdate  2  // need update (this will trigger an http request in the main loop)
		#define StateError      -1  // Error while trying to retrieve informations
        #define StateWaitingSocket 3	// Waiting for WebSocketsServer connection to permit progress data to be sent to client

		// Response saved in the main loop and checked by the webServer periodically
		String repoList;
		String repoInformation;			

		int repoListState 			= StateNoData;
		int repoInformationState 	= StateNoData;
		int repoInstallationState 	= StateNoData;	
		
// all the webSockets stuff
		void setCurrentClient(uint8_t client_num, socketDataType type);
		
		// a callback function from the users sketch to pass into the Update library to get progress values.
		void setProgressCallback(UpdateClass::THandlerFunction_Progress progressCallbackFunction);		
		
// all the OTAUpdate stuff
		// http paths to be used by the main loop
		String repoPath;
		String spiffPath;
		String binPath;
		
		
	private:		
	long int savedTime;	
// all the Preferences stuff

		Preferences preferences;

		typedef struct{
			int NameSpace;
			String Key;
			String Value;
		}Pref;
		Pref currentPref;
		
		typedef struct{
			const char *NameSpace;
			String Key;
			String Value;
		}nPref;
		nPref newPref;
		
		bool tryCredentials();
		bool ReadCredentials (Pref *readPref);
		void SavePref (Pref *savePref);
		String ReadPref(int nameSpace, String key);
		
// all the WiFi configuration stuff
		
		
        WiFiClientSecure WiFiclient;

		
// WPS specific stuff
		static esp_wps_config_t config;
		void wpsInitConfig();
		String wpspin2string(uint8_t a[]);		
		void startWPS();
		void wpsStop();
		

// all the WiFi page stuff		
		void onWiFiIndexRequest(AsyncWebServerRequest *request);		// serve up web page with saved settings
			//void onWiFiPost(AsyncWebServerRequest *request);// save Wireless LAN settings to preferences
		void onWiFiPost(AsyncWebServerRequest *request);
			//, uint8_t *data, size_t len, size_t index, size_t total
		
		void onWiFiSavedRequest(AsyncWebServerRequest *request);// serve up the saved settings from Preferences
		void onWiFiScanRequest(AsyncWebServerRequest *request);		// serve up WIFI.Scan data

// all the Firmware update page stuff
		void getRepoList();
		void getRepoInformation();
		
		void handleFirmware(AsyncWebServerRequest *request);	
		void handleRepoInformation(AsyncWebServerRequest *request);
		void checkRepoInformation(AsyncWebServerRequest *request);
		
		void handleRepoList(AsyncWebServerRequest *request);
		void checkRepoList(AsyncWebServerRequest *request);
		
		void handleUpload(AsyncWebServerRequest *request);
		
		void handleInstallFromRepo(AsyncWebServerRequest *request);
		void install(AsyncWebServerRequest *request);
		void installFromRepo(void);
		
		//flag to use from web update to reboot the ESP
		bool shouldReboot = false;
		void handleRestart(AsyncWebServerRequest *request);

// a helper function for diagnostics
		void printRequestDetails(AsyncWebServerRequest *request);
		
				
// all the webSockets tracking stuff
		bool SocketStarted = false;
		socketDataType socketData;
};
#endif