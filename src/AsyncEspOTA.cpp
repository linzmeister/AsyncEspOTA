#include "AsyncEspOTA.h"
#include "AsyncEspOTA.h"

String messagePrefix;	// s = SPIFFS, b = Binary Firmawre file
String message;	
uint8_t clientNum;

IPAddress local_ip(192,168,4,1);		// for AP!!
IPAddress gateway;						// set to STA's DHCP server gateway after connecting to STA
IPAddress subnet(255,255,255,0);

IPAddress primaryDNS(8, 8, 8, 8);		// for STA
IPAddress secondaryDNS(8, 8, 4, 4);		// for STA

bool wifiConnected=true;

const int pinLed = 2;

/*
const int HOMENETWORK     = 1;  //make these and ENUM ??
const int AWAYNETWORK     = 2; 
const int HOTSPOTNETWORK  = 3;  //wlan-0, wlan-1
*/


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


void onWiFiEvent(WiFiEvent_t event){
	switch (event) { 
		case SYSTEM_EVENT_STA_CONNECTED:
			log_d("[WiFi_Event] Connected to WiFi Network " + String(WiFi.SSID()));
			
			gateway = WiFi.gatewayIP();
			log_d("gateway should now be set to : " + String(gateway));
			
			// only use mDNS if connected to another AP - 
			//use mdns for host name resolution
			if (MDNS.begin(WiFi.softAPSSID().c_str())) {
				log_d("MDNS responder started");
				log_d("You can now connect to http://");
				log_d(WiFi.softAPSSID() + ".local");     //http://AsyncEspOTAxxxxx.local/
				
			} else {
				log_d("Error setting up MDNS responder!");
			}
			//digitalWrite(pinLed,HIGH);
			break;
      
		case SYSTEM_EVENT_STA_DISCONNECTED:
			log_i("[WiFi_Event] Disconnected from WiFi Network");
			//digitalWrite(pinLed,LOW);
			
			//WiFi.reconnect();   // check if this will come back to bit us??
			
			wifiMulti.run();
			break;

//		case ARDUINO_EVENT_WIFI_AP_START:
//			log_d("[WiFi_Event] IP Address of Access Point: %s", WiFi.softAPIP());
//			break;
		
		case SYSTEM_EVENT_AP_STOP:
			log_d("[WiFi_Event] WiFi access point stopped");
			break;

		case SYSTEM_EVENT_STA_STOP:
			log_d("[WiFi_Event] WiFi client stopped - no more internet access");
			break;
		
/*		case ARDUINO_EVENT_WIFI_STA_GOT_IP:
				log_d("[WiFi_Event] Got IP: %S", String(WiFi.localIP()));
				break;
*/		
		default: break;
	}
}


const char *wl_status_to_string(wl_status_t status){
  switch (status) {
    case WL_NO_SHIELD:
      return "WL_NO_SHIELD";
    case WL_IDLE_STATUS:
      return "WL_IDLE_STATUS";
    case WL_NO_SSID_AVAIL:
      return "WL_NO_SSID_AVAIL";
    case WL_SCAN_COMPLETED:
      return "WL_SCAN_COMPLETED";
    case WL_CONNECTED:
      return "WL_CONNECTED";
    case WL_CONNECT_FAILED:
      return "WL_CONNECT_FAILED";
    case WL_CONNECTION_LOST:
      return "WL_CONNECTION_LOST";
    case WL_DISCONNECTED:
      return "WL_DISCONNECTED";
    default:
      return "UNKNOWN";
  }
}


AsyncEspOTA::AsyncEspOTA(){
}


