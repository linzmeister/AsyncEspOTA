/*
  deleted from #remote style to allow it to grow without inner scroll bar

  max-height: 300px;
  overflow: scroll;
*/
    
const char styleCSS[] PROGMEM = R"=====(
<!DOCTYPE html>
<meta name="viewport" content="width=device-width,initial-scale=1.0">
<!-- link rel="stylesheet" href="https://www.w3schools.com/w3css/4/w3.css" -->
<html>
<!-- HTML_STYLE -->
<style>
    #remote {        
        background-color: whitesmoke;
    }
    #file-update-label {
        display: flex;
        justify-content: center;
        align-items: center;
        background-color: white;
        border-radius: 5px;
        padding: 5px;
        height: 30px;
        width: 70%;
    }
    #blocker {
        display: flex;
        flex-direction: column;
        justify-content: center;
        align-items: center;
        top: 0;
        left: 0;
        z-index: 1;
        position: fixed;
        height: 100%;
        width: 100%;
        background: steelblue;
        opacity: 0.95;
        color: white;
    }
/*    
 *     maybe use a common progress bar depending on what comes of Spiffs progress
 *     
 *     #blocksProgressDiv {
          width: 80%;
          margin: auto;
          background-color: #ddd;
    }
    #blockBar {
        width: 0%;
        height: 30px;
        background-color: #04AA6D;
        text-align: center;
        line-height: 30px;
        color: white;
    }
*/
    #spiffsProgressDiv {
      width: 80%;
      margin: auto;
      background-color: #ddd;
    }
    #binProgressDiv {
      width: 80%;
      margin: auto;
      background-color: #ddd;
    }
    #spiffsProgBar {
      width: 0%;
      height: 30px;
      background-color: #04AA6D;
      text-align: center;
      line-height: 30px;
      color: white;
    }
    #binProgBar {
      width: 0%;
      height: 30px;
      background-color: #04AA6D;
      text-align: center;
      line-height: 30px;
      color: white;
    }
    
    body {
      text-align: center;
      font-family: helvetica;
      background-color: steelblue;
      margin: 0;
    }
    form {
        display: flex;
    }
    .spacer {
        display: block;
        width: 100%;
        height: 80px;
    }
    .small {
		height: 5px;
	}
    .header {
        display: flex;
        position: fixed;
        top: 0;
        width: 100%;
        background-color: white;
    }
    .container {
        background-color: whitesmoke;
        display: flex;
        flex-direction: column;
        justify-content: center;
        border: solid 1px lightgray;
        border-radius: 5px;
        width: 500px;
        margin: auto;
        margin-bottom: 10px;
    }
    .column {
        display: flex;
        flex-direction: column;
    }
    .general-info {
        max-height: 100px;
        width: 67%;
        display: flex;
        flex-direction: column;
        padding: 10px;
    }
    .general-info > .repo-name {
        text-align: left;
        font-size: larger;
        margin-bottom: 5px;
    }
    .general-info > .repo-description {
        font-size: smaller;
        text-align: left;
    }
    .more-info {
        background-color: lightgray !important;
        width: 100%;
    }
    .more-info .repo-info {
        width: 80%;
    }
	.wifi-info {
        max-height: 100px;
        width: 60%;
        display: flex;
        padding: 10px;
    }
	.wifi-name {
        text-align: left;
        font-size: larger;
		flex-direction: column
        margin-bottom: 5px;
    }
    .setting-container {
        display: flex;
        flex-direction: column;
        border-bottom: 1px solid lightgray;
    }
    .setting-container form {
        flex-wrap: wrap;
        justify-content: flex-end;
        padding: 10px;
    }
    .setting-container label {
        display: flex;
        align-items: center;
        justify-content: flex-start;
        width: calc(35% - 10px);
        margin: 5px 0 5px;
        text-align: left;
    }
    .row {
        display: flex;
        flex-direction: row;
        align-items: center;
        justify-content: space-between;
        flex-wrap: wrap;
        background-color: white;
        margin: 5px;
        border-radius: 5px;
    }
    .set-inputs .row {
        background-color: initial;
    }
    .hidden {
        display: none !important;
    }
    .header-container {
        display: flex;
        flex-direction: row;
        justify-content: space-between;
        flex-wrap: wrap;
        border-bottom: solid 1px lightgray;
    }
    .header-container > h2 {
        padding-left: 10px;
    }
    .row>.label {
        margin-left: 10px;
    }
    .set {
        display: flex;
        flex-direction: column;
        background-color: whitesmoke;
        padding: 10px;
    }
    .set>.set-inputs {
        display: flex;
        flex-direction: column;
    }
    .set>.set-inputs input {
        width: 50%;
        height: 30px;
        border: none;
        border-radius: 5px;
        padding-left: 10px;
        margin-top: 5px;
        margin-bottom: 5px;
    }
    .set>.set-inputs>.column {
        background: darkgrey;
        border-radius: 5px;
        padding: 10px;
        color: white;
        margin-top: 5px;
    }
    select {
        width: calc(50% + 10px);
        height: 30px;
        border: none;
        margin-top: 5px;
        margin-bottom: 5px;
    }
    .set>.set-buttons {
        display: flex;
        flex-direction: row;
        justify-content: flex-end;
    }
    .btn-container {
        display: flex;
        justify-content: flex-end;
		align-items: center;
    }
    .btn-container input {
        width: 15%;
        border: 1px solid lightgrey;
        border-radius: 5px;
        padding: 15px 32px;
        margin: 10px 5px;
    }
    .btn {
        border-radius: 5px;
        padding: 15px 10px;
        text-align: center;
        transition-duration: 0.4s;
        margin: 10px 5px;
        cursor: pointer;
        text-decoration: none;
		pointer-events: auto;
    }
	.btn:disabled{
		color: whitesmoke
		pointer-events: none;
	}
    .edit {
        background-color: dodgerblue;
        border: solid 1px dodgerblue;
        color: white;
    }
    .edit:hover {
        color: dodgerblue;
    }
    .save {
        background-color: yellowgreen;
        border: solid 1px yellowgreen;
        color: white;
    }
	.save:hover {
        color: yellowgreen;
    }
	.on {
        background-color: green;
        border: solid 1px green;
        color: white;
    }
	.on:hover {
        color: green;
    }
    .off, .delete {
        background-color: tomato;
        border: solid 1px tomato;
        color: white;
    }
    .off:hover, .delete:hover {
        color: tomato;
    }
    .cancel {
        background-color: black;
        border: solid 1px black;
        color: white;
    }
    .ok {
        background-color: green;
    }
    .error {
        background-color: red;
    }
    .cancel:hover {
        color: black;
    }
    .btn:hover {
        background-color: white;
    }
    .spinner {
        height: 30px;
        border-radius: 5px;
        background-color: lightgray;
    }
    .lds-ring {
        width: 50px;
        height: 50px;
        display: inline-block;
        position: relative;
        margin: 20px;
    }
    .lds-ring div {
        box-sizing: border-box;
        display: block;
        position: absolute;
        width: 100%;
        height: 100%;
        border: 8px solid #fff;
        border-radius: 50%;
        animation: lds-ring 1.2s cubic-bezier(0.5, 0, 0.5, 1) infinite;
        border-color: #fff transparent transparent transparent;
    }
    .lds-ring div:nth-child(1) {
        animation-delay: -0.45s;
    }
    .lds-ring div:nth-child(2) {
        animation-delay: -0.3s;
    }
    .lds-ring div:nth-child(3) {
        animation-delay: -0.15s;
    }
	.connected{		// just to get an element by ClassName - does nothing graphically
	}
    @keyframes lds-ring {
        0% {
            transform: rotate(0deg);
        }
        100% {
            transform: rotate(360deg);
        }
    }
	
	.w3-green,.w3-hover-green:hover{
		color:#fff !important;
		background-color:#4CAF50 !important
	}
	.w3-container:after, .w3-container:before, .w3-panel:after,
	.w3-panel:before, .w3-row:after, .w3-row:before,
	.w3-row-padding:after, .w3-row-padding:before,
	.w3-cell-row:before, .w3-cell-row:after,
	.w3-clear:after, .w3-clear:before,
	.w3-bar:before, .w3-bar:after{
		content:"";
		display:table;
		clear:both
	}
	.w3-container, .w3-panel
	{
		padding:0.01em 16px
	}
	.w3-panel{
		margin-top:16px;
		margin-bottom:16px
	}
	
    @media only screen and (max-width: 600px) {
        body {
            margin: 0;
        }
        .container {
            width: 100%;
            border: none;
            border-radius: 0px;
        }
    }	
