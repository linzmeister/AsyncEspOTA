#include "AsyncEspOTA.h"

String messagePrefix;	// s = SPIFFS, b = Binary Firmawre file
String message;	
uint8_t clientNum;

const int HOMENETWORK     = 1;  //make these and ENUM ??
const int AWAYNETWORK     = 2; 
const int HOTSPOTNETWORK  = 3;  //wlan-0, wlan-1


/*
void AsyncEspOTA::OnWiFiEvent(WiFiEvent_t event, arduino_event_info_t info){
  switch (event) { 
        case ARDUINO_EVENT_WIFI_STA_START:
      Serial.println("Station Mode Started");
      break;
    
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.println("Connected to :" + String(WiFi.SSID()));
      Serial.print("Got IP: ");
      Serial.println(WiFi.localIP());
      break;
      
    case SYSTEM_EVENT_STA_CONNECTED:
#ifdef __debug    
      Serial.println("Connected to WiFi Network");
#endif
      break;
      
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
#ifdef __debug    
      Serial.println("Disconnected from WiFi Network");
#endif
      WiFi.reconnect();   // check if this will come back to bit us in the little donkey.
      break;

    case ARDUINO_EVENT_AP_STOP:
      Serial.println("WiFi access point stopped");
      break;

    case ARDUINO_EVENT_STA_STOP:
      Serial.println("WiFi clients stopped");
      break;

    case ARDUINO_EVENT_WPS_ER_SUCCESS:
      Serial.println("WPS Successful, stopping WPS and connecting to: " + String(WiFi.SSID()));
      esp_wifi_wps_disable();
      delay(10);
      WiFi.begin();
      break;
    
    case ARDUINO_EVENT_WPS_ER_FAILED:
      Serial.println("WPS Failed, retrying");
      esp_wifi_wps_disable();
      esp_wifi_wps_enable(&config);
      esp_wifi_wps_start(0);
      break;
    
    case ARDUINO_EVENT_WPS_ER_TIMEOUT:
      Serial.println("WPS Timeout, retrying");
      esp_wifi_wps_disable();
      esp_wifi_wps_enable(&config);
      esp_wifi_wps_start(0);
      break;
    
    case ARDUINO_EVENT_WPS_ER_PIN:
      Serial.println("WPS_PIN = " + wpspin2string(info.wps_er_pin.pin_code));
      break;
            
    default: break;
  }
}
*/

void OnWiFiEvent(WiFiEvent_t event){
  switch (event) { 
    case SYSTEM_EVENT_STA_CONNECTED:
#ifdef __debug    
      Serial.println("[WiFi_Event] Connected to WiFi Network");
#endif
      break;
      
    case SYSTEM_EVENT_STA_DISCONNECTED:
#ifdef __debug
      Serial.println("[WiFi_Event] Disconnected from WiFi Network");
#endif
      WiFi.reconnect();   // check if this will come back to bit us in the little donkey.
      //wifiMulti.run();
      break;

    case SYSTEM_EVENT_AP_STOP:
      Serial.println("[WiFi_Event] WiFi access point stopped");
      break;

    case SYSTEM_EVENT_STA_STOP:
      Serial.println("[WiFi_Event] WiFi client stopped");
      break;
      
    default: break;
  }
}


void updateProgress(size_t progress,  size_t total){
	double progPercent = (float(progress)/float(total)*100);
	message = messagePrefix
	+ String(progress) 
	+ "," 
	+ String(total) 			//example message:   b999424,1012528#84.20%
	+ "#" 
	+ String(progPercent) 
	+ "%";
	
	webSocket.sendTXT(clientNum, message);

	//	send a websocket message to reload the '/' page or force a back button press ??	
	if(messagePrefix =="b" && progPercent==100.00){
		webSocket.sendTXT(clientNum,"Restarting");
	}
}



AsyncEspOTA::AsyncEspOTA(){
}


void AsyncEspOTA::wpsInitConfig(){
  //config.crypto_funcs = &g_wifi_default_wps_crypto_funcs;
  config.wps_type = ESP_WPS_MODE;
  strcpy(config.factory_info.manufacturer, ESP_MANUFACTURER);
  strcpy(config.factory_info.model_number, ESP_MODEL_NUMBER);
  strcpy(config.factory_info.model_name, ESP_MODEL_NAME);
  strcpy(config.factory_info.device_name, ESP_DEVICE_NAME);
}


