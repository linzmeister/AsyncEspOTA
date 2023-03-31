
// concatenate with <style> from index.h

const char wifiPageA[] PROGMEM = R"=====(
<!-- HTML_DOM -->
<body>
    <div class='spacer'>
	</div>
	<div class='lds-ring' id='page-loader'>
		<div></div>
		<div></div>
		<div></div>
		<div></div>
	</div>
  
	<div class='container' id='scanned-container'>
		<div id='scanned-header-bar' class='header-container'>
			<h2>Local WiFi Networks </h2>
		</div>
		<div id='scan' class='column'>
		</div>
	</div>
	
	<div class='container' id='saved-container'>
		<div id='saved-header-bar' class='header-container'>
			<h2>Saved Networks</h2>
			<div class='btn-container'>
			<a onclick='addNew(this)'  class='btn on'>+ Add network</a>
			</div>
		</div>
		<div id='saved' class='column'>
		</div>
	</div>
	
    <div id='blocker' class='hidden'>
      <h2 id='blocker-title'>Loading</h2>
		<div id='Saved'>				
			</p>
			<p id='finished'>
			</p>
		</div>
	</div>
</body>
<!-- HTML_SCRIPT -->
<script>
  var wsPort=)=====";

// concatenate with wsPort Arduino variable		

		
const char wifiPageB[] PROGMEM = R"=====(;
	var url = "ws://" + window.location.hostname + ":" + wsPort;
	var scannedWlans = [];
	var savedWlans = [];
	
	const delay = (ms => new Promise(resolve => setTimeout(resolve, ms)));	

	// scan for local wifi networks every 15 Seconds
	const scanInterval = setInterval(doWifiScan, 15000);
	
	// enables and disables scanning (to allow the user to connect/edit without refresh being annoying)
	var scanning = true;

	// Pre load images for signal strength and security
	bar1 = new Image;
	bar2 = new Image;
	bar3 = new Image;
	bar4 = new Image;
	bar5 = new Image;
	padlockOpen = new Image;
	padlockClosed = new Image;

	bar1.src = "/images/bar1.png";		// weakest signal
	bar2.src = "/images/bar2.png";
	bar3.src = "/images/bar3.png";
	bar4.src = "/images/bar4.png";
	bar5.src = "/images/bar5.png";		// strongest signal
	padlockOpen.src   = "/images/open.png";
	padlockClosed.src = "/images/closed.png";


	const createScannedRow = (scanWlan, index, connected = false) => {
		let child = document.createElement('div');
		var strengthImageTag="";

		switch(true){			// javascript can't use < or > in switch case clauses.
			// choose 1 of 4 strength indicator images based on value signal
			case (scanWlan.rssi >= -60):
				strengthImageTag = `<img src='${bar5.src}' alt='|||||' height='35px' border='0'>`;
				break;
			case (scanWlan.rssi >= -70):
				strengthImageTag = `<img src='${bar4.src}' alt='||||'  height='35px' border='0'>`;
				break;
			case (scanWlan.rssi >= -80):
				strengthImageTag = `<img src='${bar3.src}' alt='|||'  height='35px' border='0'>`;
				break;
			case (scanWlan.rssi >= -90):
				strengthImageTag = `<img src='${bar2.src}' alt='||'  height='35px' border='0'>`;
				break;
			case (scanWlan.rssi <-90):
				strengthImageTag = `<img src='${bar1.src}' alt='|'  height='35px' border='0'>`;
				break;
			default:	// no image
				break;
		}
		
		var encryptionImageTag = "";
		switch(scanWlan.enc){			// choose 1 of 2 encryption images
			case "C":
				encryptionImageTag=`<img src='${padlockClosed.src}' alt='Secured' height='35px' border='0'>`;
				break;
			case "O":	
				encryptionImageTag= `<img src='${padlockOpen.src}' alt='Open' height='35px' border='0'>`;
				break;
			default:	// no image
				break;
		}

				// construct buttons html string so it can be concatenated later.. 
									// Chrome added </div> whre not needed
		var someButtons; 
		if(connected=="true"){
			someButtons=`
				<div class='btn-container'>
					${strengthImageTag}
					Connected 
				</div>`;
		} else {
			someButtons =`
			<div class='btn-container' id='scanWlan-${index}-connect'>
				${strengthImageTag}
				${encryptionImageTag}
				<a id='scanWlan-${index}-show' class='btn on' onclick="(function(){
					document.getElementById('scanWlan-${index}-edit').classList.remove('hidden');
					document.getElementById('scanWlan-${index}-connect').classList.add('hidden');
					stopScan();
				 })();">
					Connect
				</a>
			</div>`;
		}
		
		child.innerHTML = `
				<div class='row' id='rowScanWlan-${index}'>	<!-- row -->
					<div class='wifi-info'>				<!-- column 60%-->
						<div class='wifi-name' id='scanWlan-${index}-ssid'>
							${scanWlan.ssid} 						
						</div>
					</div>
					<div class='column'>
							${someButtons}
					</div>
				</div>
				<div class='spacer small'>	
					<!--  forces the row above to FILL a row, rather than wrapping multiple networks onto a single row -->
				</div>
				<div class='more-info hidden' id='scanWlan-${index}-edit'>
					<div class='wifi-info'>Enter Password:
						<input type='text' name='password'id='scanWlan-${index}-password'>
					</div>
					<div class='column'>
						<div class='btn-container' id='rowSsidScanWlan-${index}'>
							<a id='scanWlan-${index}-save' class='btn save' onclick='saveWlan(this)'>
								Save
							</a>
							<a id='scanWlan-${index}-cancel' class='btn cancel' onclick="(function(){
							  document.getElementById('scanWlan-${index}-password').value='';
							  document.getElementById('scanWlan-${index}-edit').classList.add('hidden');
							  document.getElementById('scanWlan-${index}-connect').classList.remove('hidden');
								  startScan();
							  })();">
							  Cancel
							</a>
						</div>
					</div>
				</div>`;
				document.getElementById('page-loader').classList.add('hidden');
		return child;
	}
	

	// Fetch all scannedWlans available
	const fetchScannedList = async () => {
		try {
			let listRetrieved = false;
			while(!listRetrieved) {
				let resp = await fetch(window.location.href + '/scan');
				let contentType = resp.headers.get("content-type");
					if(contentType && contentType.indexOf("text/json") !== -1) {
						scannedWlans = await resp.json();							// received json data!!
						if (scanning==true){
							const container = document.getElementById('scan');
							container.innerHTML="";
							// iterate through the list
							scannedWlans.forEach((wlan,index) => {
								// add a new row and call function above
								container.appendChild(createScannedRow(wlan, index, wlan.cnctd));
							});
						listRetrieved = true;
					}
				} else {
				await delay(1000);
				}
			}
		} catch (err) {
			console.error(`Error: ${err}`);
		}
	}


	const createNewSavedRow = (index) => {
		let child = document.createElement('div');
		child.innerHTML = `
		<div class='set' id='new'>
			<div class='set-inputs' id='savedWlan-${index}'>
				<div class='row'>
					<label for='ssid'>SSID:</label>
					<input type='text' name='ssid' id='savedWlan-${index}-ssid' value=''>
				</div>
				<div class='row'>
					<label for='password'>Password:</label>
					<input type='text' name='password' id='savedWlan-${index}-pass' value=''>
				</div>
				<div class='btn-container'>
					<a class='btn edit hidden' 	 id='savedWlan-${index}-edit'   onclick='editWlan(this)'>Edit</a>
					<a class='btn delete hidden' id='savedWlan-${index}-delete' onclick='deleteWlan(this)'>Delete</a>
					<a class='btn save' 		 id='savedWlan-${index}-save'   onclick='saveWlan(this)'>Save</a>
					<a class='btn cancel'		 id='savedWlan-${index}-cancel' onclick="(function(){
					  document.getElementById('new').innerHTML='';
						  startScan();
					  })();">
					  Cancel
					</a>
				</div>					
			</div>
		</div>`;
		return child.firstChild;
	}


	const createSavedRow = (SavedWlan,index) => {
		let child = document.createElement('div');
		child.innerHTML = `
		<div class='set'>
			<div class='set-inputs' id='savedWlan-${index}'>
				<div class='row'>
					<label for='ssid'>SSID:</label>
					<input type='text' name='ssid' id='savedWlan-${index}-ssid' value='${SavedWlan.ssid}'>
				</div>
				<div class='row hidden'>
					<label for='password'>Password:</label>
					<input type='text' name='password' id='savedWlan-${index}-pass' value='${SavedWlan.pass}'>
				</div>
				<div class='btn-container'>
					<a onclick='editWlan(this)'   	 id='savedWlan-${index}-edit'   class='btn edit'>Edit</a>
					<a onclick='deleteWlan(this)' 	 id='savedWlan-${index}-delete' class='btn delete hidden'>Delete</a>
					<a onclick='saveWlan(this)'   	 id='savedWlan-${index}-save'   class='btn save hidden'>Save</a>
					<a onclick='cancelEditing(this)' id='savedWlan-${index}-cancel' class='btn cancel hidden'>Cancel</a>
				</div>					
			</div>
		</div>`;
		return child.firstChild;
	}


	const fetchSavedSettings = async () => {
		document.getElementById('page-loader').classList.remove('hidden');
		try {
			let listRetrieved = false;
			while(!listRetrieved) {
				let resp = await fetch(window.location.href + '/saved');
				let contentType = resp.headers.get("content-type");					
				if(contentType && contentType.indexOf("text/json") !== -1) {	
					savedWlans = await resp.json();								// received json data!!
					const container = document.getElementById('saved');			// first block of page
					savedWlans.forEach((wlan,index) => {						// iterate through the list
					    container.appendChild(createSavedRow(wlan,index));		// add a new row and call function above
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


//  editing
	const editWlan = async (element) => {
		console.log("Edit  saved network");

		const nameParts  = element.id.split('-');
		const wlanPrefix = nameParts[0];
		const wlanIndex  = nameParts[1];

		document.getElementById(`'${wlanPrefix}-${wlanIndex}-connect'`).classList.add('hidden');
		document.getElementById(`'${wlanPrefix}-${wlanIndex}-edit'`).classList.remove('hidden');
		stopScan();
	}
	
	
//  deleting
	const deleteWlan = async (element) => {
		console.log("Delete Saved Network");

		const nameParts  = element.id.split('-');
		const wlanPrefix = nameParts[0];
		const wlanIndex  = nameParts[1];

		document.getElementById(`'${wlanPrefix}-${wlanIndex}-edit'`).classList.add('hidden');
		document.getElementById(`'${wlanPrefix}-${wlanIndex}-connect'`).classList.remove('hidden');
		startScan();
	}


	

// add a new Saved Network
	const addNew = async (element) => {
		console.log("Adding a new network");
		
		 if(savedWlans.length==0){
			console.log("creating first saved wlan");
			createNewSavedRow (0)
		 } else {
			console.log("creating another saved wlan");
			createNewSavedRow (savedWlans.length)
		 }
		stopScan();
	}

	/** save network settings:
		New Scanned
		New Saved
		edited Existing Saved
		
		https://www.w3schools.com/jsref/jsref_obj_array.asp
		
		
	*/
	const saveWlan = async (element) => {
		document.getElementById('blocker-title').innerHTML='Saving';
		document.getElementById('blocker').classList.remove('hidden');
		
		console.log("Saving new scanned wifiSetting");

		const nameParts  = element.id.split('-');
		const wlanPrefix = nameParts[0];
		const wlanIndex  = nameParts[1];

		document.getElementById(`${wlanPrefix}-${wlanIndex}-edit`).classList.add('hidden');
		document.getElementById(`${wlanPrefix}-${wlanIndex}-connect`).classList.remove('hidden');
		
		if(wlanPrefix=="scanWlan"){
			var wlan = scannedWlans[wlanIndex];
			console.log("saving a scanned wifi network");
		}else{
			var wlan = savedWlans[wlanIndex];
			console.log("saving a saved wifi network");
		}
		
		if (typeof wlan != "undefined"){
			var ssid = wlan.ssid;
			var password = document.getElementById(`${wlanPrefix}-${wlanIndex}-password`).value;
		} else {
			console.log("wlan is undefined");
		}
		
		savedWlans.push
		var data = new FormData();
		data.append("ns", wlan.);
		data.append("ssid", ssid);
		data.append("password", password);
		
		// show saving --> saved on blocker
		
		try {
			const res = await fetch(window.location.href + '/saveScannedWlan', {	//Scanned
				method: 'POST',
				body:data
			});
		} catch (err) {
			console.error(`Error: ${err}`);
		}
		document.getElementById('blocker').classList.add('hidden');
		startScan();
		fetchSavedSettings();
	}



// Window Interval functions:
//Interval never stops, we just stop responding to it!!
	
	function startScan(){		// resume scanning for WiFi Networks every 15 seconds...  
		if(scanning==false){	// but only once every 15 seconds... :D :/ 
			scanning = true;
		}
	}

	function doWifiScan(){
		if (scanning==true) fetchScannedList();
	}
	
	function stopScan(){
	  scanning = false;
	}

// Events
	window.onload = async () => {
		await fetchSavedSettings();
		await fetchScannedList();	// ALWAYS returns an empty array [] on first run !!!
		
	};
	</script>
</html>
<!-- HTML_SCRIPT_END -->
)=====";



















/*

	//replaced with universal saveWlan
	const saveSavedWlan = async (element) => {
		document.getElementById('blocker').classList.remove('hidden');
		console.log("Saving wifiSettings");
		const wlanIndex = element.id.split('-')[1];
		const wlan = savedWlans[wlanIndex];
		
		document.getElementById('blocker').classList.remove('hidden');
		document.getElementById('blocker-title').innerText = `Saving.`;
		
		var data = new FormData();
		data.append("ssid", wlan.ssid);
		data.append("password", wlan.password);
		try {
			const res = await fetch(window.location.href + '/save', {
				method: 'POST',
				body:data
			});
		} catch (err) {
			console.error(`Error: ${err}`);
			document.getElementById('blocker').classList.add('hidden');
		}
		fetchSavedSettings();
	}



	//  Delete Saved WLan
	const installCustomPaths = async (element) => {
		const ssid = document.getElementById('bin-path-input').value;			// need to know which id/row
		const password = document.getElementById('spiff-path-input').value;

		var data = new FormData();
		if(!binPath.isEmpty()){
		  data.append( "ssid", binPath);  
		}
		if(binPath!=""){
		  data.append( "Password", spiffPath);  
		}
		try {
			const res = await fetch(window.location.href + '/wifiSettings', {
				method: 'POST',
				body:data
			});
		} catch (err) {
			console.error(`Error: ${err}`);
			document.getElementById('blocker').classList.add('hidden');
		}
	}
*/



/*
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
		function readSettings() {
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
			doSend("?wifiScan");
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
			
			if (evt.data.substr(0,10) == "Restarting"){ 
				finished.innerHTML = 'Rebooting controller</br>Wait 10 seconds and click button below.</br><a http://' + window.location.hostname 
       */
/*
	'><button><font color="red">Back to Home</font></button></a>';
			}
			if (evt.data.substr(0,17) == "Upload incomplete"){
				spiffsProgressBar.innerHTML = "";
				binProgressDiv.innerHTML = "";
				finished.innerHTML = evt.data;
				//uploadButton.innerHTML = "Update Failed";
				//uploadButton.disabled = false;	  
			}
			if (evt.data.substr(0,20) == "No Updates available"){
				spiffsProgressDiv.innerHTML = "";
				binProgressDiv.innerHTML = "";
				finished.innerHTML = evt.data;
				//uploadButton.innerHTML = "No Updates available";
				//uploadButton.disabled = false;
			}
		  
		   switch(evt.data.substr(0,1)){      // update wifiScan bar %s		 example message s999424,1012528#84.20%
				case "s":   // received SPIFFS file wifiScan									
					recvdData = evt.data.substr(evt.data.indexOf("#")+1, evt.data.length - evt.data.indexOf("#"));
					//console.log("value of SPIFFS %= " + recvdData);
					spiffsBar.style.width=recvdData;
					spiffsBar.innerHTML=recvdData;
					
					spiffsBytes.innerHTML      = evt.data.substr(1,evt.data.indexOf(",")-1);
					totalSpiffsBytes.innerHTML = evt.data.substr(evt.data.indexOf(",")+1, evt.data.indexOf("#") - evt.data.indexOf(",")-1);
					break;
					
				case "b":   //received BIN file wifiScan
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
			doSend("?wifiScan");
			uploadButton.disabled = true;    // Disable button
		}
*/