</style>
)=====";

const char updatePage[] PROGMEM = R"=====(
<!-- HTML_DOM -->
<body>
    <div class="spacer">
	</div>
    <div class="header">
        <a id='restart-button' onclick='restart()' class='btn edit'>Restart</a>
    </div>
    <div class="lds-ring" id='page-loader'>
		<div></div>
		<div></div>
		<div></div>
		<div></div>
	</div>
)=====";


const char updatePageApps[] PROGMEM = R"=====(
	<div class='container' id="gpio-container">
		<div id='remote-header-bar' class="header-container">
			<h2>Install from Internet</h2>
		</div>
		<div id='remote' class='column'>
		</div>
	</div>
)=====";


const char updatePageCustom[] PROGMEM = R"=====(
    <div class="container" id="automation-container">
        <div id='local-header-bar' class="header-container">
            <h2>Install from custom URL</h2>
        </div>
        <div id='local' class='column'>
            <div class='set'>
                <div class='set-inputs'>
                    <div class='row'>
                        <label for='spiff-path'>Spiff file URL:</label>
                        <input type='text' name='spiff' id='spiff-path-input'>
                    </div>
                    <div class='row'>
                        <label for='bin-path'>Bin file URL</label>
                        <input type='text' name='bin' id='bin-path-input'>
                    </div>
                    <a id='submit-update-file' onclick='installCustomPaths()' class='btn save disabled'>Update</a>
                </div>
            </div>
        </div>
    </div>
)=====";