String AsyncEspOTA::wpspin2string(uint8_t a[]){
  char wps_pin[9];
  for(int i=0;i<8;i++){
    wps_pin[i] = a[i];
  }
  wps_pin[8] = '\0';
  return (String)wps_pin;
}


void AsyncEspOTA::startWPS(){
  WiFi.onEvent(onWiFiEvent);
  WiFi.mode(WIFI_MODE_APSTA);		//originally WIFI_MODE_STA

  Serial.println("Starting WPS");

  wpsInitConfig();
  esp_wifi_wps_enable(&config);
  esp_wifi_wps_start(0);
}


void AsyncEspOTA::wpsStop(){
    if(esp_wifi_wps_disable()){
    	Serial.println("WPS Disable Failed");
    }
}



// Attempt to read existing preferences.
// Then attempt to start Wi-Fi client with network 1, then 2.. etc
bool AsyncEspOTA::tryCredentials(){
	Serial.println("inside tryCredentials");
    for (int counter = 1; counter<4; counter++){
      
      currentPref.NameSpace=counter;
      bool canRead=ReadCredentials(&currentPref);
      if (canRead){
        if(currentPref.Key!=""){
          wifiMulti.addAP(currentPref.Key.c_str(),currentPref.Value.c_str());
          Serial.println("added " + currentPref.Key + "\t" + currentPref.Value+ " to AP List");
        } else {
          Serial.println("Empty credentials - skipping");
        }
      } else {
        Serial.println("can't read Preferences");
      }
    }
    return false;
}


//********************************************************************************************
/*    Accepts a pref pointer
 *    Updates the pointed to pref Type
 */
bool AsyncEspOTA::ReadCredentials (Pref *readPref){
    switch(readPref->NameSpace){
      case HOMENETWORK:
          preferences.begin("HOMENETWORK", false);
          readPref->Key = preferences.getString("homessid");
          readPref->Value = preferences.getString("homepassword");
          preferences.end();
          return true;
        break;
        
     case AWAYNETWORK:
          preferences.begin("AWAYNETWORK", false);
          readPref->Key = preferences.getString("awayssid");
          readPref->Value = preferences.getString("awaypassword");
          preferences.end();
          return true;
        break;
        
      case HOTSPOTNETWORK:
          preferences.begin("HOTSPOTNETWORK", false);
          readPref->Key = preferences.getString("hotspotssid");
          readPref->Value = preferences.getString("hotspotpassword");
          preferences.end();
          return true;
        break;
      
      default: 
          preferences.end();
          return false;
          break;
    }
}


void AsyncEspOTA::SavePref (Pref *savePref){      
    switch(savePref->NameSpace){
      case HOMENETWORK:
        if(savePref->Key != ""){
          preferences.begin("HOMENETWORK", false);
          if(savePref->Key == "homessid"){
            preferences.putString("homessid", savePref->Value);          
          }
          preferences.putString("homepassword", savePref->Value);
          preferences.end();
        }
        break;
        
      case AWAYNETWORK:
        if(savePref->Key != ""){
          preferences.begin("AWAYNETWORK", false);
          if(savePref->Key == "awayssid"){
            preferences.putString("awayssid", savePref->Value);
          }
          preferences.putString("awaypassword", savePref->Value);
          preferences.end();
        }
        break;
        
      case HOTSPOTNETWORK:
        if(savePref->Key != ""){
          preferences.begin("HOTSPOTNETWORK", false);
          if(savePref->Key == "hotspotssid"){
            preferences.putString("hotspotssid", savePref->Value);
          }
          preferences.putString("hotspotpassword", savePref->Value);
          preferences.end();
        }
        break;
        
      default:
          break;
        }
    
#ifdef __debug  
      Serial.println("Preference(s) saved :   " + savePref->Key + ", " + savePref->Value);
#endif
    preferences.end();
}

//
// Allows Web Server to read prefernces and populate the settings web page
//
String AsyncEspOTA::ReadPref (int nameSpace, String key){
 Serial.println("entering String ReadPref (int nameSpace, String key)");
  String returnString;
  bool returnBool;
  switch(nameSpace){
    case HOMENETWORK:
      preferences.begin("HOMENETWORK", false);
      returnString = preferences.getString("key");  //one of 2 choices
        
    case AWAYNETWORK:
      preferences.begin("AWAYNETWORK", false);
      returnString = preferences.getString("key");  //one of 2 choices
    
    case HOTSPOTNETWORK:
      preferences.begin("HOTSPOTNETWORK", false);
      returnString = preferences.getString("key");  //one of 2 choices
    
    default:
      returnString = "";
  }
    preferences.end();
    return returnString;
}



