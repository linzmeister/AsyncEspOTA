/*
    espBasicOTA

    This sketch demonstrates how to use the AsyncEspOTA Library.  The main aims 
    of the library are to make 3 things simple for end users of IoT retail products:
      1) initial "Out of Box" connection to product,
      2) configuring wifi settings of the product and 
      3) installing firmware updates to the product as new features and bug fixes are released.
    
    It is also a starting point for IoT based projects which will include webpages served by 
    the esp32 and the AsyncEspOTA library adds Over the Air firmware updates from a gitHub 
    repository, custom URL and web client local file systems.  
    
    It uses an AsyncWebServer and WebSocketsServer to display webpages and show firmware update progresss 
    including error messages on the /firmware web pages if the update fails.

    The library uses some JSON files to select which firmwware project and which version you wish to load 
    into the esp32.

    The sketch is broken up into several files to keep various elements of the code separated.  
    demo.h includes all the library references and Global variable declarations.
    demo_WebServer.ino includes the user webpages, event handlers and callback functions.
    demo_WiFi_Prefs.ino includes all the wifi initialisation functions, ssid and Password 
        settings which are saved from the web server and stored via the preferences library functions.

    If the library and supporting commands are added into multiple different projects, it is 
    possible to add them into the underlying JSON apps list so that they may be uploded Over The Air    
    
    The circuit:
    * esp32
    * no additional compulsory harware 
    
    Created 22 Nov 2022
    By Romehein
    https://github.com/RomeHein/ESPInstaller

    Modified 25 February 2023  (converted to Arduino Library with progress bar feedback and firmware source restrictions)
    By Lindsay McPhee

    https://github.com/linzmeister/AsyncEspOTA

*/

//Libraries **************************************************************************
#include <Arduino.h>
#include <WiFi.h>               // We're in IoT land, do as IoT does
#include <WiFiMulti.h>

#include <ESPAsyncWebServer.h>  // Asynchronous web server
#include <WebSocketsServer.h>   // allows OTAUpdate progress bar to send increasing values to browser page.

#include <AsyncEspOTA.h>        //library under test !!!

//Initialize the libraries  **********************************************************
WiFiMulti wifiMulti;

const int wsPort = 1337;            // variableName must match library decalaration..        declared extern in library..  Port value may be different

                                    //VariableName must match library decalaration..         declared extern in library..  
const char *appsURL= "https://raw.githubusercontent.com/linzmeister/AsyncEspOTA/main/DemoRepo/apps.json";

AsyncWebServer webServer(80);       // variableName must match library object declaration..  declared extern in library..  Port value may be different
WebSocketsServer webSocket(wsPort); // variableName must match library object declaration..  declared extern in library..

bool allowAppsURL = true;
bool allowCustomPaths = false;
bool allowLocal = false;

AsyncEspOTA myUpdater;

// Global variables, Pin Definitions, error constants etc *************************************
//IPAddress local_ip(192,168,4,1);
//IPAddress gateway(192,168,4,1);
//IPAddress subnet(255,255,255,0);

//const char *ssid     = "218";
//const char *password = "m0ng00se";

//const char *password = "G142WicksRoad";
//const char *ssid     = "Goodson";

const char *APssid = "AsyncEspOTA";
const char *APpassword = "";
const char *ssidPrefix="Thing";

String root(); 

//forward declarations to prevent compiler errors
//void onPageNotFound(AsyncWebServerRequest *request);
//void onWebSocketEvent(uint8_t clientNum, WStype_t type, uint8_t * payload, size_t length);