const char updatePageLocal[] PROGMEM = R"=====( 
    <div class="container" id="automation-container">
        <div id='local-header-bar' class="header-container">
            <h2>Install from local directory</h2>
        </div>
        <div id='local' class='column'>
            <div class='set'>
                <div class='set-inputs'>
                    <div class='row'>
                        <label for='firmware-file' id='file-update-label'>Choose file</label>
                        <input type='file' name='update' id='firmware-file' onchange='fillUpdateInput(this)' style=display:none>
                        
						<a id='submit-update-file' onclick='submitUpdate()' class='btn save disabled'>Update</a>
						
						<form method='POST' action='/update' enctype='multipart/form-data'>
							<input type='file' name='update'>
							<input type='submit' value='Update'>  
						</form>"
						
                    </div>
                </div>
            </div>
        </div>
    </div>
)=====";


const char updatePageBlocker[] PROGMEM = R"=====( 
    <div id='blocker' class='hidden'>
      <h2 id='blocker-title'>Loading</h2>
		<div id='Updating'>
			<div id="spiffsProgressDiv" width:80%>
				<div id='spiffsProgressBar' class='w3-container w3-green' style='height:24px width:0%'>
				</div>
			</div>
			<p id="spiffsP">
				SPIFFS: Writing <span id='spiffsBytes'>0</span> bytes of <span id='totalSpiffsBytes'>0</span>.
			</p>
			<div id="binProgressDiv" width:80%>
				<div id='binProgressBar' class='w3-container w3-green' style='height:24px width:0%'>
				</div>
			</div>
			<p id="binP">
				FIRMWARE: Writing <span id='binBytes'>0</span> bytes of <span id='totalBinBytes'>0</span>.
			</p>
			<p id="finished">
			</p>
		</div>
	</div>
</body>
<!-- HTML_SCRIPT -->
<script>
  
  var wsPort=)=====";	//insert ####

     
// concatenate with web socket port of ESP module