void AsyncEspOTA::begin(){
	//See discussion: https://github.com/RomeHein/ESPInstaller/issues/2
	WiFiclient.setInsecure();
	
	WiFi.onEvent(OnWiFiEvent);
	tryCredentials();		// read Preferences and add to WifiMulti to connect.

	int i=0;	
	Serial.println("Starting wifiMulti");
	WiFi.scanNetworks();
	delay(5000);
	while(wifiMulti.run() != WL_CONNECTED) {
		//Serial.println("[AsyncEspOTA.begin] WiFi not connected!");
		delay(2000);
		i++;
		if (i>4){
			WiFi.mode(WIFI_MODE_AP	);	//APSTA
			break;	
		}
	}
	
	Serial.println("");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
    
	
	if(!SPIFFS.begin()){
		Serial.println("An Error has occurred while mounting SPIFFS");
	} else {
		Serial.println("SPIFFS mounted successfully");
	}
	/*
		sequence of event handlers is important   !!
		Longest branch paths first
		specific >>> general
	*/
	
// ************************************************************************************************************************		
	//webServer.serveStatic("/", SPIFFS, "/www/").setFilter(ON_STA_FILTER);
	//webServer.serveStatic("/", SPIFFS, "/ap/").setFilter(ON_AP_FILTER);
// ************************************************************************************************************************	

  // Simple Firmware Update Form
  webServer.on("/update", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html",
	"<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>");
  });
/* ***********************************************************************************************************************	

 webServer.on("/update", HTTP_POST, [](AsyncWebServerRequest *request){
    bool shouldReboot = !Update.hasError();
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", shouldReboot?"OK":"FAIL");
    response->addHeader("Connection", "close");
    request->send(response);
  },[](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
    if(!index){
      Update.onProgress(updateProgress);  //progressCallbackFunction  
      Serial.printf("Update Start: %s\n", filename.c_str());
      if(!Update.begin()){
        Update.printError(Serial);
      }
    }
    if(!Update.hasError()){
      if(Update.write(data, len) != len){
        Update.printError(Serial);
      }
    }
    if(final){
      if(Update.end(true)){
        Serial.printf("Update Success: %uB\n", index+len);
      } else {
        Update.printError(Serial);
      }
    }
  });
// ************************************************************************************************************************	
*/		
	// respond to the restart button from the client 
	webServer.on("/restart", HTTP_GET, [this](AsyncWebServerRequest *request){
        //Serial.println(" received /restart request");
		handleRestart(request);               
     });
	 
	//  read preferences - serve list to client
	webServer.on("/wifiSettings/saved", HTTP_GET, [this](AsyncWebServerRequest *request) {
		//Serial.println(" received /wifiSettings/saved request");
		onWiFiSavedRequest(request);
	});
	
	//  - Scan WiFi, serve list to client
	webServer.on("/wifiSettings/scan", HTTP_GET, [this](AsyncWebServerRequest *request) {
		 //Serial.println(" received /wifiSettings/scan request");
		 onWiFiScanRequest(request);
	});

	// Client sends a json object of ssid & paasswords to save
	// Save wifiSettings posted from the page to preferences nv flashram
    webServer.on("/wifiSettings/saveScannedWlan", HTTP_POST, 
	[this](AsyncWebServerRequest * request){
		Serial.println(" received /wifiSettings/saveScannedWlan request");
		onWiFiPost(request);
	});
	/*,
    NULL,
    [this] (AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
		onWiFiPost(request, data, len, index, total);
        
    });
*/	
/*	
	webServer.on(
    "/post",
    HTTP_POST,
    [](AsyncWebServerRequest * request){},
    NULL,
    [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
 
      for (size_t i = 0; i < len; i++) {
        Serial.write(data[i]);
      }
 
      Serial.println();
 
      request->send(200);
  });
*/	
	
	
	
	// serve the /wifiSettings.hmtl page to client 
	webServer.on("/wifiSettings", HTTP_GET, [this](AsyncWebServerRequest *request){
        //Serial.println(" received /wifiSettings request");
		onWiFiIndexRequest(request);               
    });
	
	// get version from internet
    webServer.on("/firmware/repo/info/request", HTTP_POST, [this](AsyncWebServerRequest *request){
        handleRepoInformation(request);
    });
    
	//send version to client
	webServer.on("/firmware/repo/info", HTTP_GET, [this](AsyncWebServerRequest *request){
        checkRepoInformation(request);
    });
    
	// get repo list from internet	
	webServer.on("/firmware/repo/list/request", HTTP_GET, [this](AsyncWebServerRequest *request){
        handleRepoList(request);
    });
    
	
	// send list to client
	webServer.on("/firmware/repo/list", HTTP_GET, [this](AsyncWebServerRequest *request){
        checkRepoList(request);
    });
	 
	// client sends spiifs/ bin paths to us
	webServer.on("/firmware/repo/install/request", HTTP_POST, [this](AsyncWebServerRequest *request){
        handleInstallFromRepo(request);
    });
	 
	// send the /firmware HTML page to client 
	webServer.on("/firmware", HTTP_GET, [this](AsyncWebServerRequest *request){
        handleFirmware(request);               
     });

