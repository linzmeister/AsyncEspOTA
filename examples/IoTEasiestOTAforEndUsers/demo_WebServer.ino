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


//**************************************************************************************************
// Settings/Preferences web page
/*
String settings(){
  String result = "<form id='BFCSettings' method='POST' action='/settings' name='settingsForm'>"
      "<table width='90%' bgcolor='#33ccff' align='center'>"
        "<tr>"
            "<td colspan=2>"
                "<center><font size=4><b>Settings</b></font></center>"
                "<p>"
            "</td>"
        "</tr>"
        "<tr>"
             "<td style='text-align:right'>Home SSID :</td>"
             "<td><input id='hssidfield' type='text' name='homessid' value='" +
              hssid +
        "'<br>"
              "</td>"
        "</tr>"
        "<p>"
        "<tr>"
             "<td style='text-align:right'>Home Password :</td>"
             "<td><input type='text' id='homepassword' name='homepassword' value='" +
              hpass +
      "'><br></td>"
        "</tr>"
        "<p>"
        "<tr>"
             "<td style='text-align:right'>Away SSID :</td>"
             "<td><input type='text' name='awayssid' value='" +
              assid +
              "'><br></td>"
        "</tr>"
        //"<p>"
        "<tr>"
             "<td style='text-align:right'>Away Password :</td>"
             "<td><input type='text' name='awaypassword' value='" +
              apass +
              "'><br></td>"
        "</tr>"
        "<tr>"
             "<td style='text-align:right'>Hot Spot  SSID :</td>"
             "<td><input type='text' id='hotspotssid' name='hotspotssid' value='" +
              HSssid +
            "'><br></td>"
        "</tr>"
        "<tr>"
             "<td style='text-align:right'>Hot Spot Password :</td>"
             "<td><input type='text' id='hotspotpassword' name='hotspotpassword' value='" +
              HSpass +
             "'><br></td>"
        "</tr>"
        "<tr>"
          "<td>"
            "<p><p><p>"
              "<center><font size=4>"
                "<button onclick='updateValues(this.form)'>Save Settings</button>"
              "</font></center>"
            "</td>"
            "<td>"
            "</td>"
        "</tr>"
    "</table>"
  "</form>"
"<script>"
 "function updateValues(form)"
 "{"
    "var xhttp = new XMLHttpRequest();"
    "xhttp.onreadystatechange = function() {"
      "if (this.readyState == 4 && this.status == 200) {"
        "var elements = document.getElementById('BFCSettings').elements;"     // validate GMT Offset string is a +/- time
      "}"
    "};"
    "for (var i = 0, element; element = elements[i++];) {"
   "if (element.type === 'text')"
      "{"
        "document.getElementById(element.id).value = this.responseText;"
        "xhttp.open('POST', '/BFCValueSave', true);"
        "xhttp.send(document.getElementById(element.id).value);"
      "}"
    "}"
 "}"
    "</script>";
    return result;
}
*/

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

/*
// Show the settings page and read in any saved settings
void onSettingsIndexRequest(AsyncWebServerRequest *request){  
  getAllPrefs();
  const char* settingsIndex =  settings().c_str();
  request->send(200, "text/html", settingsIndex);
}


//*******************************************************************************
// read the body of the POST and save the data to NVS preference memory
/*
void onSettingsPost(AsyncWebServerRequest *request){
    Serial.print("request->args() = ");
    Serial.println(request->args());
    
    if (request->args() != 0){
      for (int i = 0; i < request->args(); i++) {   
        if (request->argName(i).startsWith("home")){
          currentPref.NameSpace = HOMENETWORK;
        }
        if (request->argName(i).startsWith("away")){
          currentPref.NameSpace = AWAYNETWORK;
        }
        if (request->argName(i).startsWith("hotspot")){
          currentPref.NameSpace = HOTSPOTNETWORK;
        }

        currentPref.Key = request->argName(i);
        currentPref.Value = request->arg(i);

        SavePref (&currentPref);
      }
    }
    
    request->send(200, "text/html", saved);   // show the "Saved" notification message
  }
*/

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


void onPageNotFound(AsyncWebServerRequest *request) {
    IPAddress remote_ip = request->client()->remoteIP();
    Serial.println("[Error 404 " + remote_ip.toString() +
                    "] HTTP GET request of " + request->url());
    request->send(404, "text/plain", 
      "We honestly searched everywhere but \"We still... haven't found... what your looking for\". :D ;)");
      request->send(404, "text/plain", 
      "[" + remote_ip.toString() +
                    "] HTTP GET request of " + request->url());
}



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
    webServer.onNotFound([](AsyncWebServerRequest *request){onPageNotFound(request);});  

}