const char progressPageB[] PROGMEM = R"=====(;    // end of wsPoprt definition
    var url = "ws://" + window.location.hostname + ":" + wsPort;
    var repos = [];
    const delay = (ms => new Promise(resolve => setTimeout(resolve, ms)));
    
    var spiffsProgressDiv;
    var spiffsBar;        //not created until later
    var spiffsP;
    var spiffsBytes;      //not created until later
    var totalSpiffsBytes; //not created until later
    
    var binProgressDiv;
    var binBar;         //not created until later
    var binP;
    var binBytes;       //not created until later
    var totalBinBytes;  //not created until later

    var finished;

    var button;
        
// This is called when the page finishes loading
    function showProgress() {
		spiffsProgressDiv = document.getElementById("spiffsProgressDiv");
		spiffsBar = document.getElementById("spiffsProgressBar");

		spiffsP        = document.getElementById("spiffsP");
		spiffsBytes = document.getElementById("spiffsBytes");
		totalSpiffsBytes = document.getElementById("totalSpiffsBytes");

		binProgressDiv = document.getElementById("binProgressDiv");
		binBar = document.getElementById("binProgressBar");

		binP           = document.getElementById("binP");
		binBytes = document.getElementById("binBytes");
		totalBinBytes = document.getElementById("totalBinBytes");

		finished       = document.getElementById("finished");

		//button       = document.getElementById("button");

		//button.innerHTML ='<button id="uploadProgress" class="w3-button w3-light-grey" onclick="onPress()">Back</button>';
		///button.disabled = true;

		// Connect to WebSocket server
		wsConnect(url);
    }
      
// Call this to connect to the WebSocket server
    function wsConnect(url) {
      console.log("attempting to connect");
      // Connect to WebSocket server
      websocket = new WebSocket(url);

      // Assign callbacks
      websocket.onopen = function(evt) { onOpen(evt) };
      websocket.onclose = function(evt) { onClose(evt) };
      websocket.onmessage = function(evt) { onMessage(evt) };
      websocket.onerror = function(evt) { onError(evt) };
    }
        
// Called when a WebSocket connection is established with the server
    function onOpen(evt) {

      // Log connection state
      console.log("Connected");

      // Enable button
      //button.disabled = false;

      // Request Firmware uipdates from the esp32
      doSend("?FirmwareProgress");
    }
        
// Called when the WebSocket connection is closed
    function onClose(evt) {

      // Log disconnection state
      console.log("Disconnected");

      // Disable button
      // button.disabled = true;

      // Try to reconnect after a few seconds
      setTimeout(function() { wsConnect(url) }, 2000);
    }

// Called when a message is received from the server
    function onMessage(evt) {

		// Print out our received message
		//console.log("Received: " + evt.data);
		//console.log("evt.data.substr(0,7) = " + evt.data.substr(0,7));

		if (evt.data.substr(0,15) == "FirmwareProgress"){ 
			const elements = getElementsbyClass('disabled');
			elements.forEach((element,index) => {			// iterate through the list
				element[index].classList.remove('disabled');
			});
		}
		
		if (evt.data.substr(0,10) == "Restarting"){ 
		finished.innerHTML = "Rebooting controller, please wait";
		setTimeout(loadHome, 10000)		// delay 10 seconds then go back to home
		}

		if (evt.data.substr(0,17) == "Upload incomplete"){
		spiffsProgressBar.innerHTML = "";
		binProgressDiv.innerHTML = "";
		finished.innerHTML = evt.data;
		}
		if (evt.data.substr(0,20) == "No Updates available"){
		spiffsProgressDiv.innerHTML = "";
		binProgressDiv.innerHTML = "";
		finished.innerHTML = evt.data;
		}
		if (evt.data.substr(0,14) == "reboot complete"){
			loadHome();
		}
      
       switch(evt.data.substr(0,1)){      // update progress bar %'s     example message s999424,1012528#84.20%
        case "s":   // received SPIFFS file progress                  
          recvdData = evt.data.substr(evt.data.indexOf("#")+1, evt.data.length - evt.data.indexOf("#"));
          //console.log("value of SPIFFS %= " + recvdData);
          spiffsBar.style.width=recvdData;
          spiffsBar.innerHTML=recvdData;
          
          spiffsBytes.innerHTML      = evt.data.substr(1,evt.data.indexOf(",")-1);
          totalSpiffsBytes.innerHTML = evt.data.substr(evt.data.indexOf(",")+1, evt.data.indexOf("#") - evt.data.indexOf(",")-1);
          break;
          
        case "b":   //received BIN file progress
          recvdData = evt.data.substr(evt.data.indexOf("#")+1, evt.data.length - evt.data.indexOf("#"));
          //console.log("value of bin% = " + recvdData);
          binBar.style.width=recvdData;
          binBar.innerHTML=recvdData;
          
          binBytes.innerHTML      = evt.data.substr(1,evt.data.indexOf(",")-1);
          totalBinBytes.innerHTML = evt.data.substr(evt.data.indexOf(",")+1, evt.data.indexOf("#") - evt.data.indexOf(",")-1);
          break;
        
        default:
          break;
      }
    }

