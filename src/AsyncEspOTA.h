#pragma once

#ifndef __AsyncEspOTA_H__
#define __AsyncEspOTA_H__
#include <Arduino.h>

#include <Preferences.h>        // Store an unknown # of SSID & password pairs
#define RW_MODE false
#define RO_MODE true

#include <WiFi.h>				// it's IoT... why not
#include <esp_wps.h>			// Allow WPS router configurations
#include <WiFiMulti.h>			// Connect to any saved SSID in Preferences and within radio range

#include "wifiSettings.h"		// http://[insert host]/wifiSettings html page:
								// Read preferences, 
								// scan for local WiFi networks every 15 seconds, 
								// connect via WPS, 
								// save SSID/Password credentials to Preferences NVS RAM

#include <SPIFFS.h>
#define FORMAT_SPIFFS_IF_FAILED false;	// let's not be erasing everything if we fail to read on first attempt...

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

#include <ESPmDNS.h>            // bonjour compliant mulitcastDNS for iOS, MacOS, Linux, Windows & Androids 12 and later

#ifndef HTTP_UPLOAD_BUFLEN
#define HTTP_UPLOAD_BUFLEN 1436
#endif

//uncomment following line to enable debug mode
//#define __debug

extern  WiFiMulti wifiMulti;
		
//void OnWiFiEvent(WiFiEvent_t event, system_event_info_t info);
//void onWiFiEvent(WiFiEvent_t event, arduino_event_info_t info);
void onWiFiEvent(WiFiEvent_t event);
		
//Change the definition of the WPS mode from WPS_TYPE_PBC to WPS_TYPE_PIN in the case that you are using pin type WPS
#define ESP_WPS_MODE      WPS_TYPE_PBC
#define ESP_MANUFACTURER  "ESPRESSIF"
#define ESP_MODEL_NUMBER  "ESP32"
#define ESP_MODEL_NAME    "ESPRESSIF IOT"
#define ESP_DEVICE_NAME   "ESP STATION"

enum  socketDataType {
	PROGRESSSOCKET,
	DISCONNECTED
};

extern const char *ssidPrefix;
extern const char *APpassword;
		
extern AsyncWebServer webServer;
extern WebSocketsServer webSocket;	// convert to AsyncWebServer's built in webSockets
extern const int wsPort;			// convert to AsyncWebServer's built in webSockets

extern const char *appsURL;			// where to search on the network.internet for json file
extern bool allowAppsURL;			// Allow preconfigured OTA update path  ??
extern bool allowCustomPaths;		// Allow user to enter netwok/internet url for OTA update path  ??
extern bool allowLocal;				// Allow browser local file system OTA update path  ??


// Calback function supplied to update library so we can send progress data to 
// our webpage via a webSocketsServer
//  - needs to remain outside class to keep function signature correct
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
		#define StateError      -1  	// Error while trying to retrieve information
		#define StateNoData      0  	// no data
		#define StateDataUpdated 1  	// data updated
		#define StateNeedUpdate  2  	// need update (this will trigger an http request in the main loop)
        #define StateWaitingSocket 3	// Waiting for WebSocketsServer connection to permit progress data to be sent to client

		// Responses saved in the main loop and checked by the webServer periodically
		String repoList;
		String repoInformation;			

		int repoListState 			= StateNoData;
		int repoInformationState 	= StateNoData;
		int repoInstallationState 	= StateNoData;	
		
// all the webSockets stuff
		void setCurrentClient(uint8_t client_num, socketDataType type);
		
		// a function to provide a Callback function to the Update library to get progress values.
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
		int maxCredentials=30;
		int savedCredentials=0;
		
/*		typedef struct{
			const char *nameSpace;
			String key;
			String value;
		}Pref;
		Pref currentPref;
*/
		
		typedef struct{
			const char *nameSpace;
			int wlanID;
			String ssid;
			String password;
		}Credentials;
		Credentials currentCredentials;

		bool tryCredentials();
		bool readCredentials (Credentials *readCred);
		bool saveCredentials (Credentials *saveCred);
		String readPref(String nameSpace, String key);
		
// all the WiFi configuration stuff


		String ssid;
		String password;
		uint32_t chipID;        // unique chip ID / serialNumber
		const char *APssid;		// set to: ssidPrefix_chipID at runtime
		void startAP();
		
        WiFiClientSecure WiFiclient;
		
		const char *wl_status_to_string(wl_status_t status);	//
		
// WPS specific stuff							// TODO:  Connect via WPS ???   insecure ??
		static esp_wps_config_t config;
		void wpsInitConfig();
		String wpspin2string(uint8_t a[]);		
		void startWPS();
		void wpsStop();
		

// all the web server stuff
// WiFi settings page specific stuff		
		void onWiFiIndexRequest(AsyncWebServerRequest *request);	// serve up web page with saved settings
		void onWiFiSavedRequest(AsyncWebServerRequest *request);	// serve up the saved settings from Preferences
		void onWiFiScanRequest(AsyncWebServerRequest *request);		// serve up WIFI.Scan data

		void onWiFiSavePost(AsyncWebServerRequest *request);			// save Wireless LAN settings to preferences
		void onWiFiDeletePost(AsyncWebServerRequest *request);		// delete  Saved WiFi credentials
		
// Firmware update page specific stuff
		
		void handleFirmware(AsyncWebServerRequest *request);		// serve up the htnl page to

		void handleRepoList(AsyncWebServerRequest *request);		// Fetch all project repo available
		void getRepoList();											// find which apps we can install
		void checkRepoList(AsyncWebServerRequest *request);


		void handleRepoInformation(AsyncWebServerRequest *request);
		void checkRepoInformation(AsyncWebServerRequest *request);
		void getRepoInformation();									// get the availavable version for a specifi app

		
		
		void handleUpload(AsyncWebServerRequest *request);
		
		void handleInstallFromRepo(AsyncWebServerRequest *request);
		void install(AsyncWebServerRequest *request);
		void installFromRepo(void);
		
		//flag to use from web update to reboot the ESP
		bool shouldReboot = false;
		void handleRestart(AsyncWebServerRequest *request);

		void onUpdateSPIFFS(AsyncWebServerRequest *request);	// get the images
		
// a helper function for diagnostics
		void printRequestDetails(AsyncWebServerRequest *request);
		
				
// all the webSockets tracking stuff
		bool SocketStarted = false;
		socketDataType socketData;
};
#endif