/*	
	// receive file upload from client file system and update firmware
	webServer.on("/update", HTTP_POST, [this](AsyncWebServerRequest *request) {
		handleUpload(request); 
	 },
		[this]
		{install(request); 
	});
*/	
	// send a file when /index is requested									for later
    //webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    //  request->send(SPIFFS, "/index.html");
	//  request->send(SD, "/index.html");
    // });


	// attach filesystem root at URL /images
    webServer.serveStatic("/images", SPIFFS, "/");
	
}
/*
	server.on("/update",HTTP_POST,[this]() { handleUpload(); }, [this]() { install(); });
	
	(AsyncWebServerRequest *request)> ArRequestHandlerFunction;
	(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final)> ArUploadHandlerFunction;
	(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)> ArBodyHandlerFunction;
	
	AsyncCallbackWebHandler& on(const char* uri, ArRequestHandlerFunction onRequest);
	AsyncCallbackWebHandler& on(const char* uri, WebRequestMethodComposite method, ArRequestHandlerFunction onRequest);
	AsyncCallbackWebHandler& on(const char* uri, WebRequestMethodComposite method, ArRequestHandlerFunction onRequest, ArUploadHandlerFunction onUpload);
	AsyncCallbackWebHandler& on(const char* uri, WebRequestMethodComposite method, ArRequestHandlerFunction onRequest, ArUploadHandlerFunction onUpload, ArBodyHandlerFunction onBody);
	void onNotFound(ArRequestHandlerFunction fn);  //called when handler is not assigned
    void onFileUpload(ArUploadHandlerFunction fn); //handle file uploads
    void onRequestBody(ArBodyHandlerFunction fn); //handle posts with plain body content (JSON often transmitted this way as a request)
*/

/*
//  local OTA file upload and flash	-- read AsyncWebServer doc to deconstruct
void AsyncEspOTA::install(AsyncWebServerRequest *request){
			  
	webServer.on("/update",HTTP_POST,[](AsyncWebServerRequest *request){shouldReboot = !Update.hasError();
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", shouldReboot?"OK":"FAIL");
    response->addHeader("Connection", "close");
    request->send(response);},
	[] (install) (AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
    if(!index){
      Update.onProgress(updateProgress);  //progressCallbackFunction  
      Serial.printf("Update Start: %s\n", filename.c_str());
      if(!Update.begin()){
        Update.printError(Serial);
      }
    }
    if(!Update.hasError()){
      if(Update.write(data, len) != len){
        Update.printError(Serial);
      }
    }
    if(final){
      if(Update.end(true)){
        Serial.printf("Update Success: %uB\n", index+len);
      } else {
        Update.printError(Serial);
      }
    }
  });
}
*/