void AsyncEspOTA::begin(){
	
	for(int i=0; i<17; i=i+8) {
	chipID |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
	}
	String Temp = String(ssidPrefix) + String(chipID);
	APssid=(Temp.c_str());
	
	WiFi.onEvent(onWiFiEvent);
	
	tryCredentials();		// read Preferences and add to WifiMulti to connect.
	
	if(!SPIFFS.begin()){
		log_w("An Error has occurred while mounting SPIFFS");
	} else {
		log_d("SPIFFS mounted successfully");
	}
	
	/*
		sequence of event handlers is important   !!
		Longest branch paths first
		specific >>> general
	*/

	log_d("Setting up web server responders");
/* ***********************************************************************************************************************	
  // Simple Firmware Update Form

  webServer.on("/update", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html",
	"<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>");
  });

//Browser's local OTA file update function to use as is or modify ....
 
 webServer.on("/update", HTTP_POST, [](AsyncWebServerRequest *request){
    bool shouldReboot = !Update.hasError();
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", shouldReboot?"OK":"FAIL");
    response->addHeader("Connection", "close");
    request->send(response);
  },[](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
    if(!index){
      Update.onProgress(updateProgress);  //progressCallbackFunction  
      log_d("Update Start: %s\n", filename.c_str());
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
        log_d("Update Success: %uB\n", index+len);
      } else {
        Update.printError(Serial);
      }
    }
  });
// ************************************************************************************************************************	
*/		

	// respond to the restart button from the client 
	webServer.on("/restart", HTTP_GET, [this](AsyncWebServerRequest *request){
		handleRestart(request);               
     });
	 
	//  read preferences - serve list to client
	webServer.on("/wifiSettings/saved", HTTP_GET, [this](AsyncWebServerRequest *request) {
		onWiFiSavedRequest(request);
	});
	
	//  - Scan WiFi, serve list to client
	webServer.on("/wifiSettings/scan", HTTP_GET, [this](AsyncWebServerRequest *request) {
		 onWiFiScanRequest(request);
	});

	// Client sends a body data of nameSpace, ssid & paasswords to save
	// Save wifiSettings posted from the page to preferences nv flashram
    webServer.on("/wifiSettings/saveWlan", HTTP_POST, [this](AsyncWebServerRequest * request){
		onWiFiSavePost(request);
	});
	
		// Client sends a body data of nameSpace, ssid & paasswords to save
	// Save wifiSettings posted from the page to preferences nv flashram
    webServer.on("/wifiSettings/deleteWlan", HTTP_POST, [this](AsyncWebServerRequest * request){
		onWiFiDeletePost(request);
	});
	
	// serve the /wifiSettings.hmtl page to client 
	webServer.on("/wifiSettings", HTTP_GET, [this](AsyncWebServerRequest *request){
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

// ************************************************************************************************************************		
	//webServer.serveStatic("/", SPIFFS, "/www/").setFilter(ON_STA_FILTER);
	//webServer.serveStatic("/", SPIFFS, "/ap/").setFilter(ON_AP_FILTER);
// ************************************************************************************************************************	

	// attach filesystem root at URL /images
    webServer.serveStatic("/images", SPIFFS, "/");

	webServer.on("/updateSPIFFS", HTTP_GET, [this](AsyncWebServerRequest *request){
        onUpdateSPIFFS(request);               
     });
	 
	WiFi.scanNetworks();	// to speed up the first web page results.
	
	//See discussion: https://github.com/RomeHein/ESPInstaller/issues/2
	WiFiclient.setInsecure();
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
      log_d("Update Start: %s\n", filename.c_str());
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
        log_d("Update Success: %uB\n", index+len);
      } else {
        Update.printError(Serial);
      }
    }
  });
}
*/


/*
void AsyncEspOTA::OnWiFiEvent(WiFiEvent_t event, arduino_event_info_t info){
  switch (event) { 
		case ARDUINO_EVENT_WIFI_STA_START:
			log_d("Station Mode Started");
			break;
    
		case ARDUINO_EVENT_WIFI_STA_GOT_IP:
			log_d("Connected to :" + String(WiFi.SSID()));
			log_d("Got IP: %S", String(WiFi.localIP()));
			break;
      
		case SYSTEM_EVENT_STA_CONNECTED:
			log_d("Connected to WiFi Network");
		break;
      
		case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
			log_d("Disconnected from WiFi Network");
			WiFi.reconnect();   // check if this will come back to bit us in the little donkey.
			break;

		case ARDUINO_EVENT_AP_STOP:
			log_d("WiFi access point stopped");
			break;

		case ARDUINO_EVENT_STA_STOP:
			log_d("WiFi clients stopped");
			break;

		case ARDUINO_EVENT_WPS_ER_SUCCESS:
			log_d("WPS Successful, stopping WPS and connecting to: " + String(WiFi.SSID()));
			esp_wifi_wps_disable();
			delay(10);
			WiFi.begin();
			break;
    
		case ARDUINO_EVENT_WPS_ER_FAILED:
			log_d("WPS Failed, retrying");
			esp_wifi_wps_disable();
			esp_wifi_wps_enable(&config);
			esp_wifi_wps_start(0);
			break;
    
		case ARDUINO_EVENT_WPS_ER_TIMEOUT:
			log_d("WPS Timeout, retrying");
			esp_wifi_wps_disable();
			esp_wifi_wps_enable(&config);
			esp_wifi_wps_start(0);
			break;
    
		case ARDUINO_EVENT_WPS_ER_PIN:
			log_d("WPS_PIN = " + wpspin2string(info.wps_er_pin.pin_code));
			break;
            
    default: break;
  }
}
*/