void setup(void){
  Serial.begin(115200);
  while(!Serial);
 
  WiFi.mode(WIFI_MODE_STA);
    long searchStartTime = millis();


    wifiMulti.addAP("Goodson", "G142WicksRoad");
    wifiMulti.addAP("218", "m0ng00se");
    
    Serial.println("Connecting Wifi...");
    if(wifiMulti.run() == WL_CONNECTED) {
        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
    }


/*    
      Serial.print("Attempting to connect to : "); 
      Serial.print(ssid);
      Serial.print(" ");
      Serial.println(password);
  
      WiFi.begin(ssid, password);
      
      while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
        if (millis() - searchStartTime > 20000){  // keep trying to connect for 20 seconds, then give up - it ain't happening
          WiFi.disconnect();
          Serial.println(" ");
          Serial.println("Connection timed out.");
        }
        Serial.print("IP address : ");
        Serial.println(WiFi.localIP());
      }
*/
    webServer.on("/", HTTP_GET, onRootIndexRequest);
    webServer.onNotFound([](AsyncWebServerRequest *request){onPageNotFound(request);});
    
    myUpdater.begin();
    webServer.begin();
    webSocket.begin();
    webSocket.onEvent(onWebSocketEvent);
 
}

void loop(){
    if(wifiMulti.run() != WL_CONNECTED) {
        Serial.println("WiFi not connected!");
        delay(1000);
    }
   // myUpdater.loop(); //checks to see if the the user has changed Firmware Update flags and responds accordingly
    webSocket.loop();
    delay(30);

    //Do other things that need to happen regularly for your project here
}


// Callback: receiving any WebSocket messages
void onWebSocketEvent(uint8_t clientNum, WStype_t type, uint8_t * payload, size_t length) {
  Serial.println("A web socket event has been received");
  
  // Figure out the type of WebSocket event
  switch(type) {

    // Client has disconnected
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", clientNum);
      break;

    // New client has connected
    case WStype_CONNECTED:
      Serial.println("connection request received");
      {
        IPAddress clientIP = webSocket.remoteIP(clientNum);
        Serial.printf("[%u] Connection from ", clientNum);
        Serial.println(clientIP.toString());
      }
      break;

    // Handle text messages from client
    case WStype_TEXT:
      // Print out raw message
      Serial.printf("[%u] Received text: %s\n", clientNum, payload);
      
// ****************************************************************************************************************************
      // Start sending progress bar info
      if(strcmp((char *)payload, "?FirmwareProgress") == 0 ) {
          webSocket.sendTXT(clientNum, "FirmwareProgress");      //just for debugging web client.. doesn't *do* anything
          
          // let myUpdater know which client requested a firmware update
//          myUpdater.setCurrentClient(clientNum, PROGRESSSOCKET);
      }
// ****************************************************************************************************************************

      else if(strcmp((char *)payload, "?SketchQuery1") == 0 ) 
      {
          // webSocket.sendTXT(clientNum, "query1");
          //Do Stuff 1
      }
      else if(strcmp((char *)payload, "?SketchQuery2") == 0 ) 
      {
          // webSocket.sendTXT(clientNum, "query2");
          // Do stuff 2
      
      }else{  // Message not recognized.. write more code!! ;) 
        Serial.printf("[%u] Message not recognized\n" ,clientNum, payload);
      }
      break;

    // For everything else: do nothing
    case WStype_BIN:
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    default:
      break;
  }
}


// Show the home page page
void onRootIndexRequest(AsyncWebServerRequest *request){
  const char* rootIndex =  root().c_str();
  request->send(200, "text/html", rootIndex);
}


void onPageNotFound(AsyncWebServerRequest *request) {
    IPAddress remote_ip = request->client()->remoteIP();
    Serial.println("[Error 404 " + remote_ip.toString() +
                    "] HTTP GET request of " + request->url());
    request->send(404, "text/plain", 
      "We honestly searched everywhere but \"We still... haven't found... what you're looking for\". :D ;)");
      request->send(404, "text/plain", 
      "[" + remote_ip.toString() +
                    "] HTTP GET request of " + request->url());
}

// the web server home page.. with a link that will trigger the library to show the firmware page
String root(){
 return "Go to the <a href=\"http://" + WiFi.localIP().toString() + "/firmware/\">firmware page</a>";
}