void AsyncEspOTA::installFromRepo(void){
#ifdef __debug
	Serial.println("inside AsyncEspOTA::installFromRepo(void)");	
#endif	
  if (!spiffPath.isEmpty()){		
#ifdef __debug
    Serial.printf("[OTA] start SPIFFS download from: %s\n",spiffPath.c_str());
#endif
	Update.onProgress(updateProgress);	//progressCallbackFunction
	
	// initialise message with for 's' or SPIFFS 
	messagePrefix = "s";

    t_httpUpdate_return spiffRet = httpUpdate.updateSpiffs(WiFiclient,spiffPath);
    switch (spiffRet)
    {
    case HTTP_UPDATE_FAILED:
#ifdef __debug
		Serial.printf("[OTA] Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
#endif
		webSocket.sendTXT(clientNum,"Upload incomplete");
      return;

    case HTTP_UPDATE_NO_UPDATES:
#ifdef __debug
      Serial.println("[OTA] no updates");
#endif
		webSocket.sendTXT(clientNum,"No Updates available");
      return;

    case HTTP_UPDATE_OK:
#ifdef __debug
      Serial.println("[OTA] SPIFFS updated");
#endif
      break;
    }
  }

  if (!binPath.isEmpty()){
#ifdef __debug
    Serial.printf("[OTA] start BIN download from: %s\n",binPath.c_str());
#endif

	// initialise message with 'b' for BIN 
	messagePrefix = "b";
	
    t_httpUpdate_return binRet = httpUpdate.update(WiFiclient, binPath);
    switch (binRet){
		case HTTP_UPDATE_FAILED:
#ifdef __debug
			Serial.printf("[OTA] Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
#endif
			repoInstallationState = StateError;
#ifdef __debug
			Serial.println("Set repoInstallationState = " + String(repoInstallationState));
#endif
			webSocket.sendTXT(clientNum,"Upload incomplete");
			break;

		case HTTP_UPDATE_NO_UPDATES:
#ifdef __debug
			Serial.println("[OTA] no updates");
#endif
			repoInstallationState = StateError;
#ifdef __debug
			Serial.println("Set repoInstallationState = " + String(repoInstallationState));
#endif
			webSocket.sendTXT(clientNum,"No Updates available");
			break;

		case HTTP_UPDATE_OK:
#ifdef __debug
			Serial.println("[OTA] BIN updated");
#endif
			repoInstallationState = StateDataUpdated;
#ifdef __debug
			Serial.println("Set repoInstallationState = " + String(repoInstallationState));
#endif
			//	send a websocket message to notify client we have finished
			webSocket.sendTXT(clientNum,"Restarting");
			ESP.restart();
			break;
		}
  }
}


void AsyncEspOTA::handleUpload(AsyncWebServerRequest *request){
#ifdef __debug
    Serial.println("inside AsyncEspOTA::handleUpload");
	printRequestDetails(request);
#endif
	request->send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart(); 
}


void AsyncEspOTA::getRepoList(){
#ifdef __debug
	Serial.println("inside AsyncEspOTA::getRepoList()");
#endif
	HTTPClient http;
#ifdef __debug
  Serial.printf("[SERVER] Retrieving repo list from %s\n", appsURL);
#endif
  http.begin(WiFiclient, appsURL);
  int httpResponseCode = http.GET();
  if (httpResponseCode > StateNoData)  {
	repoList = http.getString();
	repoListState = StateDataUpdated;
#ifdef __debug
	Serial.println("Set repoListState = " + String(repoListState));
    Serial.printf("[SERVER] repo list retrieved: \n%s\n",repoList.c_str());
#endif
  } else{
#ifdef __debug
	Serial.printf("[SERVER] Could not get repo list. Error %d: %s\n", httpResponseCode, http.errorToString(httpResponseCode).c_str());
#endif
	repoListState = StateError;
#ifdef __debug
	Serial.println("Set repoListState = " + String(repoListState));
#endif
  }
  http.end();
}


void AsyncEspOTA::getRepoInformation(){
#ifdef __debug
	Serial.println("inside AsyncEspOTA::getRepoInformation()");
#endif
	HTTPClient http;
#ifdef __debug
	Serial.printf("[SERVER] Retrieving repo information from %s\n",repoPath.c_str());
#endif
	http.begin(WiFiclient,repoPath.c_str());
	int httpResponseCode = http.GET();
	if (httpResponseCode > StateNoData){
		repoInformation = http.getString();
		repoInformationState = StateDataUpdated;
#ifdef __debug
		Serial.println("Set repoInformationState = " + String(repoInformationState));
		Serial.printf("[SERVER] repo info retrieved: %s\n",repoInformation.c_str());
#endif
		http.end();
	}
	else
	{
#ifdef __debug
		Serial.printf("[SERVER] Could not get repo information: %s\n", http.errorToString(httpResponseCode).c_str());
#endif
		repoInformationState = StateError;
#ifdef __debug
		Serial.println("Set repoInformationState = " + String(repoInformationState));
#endif
		http.end();
	}
}


void AsyncEspOTA::loop(){
/*	
#ifdef __debug
	Serial.println("inside AsyncEspOTA::loop()");	
	Serial.println("repoListState = " + String(repoListState));
 #endif
*/	

    if (millis()-savedTime>5000){
        savedTime=millis();
		wifiMulti.run();
		if(wifiMulti.run() != WL_CONNECTED) {
			Serial.println("[AsyncOTa.loop] WiFi not connected!");
		} else {
			Serial.println("[AsyncOTa.loop] WiFi is connected!");
		}	  
    }

	if(SocketStarted){			// wait for webSockets onnection before 
		switch(socketData){
			case SCANSOCKET:
				break;
			case SAVEDSOCKET:
				break;
			case PROGRESSSOCKET:
				repoInstallationState = StateNeedUpdate;	// Loop will update on next run cycle
				SocketStarted=false;
				break;
			case DISCONNECTED:
				SocketStarted = false;
				break;
			default:
				break;
		}
	}
	
	if (repoListState == StateNeedUpdate)    {
		repoListState = StateNoData;

		getRepoList();
    }
    if (repoInformationState == StateNeedUpdate && !repoPath.isEmpty())    {
		repoInformationState = StateNoData;
		getRepoInformation();
    }

    if (repoInstallationState == StateNeedUpdate)    {
		repoInstallationState = StateNoData;
		installFromRepo();
    }
}


// Serve up the wiFiSettings web page
void AsyncEspOTA::onWiFiIndexRequest(AsyncWebServerRequest *request){
#ifdef __debug
	Serial.println("inside AsyncEspOTA::handleWiFiRequest");
	printRequestDetails(request);
#endif   
	String completeWifiPage = styleCSS;
	completeWifiPage += wifiPageA;
	completeWifiPage += wsPort;
	completeWifiPage += wifiPageB;
	
	request->send(200, "text/html", completeWifiPage);
}


void AsyncEspOTA::onWiFiSavedRequest(AsyncWebServerRequest *request){
#ifdef __debug
	Serial.println("inside AsyncEspOTA::onWifiSavedRequest");
	printRequestDetails(request);
#endif 


/*	[
		{"ns":"wlan-0","ssid":"","pass":""},
		{"ns":"wlan-1","ssid":"","pass":""},
		{"ns":"wlan-2","ssid":"","pass":""},
		{"ns":"wlan-3","ssid":"","pass":""},
		{"ns":"wlan-4","ssid":"","pass":""},
		{"ns":"wlan-5","ssid":"","pass":""},
		{"ns":"wlan-6","ssid":"","pass":""},
		{"ns":"wlan-7","ssid":"","pass":""},
		{"ns":"wlan-8","ssid":"","pass":""},
		{"ns":"wlan-9","ssid":"","pass":""}
	]
	*/
	String json = "[";
        for (int i = 0; i < 30; ++i) {
			//open Preferences in Read Only mode (true)
			if(preferences.begin(nameSpaces[i], false)){	//Namespace = wlan-[0-9]
					
				// If the preference exists...
				if(preferences.getString("ssid","FF") != "FF"){
					if(i) json += ",";							// only add a comma if not first. last cant reach here
					json += "{";
					json += "\"ns\":\"wlan-" + String(i) + "\",";		// namespace
					json += "\"ssid\":\""     + preferences.getString("ssid")    + "\",";
					json += "\"pass\":\"" + preferences.getString("password") + "\"";
					json += "}";
					preferences.end();
				} else {							// If the preference doesn't exists...
					if(i) json += ",";								// only add a comma if not first. last cant reach here
					json += "{";
					json += "\"ns\":\"wlan-" + String(i) + "\",";	// valid namespace 0-9
					json += "\"ssid\":\"\",";
					json += "\"pass\":\"\"}";	//empty string place holders
					preferences.end();
					break;						//exit the loop

				}
			}
        }
    json += "]";
	request->send(200, "text/json", json);
	Serial.println(json);
}


void AsyncEspOTA::onWiFiPost(AsyncWebServerRequest *request){  
//, uint8_t *data, size_t len, size_t index, size_t total
#ifdef __debug
	Serial.println("inside AsyncEspOTA::onWiFiPost");
	printRequestDetails(request);
#endif   

// read the body of the POST and save the data to NVS preference memory
    Serial.print("request->args() = ");
    Serial.println(request->args());
	
	//for(i=0;i<params;i++){
		AsyncWebParameter* param = request->getParam(i);	  
		if(param->isPost()){
			Serial.printf("POST[%s]: %s\n", param->name().c_str(), param->value().c_str());
			newPref.NameSpace = param->value(0).c_str();
			newPref.Key = param->value(1);
			newPref.Value = param->value(2);
		}
	//}
/*	if (request->args() != 0){
      for (int i = 0; i < request->args(); i++) {
        if (request->argName(i).startsWith("ns")){
          newPref.NameSpace = request->arg(i).c_str();
        }
		if (request->argName(i).startsWith("ssid")){
			newPref.Key = request->argName(i);
        }
		if (request->argName(i).startsWith("pass")){
			newPref.Value = request->arg(i);
        }
*/		
		Serial.print("ns    =");
		Serial.println(newPref.NameSpace);
		Serial.print("key   =");
		Serial.println(newPref.Key);
		Serial.print("value =");
		Serial.println(newPref.Value);
        //SavePrefs (&currentPref);
      }
    }
		//SavePref (&currentPref);
    request->send(200, "text/html", "saved");   // show the "Saved" notification message
	//request->send(200, "text/plain", "ok");
}