// Called when a WebSocket error occurs
    function onError(evt) {
      console.log("ERROR: " + evt.data);
    }

// Sends a message to the server (and prints it to the console)
    function doSend(message) {
      console.log("Sending: " + message);
      websocket.send(message);
    }

// Called whenever the HTML button is pressed
    function onPress() {
      doSend("?FirmwareProgress");
      uploadButton.disabled = true;    // Disable button
    }

    const loadHome = async () => {
      const res = await fetch(window.location.hostname);
    }
	
	
    // Restart ESP
    const restart = async () => {
      try {
        const res = await fetch(window.location.href + 'restart');
        blocker.classList.remove('hidden');
        await delay(2000);
        location.reload();
      } catch (err) {
        blocker.classList.add('hidden');
        console.error(`Error: ${err}`);
      }
    }

    const createRepoRow = (repo,index) => {
      let child = document.createElement('div');
      child.innerHTML = `<div class='row' id='rowRepo-${index}'>
        <div class='general-info'>
          <div class='repo-name'>
            ${repo.name}
          </div>
          <div class='repo-description'>
            ${repo.description}
          </div>
        </div>
        <div class='btn-container'>
          <a onclick='fetchRepoInfo(this)' id='getRepoInfo-${index}' class='btn edit'>Check versions</a>
        </div>
      </div>`;
      return child.firstChild;
    }

    const createRepoMoreInfo = (moreInfo,index) => {
      const options = moreInfo.reduce((prev,info) => {
        return prev + `<option value='${info.version}'>${info.version}${info.current?' (last)':''}</option>`
      },'');
      let child = document.createElement('div');
      child.innerHTML = `<div class='row more-info' id='rowRepoInfo-${index}'>
        <div class='repo-info'>
          <select id='version-${index}' name='versions'>${options}</select>
        </div>
        <div class='btn-container'>
          <a onclick='install(this)' id='getRepoInfo-${index}' class='btn save'>Install</a>
        </div>
      </div>`;
      return child.firstChild;
    }

    // Fetch all project repo available
    const fetchRepoList = async () => {
      
      try {
        await fetch(window.location.href + 'repo/list/request');		// gets OK response
        // Avoid connection_reset from backend
        await delay(2000);
        let listRetrieved = false;
        while(!listRetrieved) {
          let resp = await fetch(window.location.href + 'repo/list');	// gets json text response
          let contentType = resp.headers.get("content-type");
          if(contentType && contentType.indexOf("text/json") !== -1) {
            repos = await resp.json();
            const container = document.getElementById('remote');
            repos.forEach((repo,index) => {
              container.appendChild(createRepoRow(repo,index));
            });
            listRetrieved = true;
          } else {
            await delay(1000);
          }
        }
      } catch (err) {
        console.error(`Error: ${err}`);
      }
    }

    const fetchRepoInfo = async (element) => {
      
      document.getElementById('page-loader').classList.remove('hidden');
      const repoIndex = element.id.split('-')[1];
      const repo = repos[repoIndex];
      var data = new FormData();
      data.append( "repo", repo.repoRawPath );
      try {
        await fetch(window.location.href + 'repo/info/request', {
          method: 'POST',
          body:data
        });
        // Avoid connection_reset from backend
        await delay(2000);
        let informationRetrieved = false;
        while(!informationRetrieved) {
          console.log("Calling for Repo Details");
          let resp = await fetch(window.location.href + 'repo/info');
          let contentType = resp.headers.get("content-type");
          if(contentType && contentType.indexOf("text/json") !== -1) {
            const repoMoreInfo = await resp.json();
            const repoRow = document.getElementById(`rowRepo-${repoIndex}`);
            if (repoRow.childElementCount>2) {
              repoRow.replaceChild(repoRow.lastElementChild,createRepoMoreInfo(repoMoreInfo,repoIndex));
            } else {
              repoRow.appendChild(createRepoMoreInfo(repoMoreInfo,repoIndex));
            }
            informationRetrieved = true;
          } else {
            await delay(1000);
          }
        }
        document.getElementById('page-loader').classList.add('hidden');
      } catch (err) {
        console.error(`Error: ${err}`);
      }
      //console.log("Standing by to get me some progress data");
      showProgress();
    }

    const install = async (element) => {
      console.log("Install from Repo");
      const repoIndex = element.id.split('-')[1];
      const repo = repos[repoIndex];
      const selectedVersion = document.getElementById(`version-${repoIndex}`).value;
      
      
      document.getElementById('blocker').classList.remove('hidden');
      document.getElementById('blocker-title').innerText = `Downloading ${repo.name} version ${selectedVersion}.\n`;
      
      var data = new FormData();
      data.append( "binPath", repo.repoRawPath + `${selectedVersion}/`+ repo.BinFilename);

	  // prevent crashing when no SPIFFS supplied
      if (repo.SPIFFSFilename!=""){
        data.append( "spiffsPath", repo.repoRawPath + `${selectedVersion}/` + repo.SPIFFSFilename);
      }
      try {
		console.log("About to send request to ESP to perform update");
        const res = await fetch(window.location.href + 'repo/install/request', {
          method: 'POST',
          body:data
        });
		console.log("Request sent!!");
      } catch (err) {
        console.error(`Error: ${err}`);
        document.getElementById('blocker').classList.add('hidden');
      } 
    }

    const installCustomPaths = async (element) => {
      const binPath = document.getElementById('bin-path-input').value;
      const spiffPath = document.getElementById('spiff-path-input').value;

      document.getElementById('blocker').classList.remove('hidden');
      document.getElementById('blocker-title').innerText = `Downloading repo ${binPath}.\n Reload the page manually in few minutes.`;
      
      var data = new FormData();
      if(!binPath.isEmpty()){
        data.append( "binPath", binPath);  
      }
      if(binPath!=""){
        data.append( "spiffsPath", spiffPath);  
      }
      try {
        const res = await fetch(window.location.href + 'repo/install/request', {
          method: 'POST',
          body:data
        });
      } catch (err) {
        console.error(`Error: ${err}`);
        document.getElementById('blocker').classList.add('hidden');
      }
    }

    //TODO  write submitUpdate() function to handle upload from client local filesystem.

    // Events
    window.onload = async () => {
      await fetchRepoList();
      document.getElementById('page-loader').classList.add('hidden');
      //showProgress(); //start webSockects connection
    };
  </script>
</html>
<!-- HTML_SCRIPT_END -->
)=====";


/*
<div class="row" id="rowRepo-1">
	<div class="general-info">
	  <div class="repo-name">
		BasicAsyncEspOTA
	  </div>
	  <div class="repo-description">
		a simplified example of OTA
	  </div>
	</div>
	<div class="btn-container">
	  <a onclick="fetchRepoInfo(this)" id="getRepoInfo-1" class="btn edit">Check versions</a>
	</div>
  <div class="row more-info" id="rowRepoInfo-1">
	<div class="repo-info">
	  <select id="version-1" name="versions">
		<option value="2.5.19">2.5.19 (last)</option>
		<option value="1.2">1.2</option>
		<option value="1.0.0">1.0.0</option>
	  </select>
	</div>
	<div class="btn-container">
	  <a onclick="install(this)" id="getRepoInfo-1" class="btn save">Install</a>
	</div>
  </div>
</div>
*/