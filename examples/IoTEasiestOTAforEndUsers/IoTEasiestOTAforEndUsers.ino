/*
https://github.com/espressif/arduino-esp32/blob/master/docs/source/tutorials/preferences.rst

Deleting key-value Pairs
preferences.clear();
Deletes all the key-value pairs in the currently opened namespace.
The namespace still exists.
The namespace must be open in read-write mode for this to work.
preferences.remove("keyname");
Deletes the "keyname" and value associated with it from the currently opened namespace.
The namespace must be open in read-write mode for this to work.
Tip: use this to remove the "test key" to force a "factory reset" during the next reboot (see the Real World Example above).
If either of the above are used, the key-value pair will need to be recreated before using it again.
*/

/*
    espBasicOTA

    This sketch demonstrates how to use the AsyncEspOTA Library.  The main aims 
    of the library are to make 3 things simple for end users of IoT retail products:
      1) initial "Out of Box" connection to product,
      2) configuring wifi settings of the product and 
      3) installing firmware updates to the product as new features and bug fixes are released.
    
    It is also a starting point for IoT based projects which will include webpages served by 
    the esp32 and the AsyncEspOTA library adds Over the Aiur firmware updates from a gitHub 
    repository, custom URL and web client local file systems.  
    
    It uses an AsyncWebServer and WebSocketsServer to display webpages and show firmware update progresss 
    including error messages on the /firmware web pages if the update fails.

    The library uses some JSON files to select which firmwware poroject and which version you wish to load 
    into the esp32.

    The sketch is broken up into several files to keep various elements of the code spearated.  
    demo.h includes all the library references and Global variable declarations.
    demo_WebServer.ino includes the user webpages, event handlers and callback functions.
    demo_WiFi_Prefs.ino includes all the wifi initialisation functions, ssid and Password 
        settings which are saved from the web server and stored via the preferences library functions.

    If the library and supporting commands are added into multiple different projects, it is 
    possible to add them into the inderlying JSON apps list so that they may be uploded Over The Air    
    
    The circuit:
    * esp32
    * no additional compulsory harware 
    
    Created 22 Nov 2022
    By Romehein
    https://github.com/RomeHein/ESPInstaller

    Modified 25 February 2023  (converted to Arduino Library with progress bar feedback)
    By Lindsay McPhee

    https://github.com/linzmeister/AsyncEspOTA

*/

#include "demo.h"

//uncomment the following line to enable debug mode
#define __debug

// TODO: create method to change default APpassword and all it to be 
// reset back to factory default if user-created password is forgotten
// Hold a button during bootup??
// create/ Erase preference for APpassword


//forware declarations
void eraseAllPrefs();

void setup(void){
    Serial.begin(115200);
    while(!Serial);
    
    //eraseAllPrefs();  

    myUpdater.begin();
    configureWebServer();   //Start the web server so they can set the SSIDs and Passwords via AP
    webServer.begin();
    
    webSocket.begin();
    webSocket.onEvent(onWebSocketEvent); 
    Serial.println("Web server is now completely alive - even if only on the AP interface");
}

void loop(){
  myUpdater.loop(); //checks to see if the the user has changed Firmware Update flags and responds accordingly
  webSocket.loop();
  delay(300);
}


void eraseAllPrefs(){
#ifdef __debug
  Serial.println("About to erase all prefs and restart uMC");
#endif  
  nvs_flash_erase();  //erase the NVS partition and  
  nvs_flash_init();   //initialise the NVE partition
  ESP.restart();
}