void AsyncEspOTA::onWiFiScanRequest(AsyncWebServerRequest *request){
#ifdef __debug
	Serial.println("inside AsyncEspOTA::handleWifiScan");
	printRequestDetails(request);
#endif   


	//First request will return 0 results unless you start scan from somewhere else (loop/setup)
	//Do not request more often than 3-5 seconds
	String json = "[";
	int n = WiFi.scanComplete();
	if(n == -2){
		WiFi.scanNetworks(true);
	} else if(n){
		for (int i = 0; i < n; ++i){
			if(i) json += ",";
			json += "{";
			json += "\"rssi\":\"" + String(WiFi.RSSI(i)) +"\",";
			json += "\"enc\":\"";
			json += (WiFi.encryptionType(i) == WIFI_AUTH_OPEN)? "O\"," : "C\",";

			if  (WiFi.SSID(i)==WiFi.SSID()){ 	// is the the network we are connected to??
				json += "\"cnctd\":\"true\",";
			} else {
				json += "\"cnctd\":\"false\",";
			}

			json += "\"ssid\":\""+WiFi.SSID(i) +"\"";
			json += "}";
		}
		WiFi.scanDelete();
		if(WiFi.scanComplete() == -2){
		  WiFi.scanNetworks(true);
		}
	  }
	  json += "]";
	  Serial.println(json);
	  request->send(200, "text/json", json);
	  json = String();
}


