/*
 *   Root index
 */ 
const char indexPageA[] PROGMEM = R"=====(<style>
  </style>
  <table width='90%' bgcolor='#33ccff' align='center'>
      <tr>
          <td colspan=3>
              <center><font size=5><b>
                  [Insert Product name here]
              </b></font></center>
          </td>
      </tr>
      <tr>
         <td colspan=3>Infrastructure IP Address =
              <A href="http://)=====";

const char indexPageB[] PROGMEM = R"=====(<br/>
           </td>
      </tr>
      <tr height='70'>
           <td>
                <a href=/firmware/><button>Update Firmware</button></a>
           </td>
           <td>
                <a href=/wifiSettings><button>WiFi Settings</button></a>
           </td>
            <td>
                <!-- a href=/settings><button></button></a -->
           </td>
      </tr>
      <tr height='70'>
           <td>
                <a href=/diagnostics><button>Run diagnostics</button></a>
           </td>
            <td>
                <a href=/totals><button>View total Counters</button></a>
           </td>
      </tr>
      <tr height='70'>
           <td>
                <a href=/logs><button>Download log data</button></a>
           </td>
           <td>
              <a href=/coolthingpage><button>do a cool thing</button></a>
           </td>
      </tr>
      
      <br>
      <br>
    </table>)=====";


String root(){
 String result = indexPageA
               + WiFi.localIP().toString()
               + "\">"
               + WiFi.localIP().toString()
               + "</a><p>"
               + "or use this url <A href=\""
               + "http://"
               + WiFi.softAPSSID()
               + ".local\">"
               + WiFi.softAPSSID()
               + ".local</a>"
               + " except on Android version <12."
               + indexPageB;
  return result;
}


//const char* saved[] PROGMEM = R"=====(
const char* saved =
  "<script>"
    "window.alert('Settings Saved');"
  "</script>";



// Show the home page page
void onRootIndexRequest(AsyncWebServerRequest *request){
  const char* rootIndex =  root().c_str();
  request->send(200, "text/html", rootIndex);
}


// Callback: receiving any WebSocket message
void onWebSocketEvent(uint8_t clientNum, WStype_t type, uint8_t * payload, size_t length) {
  Serial.println("A web socket event has been received");
  
  // Figure out the type of WebSocket event
  switch(type) {

    // Client has disconnected
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", clientNum);
// ****************************************************************************************************************************
      //myUpdater.setCurrentClient(clientNum, DISCONNECTED);
// ****************************************************************************************************************************      
      break;

    // New client has connected
    case WStype_CONNECTED:
      Serial.println("[sketch::onWebSocketEvent]connection request received");
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
      Serial.println(strcmp((char *)payload, "?FirmwareProgress"));
// ****************************************************************************************************************************
      // Start sending progress bar info
      if(strcmp((char *)payload, "?FirmwareProgress") == 0 ) {
          webSocket.sendTXT(clientNum, "FirmwareProgress");      //enables the Install buttons
          
          // let myUpdater know which client requested a firmware update
          //H4AsyncEspOTA.setCurrentClient(clientNum, PROGRESSSOCKET);
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


/*void onPageNotFound(AsyncWebServerRequest *request) {
    IPAddress remote_ip = request->client()->remoteIP();
    Serial.println("[Error 404 " + remote_ip.toString() +
                    "] HTTP GET request of " + request->url());
    request->send(404, "text/plain", 
      "We honestly searched everywhere but \"We still... haven't found... what your looking for\". :D ;)");
      request->send(404, "text/plain", 
      "[" + remote_ip.toString() +
                    "] HTTP GET request of " + request->url());
}
*/


void configureWebServer(){
    Serial.println("Starting web server");
    webServer.on("/", HTTP_GET, onRootIndexRequest);
/*    webServer.on("/settings", HTTP_GET, onSettingsIndexRequest); //GET  !!    
    webServer.on("/settings", HTTP_POST, onSettingsPost);        //POST !!
    
    webServer.on("/sync", HTTP_GET, functionName);           //TODO:  write functionName() or go lambda if you wish
    webServer.on("/diagnostics", HTTP_GET, functionName); 
    webServer.on("/totals", HTTP_GET, functionName); 
    webServer.on("/logs", HTTP_GET, functionName);           
    webServer.on("/coolthingpage", HTTP_GET, functionName); 
*/
    //webServer.onNotFound([](AsyncWebServerRequest *request){onPageNotFound(request);});  

}