void  AsyncEspOTA::startAP(){ 
  log_d("Starting (Access Point) with ssid : " + String(APssid));
  // Remove the password parameter, if you want the AP (Access Point) to be open


  WiFi.persistent(false);
  WiFi.setAutoReconnect(true);	// WiFi Multi should take care of this
  WiFi.setSleep(false);
  
  //  TODO:  Allow user to choose their own APpassword, 
  // but to also reset to factory default if they forget
  WiFi.softAP(APssid, APpassword);      
  WiFi.softAPConfig(local_ip, gateway, subnet);
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

  log_d("Starting WPS");

  wpsInitConfig();
  esp_wifi_wps_enable(&config);
  esp_wifi_wps_start(0);
}


void AsyncEspOTA::wpsStop(){
    if(esp_wifi_wps_disable()){
    	log_d("WPS Disable Failed");
    }
}



/** Attempt to read existing preferences.
    Then attempt to start Wi-Fi client with any saved credentials
*/
bool AsyncEspOTA::tryCredentials(){
	log_d("inside tryCredentials");
	bool resp = false;
	
/*	
	// flash the onboard LED !!
	pinMode(pinLed,OUTPUT);
	for(int i=0; 1<5; i++){
		digitalWrite(pinLed,HIGH);
		delay(500);
		digitalWrite(pinLed,LOW);
		delay(500);
	}
*/		
	
	String temp;
	for (int i = 0; i < maxCredentials; i++) {
		temp = "wlan-" + String(i);
		log_e("trying to read preference : %S", temp);
		currentCredentials.nameSpace=temp.c_str();
			
		if(readCredentials(&currentCredentials)){
			log_e("ssid key exists");				
			if(currentCredentials.ssid != ""){
				log_e("adding %s to WiFi-Multi list", currentCredentials.ssid);
				wifiMulti.addAP(currentCredentials.ssid.c_str(), currentCredentials.password.c_str());
				savedCredentials++;
				log_e("savedCredentials = %i ", savedCredentials);
				resp=true;
			}
		} else {
			log_e("readCredentials returned false");
		}
    }
	
	WiFi.mode(WIFI_MODE_STA);
	
	int i=0;
	log_d("Starting wifiMulti");
	while(wifiMulti.run() != WL_CONNECTED) {
		log_d("WiFi multi checking for networks to connect to");
		delay(6000);
		i++;
		if (i>3){
			log_d("[AsyncEspOTA.begin] No saved networks found");
			break;	
		}
	}

	if (WiFi.status() == WL_CONNECTED){
		wifiConnected=true;
	} else {
		wifiConnected=false;
	}
	
	WiFi.mode(WIFI_MODE_APSTA);
	startAP();	
    return resp;
}


/**   Accepts a Credentials pointer
 *    Updates the pointed to Struct
 */
bool AsyncEspOTA::readCredentials (Credentials *readCred){
	bool resp = false;
	if (preferences.begin(readCred->nameSpace, RO_MODE)){
		log_e("nameSpace readable");
		if(preferences.isKey("ssid")){
			readCred->wlanID = preferences.getInt("wlanID", -1);
			readCred->ssid = preferences.getString("ssid", "FF");
			readCred->password = preferences.getString("password", "FF");
			if(readCred->wlanID != -1 && 
			   readCred->ssid != "FF" && 
			   readCred->password != "FF"){
				log_d("valid credentials found !-1, !FF etc");
				preferences.end();
				 resp = true;
			}
		} else {
			log_e("no Credentials present");
		}
	}
	preferences.end();
	return resp;
}


bool AsyncEspOTA::saveCredentials(Credentials *saveCred){
	bool resp = false;
	if(saveCred->ssid != ""){
		preferences.begin(saveCred->nameSpace, RW_MODE);
		preferences.putInt("wlanID", saveCred->wlanID);
		preferences.putString("ssid", saveCred->ssid);
		preferences.putString("password", saveCred->password);
		resp=true;
	}
	preferences.end();
	log_e("saved ");
	return resp;
}