void AsyncEspOTA::handleFirmware(AsyncWebServerRequest *request){
#ifdef __debug
	Serial.println("inside AsyncEspOTA::handleFirmware");
	printRequestDetails(request);
#endif   
	
	String completeProgressPage = styleCSS;
	completeProgressPage += updatePage;
	
	if (allowAppsURL){
		completeProgressPage += updatePageApps;
	}
	if (allowCustomPaths){
		completeProgressPage += updatePageCustom;
	}
	
	if (allowLocal){
		completeProgressPage += updatePageLocal;
	}

	completeProgressPage += updatePageBlocker;
	//completeProgressPage += progressPageA2;
	completeProgressPage += wsPort;
	completeProgressPage += progressPageB;
		
	request->send(200, "text/html", completeProgressPage);
}


void AsyncEspOTA::handleRepoList(AsyncWebServerRequest *request){
#ifdef __debug
    Serial.println("inside AsyncEspOTA::handleRepoList");
	printRequestDetails(request);
#endif
	repoListState = StateNeedUpdate;
#ifdef __debug
	Serial.println("Set repoListState = " + String(repoListState));
#endif
    request->send(200, "text/plain", "ok");
}


void AsyncEspOTA::checkRepoList(AsyncWebServerRequest *request) {
#ifdef __debug
    Serial.println("inside AsyncEspOTA::checkRepoList");
	printRequestDetails(request);
#endif
	if (repoListState == StateDataUpdated) {
#ifdef __debug
		Serial.println("sending repoList to request client");
		Serial.println(repoList);
#endif
        request->send(200, "text/json", repoList);
    } else {
#ifdef __debug
		Serial.println("sending repoListState # to request client");
#endif
        request->send(200, "text/plain",String(repoListState));
    }
}

void AsyncEspOTA::handleRepoInformation(AsyncWebServerRequest *request){
#ifdef __debug
    Serial.println("inside AsyncEspOTA::handleRepoInformation");
	printRequestDetails(request);
#endif
	if (request->hasArg("repo")) {
        repoPath = request->arg("repo") + "versions.json";
        repoInformationState = StateNeedUpdate;
#ifdef __debug
		Serial.println("Set repoInformationState = " + String(repoInformationState));
#endif
        request->send(200, "text/plain", "ok");
        return;
    }
    request->send(503, "text/plain", "Parameters missing");
}


