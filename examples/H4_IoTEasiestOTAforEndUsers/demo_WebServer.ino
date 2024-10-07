/*
 *   Root index
 */ 

const char indexPageA[] PROGMEM = R"=====(
  <style>
  body {
    text-align: center;
    font-family: helvetica;
    background-color: steelblue;
    margin: 0;
  }
  </style> 
  <table width='90%' align='center'>
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
                webpage served from AsyncESPOTA library
                <a href=/firmware/><button>Update Firmware</button></a>
           </td>
           <td>
                webpage served from AsyncESPOTA library
                <a href=/wifiSettings><button>WiFi Settings</button></a>
           </td>
            <td>
                <!-- a href=/settings><button></button></a -->
           </td>
      </tr>
      <tr height='70'>
           <td>
                webpage served from AsyncESPOTA library
                <a href=/crash><button>Crash Firmware</button></a>
           </td>
            <td>
                webpage served from AsyncESPOTA library
                <a href=/downloadcoredump><button>View total Counters</button></a>
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
void onRootIndexRequest(H4AW_HTTPHandler *handler){
  const char* rootIndex =  root().c_str();
  handler->send(200, "text/html", strlen(rootIndex), rootIndex);
}



/*
// Callback: receiving any WebSocket message
void onWebSocketEvent(uint8_t clientNum, WStype_t type, uint8_t * payload, size_t length) {
  Serial.println("A web socket event has been received");
  
  // Figure out the type of WebSocket event
  switch(type) {

    // Client has disconnected
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", clientNum);
// ****************************************************************************************************************************
      myUpdater.setCurrentClient(clientNum, DISCONNECTED);
// ****************************************************************************************************************************      
      break;

    // New client has connected
    case WStype_CONNECTED:
      Serial.println("[skewtch::onWebSocketEvent]connection request received");
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
      Serial.print("comparing string to test statement = ");
      Serial.println(strcmp((char *)payload, "?FirmwareProgress"));
// ****************************************************************************************************************************
      // Start sending progress bar info
      if(strcmp((char *)payload, "?FirmwareProgress") == 0 ) {
          webSocket.sendTXT(clientNum, "FirmwareProgress");      //just for debugging web client.. doesn't *do* anything
          
          // let myUpdater know which client requested a firmware update
          myUpdater.setCurrentClient(clientNum, PROGRESSSOCKET);
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
        Serial.printf("[%u] Message not recognized %S\n" ,clientNum, payload);
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
*/


void configureWebServer(){
    Serial.println("Starting web server");
/*
 * H4 has a built in notFound handler - no need to add another
 * H4 has a built in static file server that refers to SPIFFS - no need to add a nother
 */
     
    webServer.on("/", HTTP_GET, onRootIndexRequest);
/* TODO:  write giveMeAfunctionName() function or go lambda if you wish
 *  webServer.on("/settings", HTTP_GET, onSettingsIndexRequest);    //GET  !!    
    webServer.on("/settings", HTTP_POST, onSettingsPost);           //POST !!
    webServer.on("/sync", HTTP_GET, giveMeAfunctionName);           
    webServer.on("/diagnostics", HTTP_GET, giveMeAfunctionName); 
    webServer.on("/totals", HTTP_GET, giveMeAfunctionName); 
    webServer.on("/logs", HTTP_GET, giveMeAfunctionName);           
    webServer.on("/coolthingpage", HTTP_GET, giveMeAfunctionName); 
*/
}