void AsyncEspOTA::loop(){
	log_v("inside AsyncEspOTA::loop()");	
	log_v("repoListState = %S", String(repoListState));

	if (wifiConnected){
		if (millis()-savedTime>6000){	// don't scan too frequenetly..  minumum 5 seconds
			savedTime=millis();
			wifiMulti.run();
		}
	}
	
	if(SocketStarted){			// wait for webSockets onnection before 
		switch(socketData){
			/*
			case SCANSOCKET:
				break;
			case SAVEDSOCKET:
				break;
			*/	
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
	log_d("inside AsyncEspOTA::handleWiFiRequest");
	log_v(printRequestDetails(request));
	String completeWifiPage = styleCSS;
	completeWifiPage += wifiPageA;
	completeWifiPage += wsPort;
	completeWifiPage += wifiPageB;
	
	request->send(200, "text/html", completeWifiPage);
}


void AsyncEspOTA::onWiFiSavedRequest(AsyncWebServerRequest *request){
	log_d("inside AsyncEspOTA::onWifiSavedRequest");
	log_v(printRequestDetails(request));

	
	savedCredentials=0;
	String json = "[";
        for (int i = 0; i < maxCredentials; ++i) {
			//open Preferences in Read Only mode (true)
			String temp = "wlan-" + String(i);
			currentCredentials.nameSpace=temp.c_str();
			log_e("trying to read preference : %S", temp);
			
			if(readCredentials(&currentCredentials)){
					if(json.length()>3) json += ",";	// only add a comma if not first... and last cant reach here
					json += "{";
					json += "\"ns\":\"" 	+ String(currentCredentials.nameSpace) + "\",";
					json += "\"id\":\"" + String(currentCredentials.wlanID) + "\",";
					json += "\"ssid\":\""   + currentCredentials.ssid + "\",";
					json += "\"pass\":\"" 	+ currentCredentials.password + "\",";
					
					if  (currentCredentials.ssid == WiFi.SSID()){ 	// is this the network we are connected to??
						json += "\"cnctd\":\"true\"";				// let the client know..
					} else {
						json += "\"cnctd\":\"false\"";				// also if not
					}
					json += "}";
				savedCredentials++;
				log_e("savedCredentials = %i ", savedCredentials);
			}
        }
    json += "]";
	if (json.length()>3){
		request->send(200, "text/json", json);	
		log_d(json);
	} else {
		request->send(200, "text/json", "[]");
		log_d("no Saved settings to send, sending empty set to client");
	}
}


void AsyncEspOTA::onWiFiScanRequest(AsyncWebServerRequest *request){
	log_d("inside AsyncEspOTA::handleWifiScan");
	log_v(printRequestDetails(request));


	String json = "[";
	
	log_d("current wifi mode is %i", WiFi.getMode());	//??  returns an Int??  potential crahs point
	log_d(WiFi.getMode());
	
	//First request will return 0 results unless you start scan from somewhere else (loop/setup)
	//Do not request more often than 3-5 seconds
	int n = WiFi.scanComplete();
	
	//log_d("scanComplete() = "  + String(n));
	int found;
	
	if(n == WIFI_SCAN_FAILED){
		log_d("WiFi scan failed");
		WiFi.scanNetworks(true);
	} else if(n){
		for (int i = 0; i < n; ++i){
			found=json.indexOf(WiFi.SSID(i));
			if(found == -1){		//show no duplicates - different channels, 5.4/5 GHz bands
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
		}
		WiFi.scanDelete();
		if(WiFi.scanComplete() == -2){
		  WiFi.scanNetworks(true);
		}
	  }
	  json += "]";
	  request->send(200, "text/json", json);
	  json = String();
}


void AsyncEspOTA::onWiFiSavePost(AsyncWebServerRequest *request){
	log_d("inside AsyncEspOTA::onWiFiSavePost");
	log_v(printRequestDetails(request));

	// check if these are existing or new credentials and 
	// savedCredentials++ if new
	
	// read the body of the POST and save the data to NVS preference memory
	AsyncWebParameter* param = request->getParam(0);
	currentCredentials.nameSpace = param->value().c_str();
	log_d("%s", currentCredentials.nameSpace);

	param = request->getParam(1);
	currentCredentials.wlanID = atoi(param->value().c_str());
	log_d("%i", currentCredentials.wlanID);

	param = request->getParam(2);
	currentCredentials.ssid = param->value();
	log_d("%s", currentCredentials.ssid);
	
	param = request->getParam(3);
	currentCredentials.password = param->value();
	log_d("%s", currentCredentials.password);
	
	if (currentCredentials.wlanID-savedCredentials==1) savedCredentials++;	//new network being saved
		
	saveCredentials (&currentCredentials);

    request->send(200, "text/html", "saved");   // show the "Saved" notification message
	tryCredentials();		// load saved settings into WiFi Multi to connect to saved network
}


void AsyncEspOTA::onWiFiDeletePost(AsyncWebServerRequest *request){
	log_d("inside AsyncEspOTA::onWiFiPostDelete");
	log_v(printRequestDetails(request));

	
	AsyncWebParameter* param = request->getParam(0);
	currentCredentials.wlanID = atoi(param->value().c_str());	//int in the range of 0 - 29
	
	String name;
	log_d("savedCredentials %i", savedCredentials);
	
	int indexToDelete =  atoi(param->value().c_str());	//convert char to int

	for(int i = 0; i < savedCredentials+1; i++){
		name = "wlan-" + String(i);
		
		if (i == indexToDelete){
			preferences.begin(name.c_str(), RW_MODE);
			if(preferences.isKey("ssid")){
				preferences.clear();
				preferences.end();
			}
		} else if (i > indexToDelete){
			// shuffle currentCredentials to lower number:
				// read existing values into currentCredentials,
				// change the currentCredentials.nameSpace & ID
				// save into new nameSpace
				
				currentCredentials.nameSpace= name.c_str();
				readCredentials(&currentCredentials);
				name = "wlan-" + String(i-1);
				currentCredentials.nameSpace = name.c_str();
				currentCredentials.wlanID--;
				saveCredentials(&currentCredentials);
		}
	}
	
	name = "wlan-" + String(savedCredentials-1);
	
	preferences.begin(name.c_str(), RW_MODE);
	log_d("attempting to delete nameSpace  = %s... THE LAST ONE!!", name);
	preferences.clear();
	preferences.end();
	
	savedCredentials--;
	log_d("savedCredentials %i\n", savedCredentials-1);
	request->send(200, "text/html", "deleted");   // show the "Deleted" notification message	
}


void AsyncEspOTA::handleFirmware(AsyncWebServerRequest *request){
	log_d("inside AsyncEspOTA::handleFirmware");
	log_v(printRequestDetails(request));
	
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
	completeProgressPage += wsPort;
	completeProgressPage += progressPageB;
		
	request->send(200, "text/html", completeProgressPage);
}


void AsyncEspOTA::handleRepoList(AsyncWebServerRequest *request){
    log_d("inside AsyncEspOTA::handleRepoList");
	log_v(printRequestDetails(request));
	
	repoListState = StateNeedUpdate;
	log_d("Set repoListState = " + String(repoListState));
    request->send(200, "text/plain", "ok");
}


void AsyncEspOTA::checkRepoList(AsyncWebServerRequest *request) {
    log_d("inside AsyncEspOTA::checkRepoList");
	log_v(printRequestDetails(request));

	if (repoListState == StateDataUpdated) {
		log_d("sending repoList to request client");
		log_d(repoList);
        request->send(200, "text/json", repoList);
    } else {
		log_d("sending repoListState # to request client");
        request->send(200, "text/plain", String(repoListState));
    }
}

void AsyncEspOTA::handleRepoInformation(AsyncWebServerRequest *request){
    log_d("inside AsyncEspOTA::handleRepoInformation");
	log_v(printRequestDetails(request));

	if (request->hasArg("repo")) {
        repoPath = request->arg("repo") + "versions.json";
        repoInformationState = StateNeedUpdate;
		log_d("Set repoInformationState = " + String(repoInformationState));
        request->send(200, "text/plain", "ok");
        return;
    }
    request->send(503, "text/plain", "Parameters missing");
}


void AsyncEspOTA::checkRepoInformation(AsyncWebServerRequest *request) {
    log_d("inside AsyncEspOTA::checkRepoInformation");
	log_v(printRequestDetails(request));
	
	if (repoInformationState == StateDataUpdated) {
		log_d("sending repoListInformation to request client");
		log_d(repoInformation);
        request->send(200, "text/json",repoInformation);
    } else {
		log_d("sending repoListInformationState # to request client");
        request->send(200, "text/plain",String(repoInformationState));
    }
}


void AsyncEspOTA::handleUpload(AsyncWebServerRequest *request){
    log_d("inside AsyncEspOTA::handleUpload");
	log_v(printRequestDetails(request));

	request->send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart(); 
}


void AsyncEspOTA::getRepoList(){
	log_d("inside AsyncEspOTA::getRepoList()");

	HTTPClient http;

	log_d("[SERVER] Retrieving repo list from %s\n", appsURL);

  http.begin(WiFiclient, appsURL);
  int httpResponseCode = http.GET();
  if (httpResponseCode > StateNoData)  {
	repoList = http.getString();
	repoListState = StateDataUpdated;

	log_d("Set repoListState = " + String(repoListState));
    log_d("[SERVER] repo list retrieved: \n%s\n",repoList.c_str());
  } else{

	log_d("[SERVER] Could not get repo list. Error %d: %s\n", httpResponseCode, http.errorToString(httpResponseCode).c_str());

	repoListState = StateError;
	log_d("Set repoListState = " + String(repoListState));
  }
  http.end();
}


void AsyncEspOTA::getRepoInformation(){
	log_d("inside AsyncEspOTA::getRepoInformation()");

	HTTPClient http;

	log_d("[SERVER] Retrieving repo information from %s\n",repoPath.c_str());

	http.begin(WiFiclient,repoPath.c_str());
	int httpResponseCode = http.GET();
	if (httpResponseCode > StateNoData){
		repoInformation = http.getString();
		repoInformationState = StateDataUpdated;

		log_d("Set repoInformationState = " + String(repoInformationState));
		log_d("[SERVER] repo info retrieved: %s\n",repoInformation.c_str());
		http.end();
	}
	else
	{
		log_d("[SERVER] Could not get repo information: %s\n", http.errorToString(httpResponseCode).c_str());
		repoInformationState = StateError;
		log_d("Set repoInformationState = " + String(repoInformationState));
		http.end();
	}
}


void AsyncEspOTA::handleRestart(AsyncWebServerRequest *request) {
    log_d("inside AsyncEspOTA::handleRestart");
	log_v(printRequestDetails(request));

	request->send(200, "text/plain", "OK");
    ESP.restart();
}


void AsyncEspOTA::handleInstallFromRepo(AsyncWebServerRequest *request) {
    log_d("inside AsyncEspOTA::handleInstallFromRepo");
	log_v(printRequestDetails(request));
	
	if (request->hasArg("spiffsPath")) {
        spiffPath = request->arg("spiffsPath");
        repoInstallationState = StateNeedUpdate;
		log_d("Set repoInstallationState = " + String(repoInstallationState));
    }
    if (request->hasArg("binPath")) {
        binPath = request->arg("binPath");
		repoInstallationState = StateNeedUpdate;
		log_d("Set repoInstallationState = " + String(repoInstallationState));
    }
	request->send(200, "text/plain",  "OK");
}


void AsyncEspOTA::installFromRepo(void){
	log_d("inside AsyncEspOTA::installFromRepo(void)");	

  if (!spiffPath.isEmpty()){		
    log_d("[OTA] start SPIFFS download from: %s\n",spiffPath.c_str());

	Update.onProgress(updateProgress);	//progressCallbackFunction
	
	// initialise message with for 's' or SPIFFS 
	messagePrefix = "s";

    t_httpUpdate_return spiffRet = httpUpdate.updateSpiffs(WiFiclient,spiffPath);
    switch (spiffRet)
    {
    case HTTP_UPDATE_FAILED:
		log_d("[OTA] Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
		webSocket.sendTXT(clientNum,"Upload incomplete");
      return;

    case HTTP_UPDATE_NO_UPDATES:
      log_d("[OTA] no updates");
		webSocket.sendTXT(clientNum,"No Updates available");
      return;

    case HTTP_UPDATE_OK:
      log_d("[OTA] SPIFFS updated");
      break;
    }
  }

  if (!binPath.isEmpty()){
    log_d("[OTA] start BIN download from: %s\n",binPath.c_str());

	// initialise message with 'b' for BIN 
	messagePrefix = "b";
	
    t_httpUpdate_return binRet = httpUpdate.update(WiFiclient, binPath);
    switch (binRet){
		case HTTP_UPDATE_FAILED:
			log_d("[OTA] Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
			repoInstallationState = StateError;
			log_d("Set repoInstallationState = " + String(repoInstallationState));
			webSocket.sendTXT(clientNum,"Upload incomplete");
			break;

		case HTTP_UPDATE_NO_UPDATES:
			log_d("[OTA] no updates");
			repoInstallationState = StateError;
			log_d("Set repoInstallationState = " + String(repoInstallationState));
			webSocket.sendTXT(clientNum,"No Updates available");
			break;

		case HTTP_UPDATE_OK:
			log_d("[OTA] BIN updated");
			repoInstallationState = StateDataUpdated;
			log_d("Set repoInstallationState = " + String(repoInstallationState));
			//	send a websocket message to notify client we have finished
			webSocket.sendTXT(clientNum,"Restarting");
			ESP.restart();
			break;
		}
  }
}


//	SCANSOCKET
//	SAVEDSOCKET		??
//	PROGRESSSOCKET
//	DISCONNECTED
void AsyncEspOTA::setCurrentClient(uint8_t client_Num, socketDataType type){
	log_d("[AsyncEspOTA::setCurrentClient] WebSocket client $i connected", clientNum);

	clientNum = clientNum;
	SocketStarted=true;
	socketData=type;
}

void AsyncEspOTA::onUpdateSPIFFS(AsyncWebServerRequest *request){
    log_d("inside AsyncEspOTA::handleInstallFromRepo");
	log_v(printRequestDetails(request));

	// download images from GitHub to SPIFFS

	// or use install from REPO - spiffs update logic???
/*	
	https://github.com/linzmeister/AsyncEspOTA/blob/main/images/SPIFFS/bar1.png
	https://github.com/linzmeister/AsyncEspOTA/blob/main/images/SPIFFS/bar2.png
	https://github.com/linzmeister/AsyncEspOTA/blob/main/images/SPIFFS/bar3.png
	https://github.com/linzmeister/AsyncEspOTA/blob/main/images/SPIFFS/bar4.png
	https://github.com/linzmeister/AsyncEspOTA/blob/main/images/SPIFFS/bar5.png
	https://github.com/linzmeister/AsyncEspOTA/blob/main/images/SPIFFS/open.png
	https://github.com/linzmeister/AsyncEspOTA/blob/main/images/SPIFFS/closed.png
*/

}

void AsyncEspOTA::printRequestDetails(AsyncWebServerRequest *request){
	int i;

	log_d("request details:");

	log_d(request->version());       // uint8_t: 0 = HTTP/1.0, 1 = HTTP/1.1
	log_d(request->method());        // enum:    HTTP_GET, HTTP_POST, HTTP_DELETE, HTTP_PUT, HTTP_PATCH, HTTP_HEAD, HTTP_OPTIONS
	log_d(request->url());           // String:  URL of the request (not including host, port or GET parameters)
	log_d(request->host());          // String:  The requested host (can be used for virtual hosting)
	log_d(request->contentType());   // String:  ContentType of the request (not avaiable in Handler::canHandle)
	log_d(request->contentLength()); // size_t:  ContentLength of the request (not avaiable in Handler::canHandle)
	log_d(request->multipart());     // bool:    True if the request has content type "multipart"
	
	//List all collected headers
	int headers = request->headers();
	  for(i=0;i<headers;i++){
	  AsyncWebHeader* h = request->getHeader(i);
	  log_d("HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
	}

	//List all collected headers (Compatibility)
	//int headers = request->headers();
	//int i;
	for(i=0;i<headers;i++){
	  log_d("HEADER[%s]: %s\n", request->headerName(i).c_str(), request->header(i).c_str());
	}

	//get specific header by name (Compatibility)
	if(request->hasHeader("MyHeader")){
	  log_d("MyHeader: %s\n", request->header("MyHeader").c_str());
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
		log_d("FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
	  } else if(p->isPost()){
		log_d("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
	  } else {
		log_d("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
	  }
	}
  
	//List all parameters (Compatibility)
	int args = request->args();
	for(i=0;i<args;i++){
	  log_d("ARG[%s]: %s\n", request->argName(i).c_str(), request->arg(i).c_str());
	}
}