void AsyncEspOTA::checkRepoInformation(AsyncWebServerRequest *request) {
#ifdef __debug
    Serial.println("inside AsyncEspOTA::checkRepoInformation");
	printRequestDetails(request);
#endif
	if (repoInformationState == StateDataUpdated) {
#ifdef __debug
		Serial.println("sending repoListInformation to request client");
		Serial.println(repoInformation);
#endif
        request->send(200, "text/json",repoInformation);
    } else {
#ifdef __debug
		Serial.println("sending repoListInformationState # to request client");
#endif
        request->send(200, "text/plain",String(repoInformationState));
    }
}


void AsyncEspOTA::handleRestart(AsyncWebServerRequest *request) {
#ifdef __debug
    Serial.println("inside AsyncEspOTA::handleRestart");
	printRequestDetails(request);
#endif
	request->send(200, "text/plain", "OK");
    ESP.restart();
}


void AsyncEspOTA::handleInstallFromRepo(AsyncWebServerRequest *request) {
#ifdef __debug
    Serial.println("inside AsyncEspOTA::handleInstallFromRepo");
	printRequestDetails(request);
#endif
	// remote = request->client()->remoteIP();
	
	if (request->hasArg("spiffsPath")) {
        spiffPath = request->arg("spiffsPath");
        repoInstallationState = StateNeedUpdate;
#ifdef __debug
		Serial.println("Set repoInstallationState = " + String(repoInstallationState));
#endif
    }
    if (request->hasArg("binPath")) {
        binPath = request->arg("binPath");
		repoInstallationState = StateNeedUpdate;
#ifdef __debug
		Serial.println("Set repoInstallationState = " + String(repoInstallationState));
#endif
    }
	request->send(200, "text/plain",  "OK");
}


//	SCANSOCKET
//	SAVEDSOCKET		??
//	PROGRESSSOCKET
//	DISCONNECTED
void AsyncEspOTA::setCurrentClient(uint8_t client_Num, socketDataType type){
#ifdef __debug
	Serial.println(" we have a webSocket client");
#endif
	clientNum = clientNum;
	SocketStarted=true;
	socketData=type;
}


void AsyncEspOTA::printRequestDetails(AsyncWebServerRequest *request){
	int i;

	Serial.println("request details:");

	Serial.println(request->version());       // uint8_t: 0 = HTTP/1.0, 1 = HTTP/1.1
	Serial.println(request->method());        // enum:    HTTP_GET, HTTP_POST, HTTP_DELETE, HTTP_PUT, HTTP_PATCH, HTTP_HEAD, HTTP_OPTIONS
	Serial.println(request->url());           // String:  URL of the request (not including host, port or GET parameters)
	Serial.println(request->host());          // String:  The requested host (can be used for virtual hosting)
	Serial.println(request->contentType());   // String:  ContentType of the request (not avaiable in Handler::canHandle)
	Serial.println(request->contentLength()); // size_t:  ContentLength of the request (not avaiable in Handler::canHandle)
	Serial.println(request->multipart());     // bool:    True if the request has content type "multipart"
	
	//List all collected headers
	int headers = request->headers();
	  for(i=0;i<headers;i++){
	  AsyncWebHeader* h = request->getHeader(i);
	  Serial.printf("HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
	}

	//List all collected headers (Compatibility)
	//int headers = request->headers();
	//int i;
	for(i=0;i<headers;i++){
	  Serial.printf("HEADER[%s]: %s\n", request->headerName(i).c_str(), request->header(i).c_str());
	}

	//get specific header by name (Compatibility)
	if(request->hasHeader("MyHeader")){
	  Serial.printf("MyHeader: %s\n", request->header("MyHeader").c_str());
	}
	
		//Check if GET parameter exists
	if(request->hasParam("download"))
	  AsyncWebParameter* p = request->getParam("download");

	//Check if POST (but not File) parameter exists
	if(request->hasParam("download", true))
	  AsyncWebParameter* p = request->getParam("download", true);

	//Check if FILE was uploaded
	if(request->hasParam("download", true, true))
	  AsyncWebParameter* p = request->getParam("download", true, true);

	//List all parameters
	int params = request->params();
	for(i=0;i<params;i++){
	  AsyncWebParameter* p = request->getParam(i);
	  if(p->isFile()){ //p->isPost() is also true
		Serial.printf("FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
	  } else if(p->isPost()){
		Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
	  } else {
		Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
	  }
	}
  
	//List all parameters (Compatibility)
	int args = request->args();
	for(i=0;i<args;i++){
	  Serial.printf("ARG[%s]: %s\n", request->argName(i).c_str(), request->arg(i).c_str());
	}
}