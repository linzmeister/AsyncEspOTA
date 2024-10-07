
var wsPort=@param1

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