
  var wsPort=)=====";

// concatenate with wsPort Arduino variable		

		
const char wifiPageB[] PROGMEM = R"=====(;
	var url = "ws://" + window.location.hostname + ":" + wsPort;
	var scannedWlans = [];
	var savedWlans = [];
	
	const delay = (ms => new Promise(resolve => setTimeout(resolve, ms)));	

	// scan for local wifi networks every 15 Seconds
	const scanInterval = setInterval(doWifiScan, 15000);
	
	// enables and disables scanning for WiFi networks (to allow the user to connect/edit without refresh being annoying)
	var scanning = true;

	// Pre load images for signal strength and security
	bar1 = new Image;
	bar2 = new Image;
	bar3 = new Image;
	bar4 = new Image;
	bar5 = new Image;
	padlockOpen = new Image;
	padlockClosed = new Image;
	
	var imageLoadFailed=false;
	
	try{
		bar1.src = "/bar1.png";		// weakest signal
	} catch(err) {
		imageLoadFailed=true;
	}
	try{
		bar2.src = "/bar2.png";
	} catch(err) {
		imageLoadFailed=true;
	}
	try{
		bar3.src = "/bar3.png";
	} catch(err) {
		imageLoadFailed=true;
	}
	try{
		bar4.src = "/bar4.png";
	} catch(err) {
		imageLoadFailed=true;
	}
	try{
		bar5.src = "/bar5.png";		// strongest signal
	} catch(err) {
		imageLoadFailed=true;
	}
	try{
		padlockOpen.src   = "/open.png";
	} catch(err) {
		imageLoadFailed=true;
	}
	try{
		padlockClosed.src = "/closed.png";
	} catch(err) {
		imageLoadFailed=true;
	}

	if (imageLoadFailed==true) {
		document.getElementById('imageDownload').classList.remove('hidden')
	}

	const updateSPIFFS = async () => {
		try{
			let resp = await fetch(window.location.href + '/updateSPIFFS');
			// further testing???
		} catch (err){
			console.log(`Error: ${err}`);
		}
	}


const signalImageTag = (rssi) => {
	var strengthImageTag="";

	switch(true){			// javascript can't use < or > in switch case clauses.
		// choose 1 of 4 strength indicator images based on value signal
		case (rssi >= -60):
			strengthImageTag = `<img src='${bar5.src}' alt='|||||' height='35px' border='0'>`;
			break;
		case (rssi >= -70):
			strengthImageTag = `<img src='${bar4.src}' alt='||||'  height='35px' border='0'>`;
			break;
		case (rssi >= -80):
			strengthImageTag = `<img src='${bar3.src}' alt='|||'  height='35px' border='0'>`;
			break;
		case (rssi >= -90):
			strengthImageTag = `<img src='${bar2.src}' alt='||'  height='35px' border='0'>`;
			break;
		case (rssi <-90):
			strengthImageTag = `<img src='${bar1.src}' alt='|'  height='35px' border='0'>`;
			break;
		default:	// no image
			break;
	}
	return strengthImageTag;
}


const securityImageTag = (encryption) =>  {
		var encryptionImageTag = "";
		switch(encryption){			// choose 1 of 2 encryption images
			case "C":
				encryptionImageTag=`<img src='${padlockClosed.src}' alt='Secured' height='35px' border='0'>`;
				break;
			case "O":	
				encryptionImageTag= `<img src='${padlockOpen.src}' alt='Open' height='35px' border='0'>`;
				break;
			default:	// no image
				break;
		}
		return encryptionImageTag;
}


	const createScannedRow = (scanWlan, index, connected = false) => {
		let child = document.createElement('div');

				// construct buttons html string so it can be concatenated later.. 
									// Chrome added </div> whre not needed
		var someButtons; 
		if(connected=="true"){
			someButtons=`
				<div class='btn-container'>
					${signalImageTag(scanWlan.rssi)}
					${securityImageTag(scanWlan.enc)} 
				</div>`;
		} else {
			someButtons =`
			<div class='btn-container' id='scanWlan-${index}-connect'>
				${signalImageTag(scanWlan.rssi)}
				${securityImageTag(scanWlan.enc)}
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
							scannedWlans.forEach((wlan, index) => {
								
								// search through the saved networks and put the signal strength 
								// next to saved instead of duplicating..
								var isSaved = false;
								var savedIndex;
								for(count=0;  count<savedWlans.length; count++){
									if (wlan.ssid == savedWlans[count].ssid){
										isSaved	= true;
										savedIndex = count;
										break;
									}
								}
								if(isSaved == true){
									if (wlan.cnctd == "true"){
										const elements = document.getElementsByClassName('connected');		//should only ever return 1 result
										try{
											console.log("# of connected networks : " + elements.length);
											elements[0].innerHTML = `Active
											${signalImageTag(wlan.rssi)}
											<a id='savedWlan-${savedIndex}-show' class='btn edit' onclick="(function(){
												document.getElementById('savedWlan-${savedIndex}-connect').classList.add('hidden');
												document.getElementById('savedWlan-${savedIndex}-edit').classList.remove('hidden');
												stopScan();
												})();">
												edit
											</a>`;
										} catch {
											console.log("No saved networks to set properties for");
										}
									} else {
										try{
											document.getElementById(`savedWlan-${savedIndex}-connect`).innerHTML = `${signalImageTag(wlan.rssi)}
											<a id='savedWlan-${savedIndex}-show' class='btn edit' onclick="(function(){
												document.getElementById('savedWlan-${savedIndex}-connect').classList.add('hidden');
												document.getElementById('savedWlan-${savedIndex}-edit').classList.remove('hidden');
												stopScan();
												})();">
												edit
											</a>`;
										} catch {
											console.log("No saved networks to set properties for");
										}
									}
								} else {
									container.appendChild(createScannedRow(wlan, index, wlan.cnctd));		// add a new row and call function above
								}
								isSaved=false;
							});
						listRetrieved = true;
					}
				} else {
				await delay(6000);
				}
			}
		} catch (err) {
			console.error(`Error: ${err}`);
		}
	}


	const createSavedRow = (savedWlan, index, connected=false) => {
		let child = document.createElement('div');
		
		if(connected=="false"){
			var someButtons =`
			<div class='btn-container' id='savedWlan-${index}-connect'>
				<a id='savedWlan-${index}-show' class='btn edit' onclick="(function(){
					document.getElementById('savedWlan-${index}-connect').classList.add('hidden');
					document.getElementById('savedWlan-${index}-edit').classList.remove('hidden');
					stopScan();
					})();">
					edit
				</a>
			</div>`;
		} else {
		var someButtons =`
			<div class='btn-container connected' id='savedWlan-${index}-connect'>
				Connected
				<a id='savedWlan-${index}-show' class='btn edit' onclick="(function(){
					document.getElementById('savedWlan-${index}-connect').classList.add('hidden');
					document.getElementById('savedWlan-${index}-edit').classList.remove('hidden');
					stopScan();
					})();">
					edit
				</a>
			</div>`;
		}
		
		child.innerHTML = `
		<div class='set'>
			<div class='row' id='savedWlan-${index}'>
				<div class='wifi-info' id='savedWlan-${index}-ssid'>
					<div class='wifi-name' id='scanWlan-${index}-ssid'>
						${savedWlan.ssid}
					</div>
				</div>
				<div class='column'>
						${someButtons}
				</div>
			</div>	
				<div class='spacer small'>	
					<!--  forces the row above to FILL a row, rather than wrapping multiple networks onto a single row -->
				</div>			
				<div class='row hidden' id='savedWlan-${index}-edit'>
					<label for='password'>Password:</label>
					<input type='text' name='password' id='savedWlan-${index}-password' value='${savedWlan.pass}'>
					<div class='btn-container'>
						<a class='btn save'		id='savedWlan-${index}-save'   	onclick='saveWlan(this)'>Save</a>
						<a class='btn delete'	id='savedWlan-${index}-delete'	onclick='deleteWlan(this)'>Delete</a>
						<a class='btn cancel'	id='savedWlan-${index}-cancel'	onclick="(function(){
							document.getElementById('savedWlan-${index}-password').value = ${savedWlans[index].password};
							document.getElementById('savedWlan-${index}-connect').classList.remove('hidden');
							document.getElementById('savedWlan-${index}-edit').classList.add('hidden');
							stopScan();
							})();">
						 Cancel</a>
					</div>
				</div>
			</div>`;
		return child;
	}


	const fetchSavedSettings = async () => {
		document.getElementById('page-loader').classList.remove('hidden');
		try {
			let listRetrieved = false;
			while(!listRetrieved) {
				let resp = await fetch(window.location.href + '/saved');
				let contentType = resp.headers.get("content-type");					
				if(contentType && contentType.indexOf("text/json") !== -1) {	
					savedWlans = await resp.json();										// received json data!!
					console.log("Length of savedWlans : " + savedWlans.length);
					const container = document.getElementById('saved');
					savedWlans.forEach((wlan,index) => {								// iterate through the list
					    container.appendChild(createSavedRow(wlan, index, wlan.cnctd));	// add a new row and call function above
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


const createNewSavedRow = (index) => {
		console.log("creating html elements");
		console.log("network " + index);
		let child = document.createElement('div');
		child.innerHTML = `
		<div class='set' id='new'>
			<div class='set-inputs' id='newWlan-${index}'>
				<div class='row'>
					<label for='ssid'>SSID:</label>
					<input type='text' name='ssid' id='newWlan-${index}-ssid' value=''>
				</div>
				<div class='row'>
					<label for='password'>Password:</label>
					<input type='text' name='password' id='newWlan-${index}-password' value=''>
				</div>
				<div class='spacer small'>	
					<!--  forces the row above to FILL a row, rather than wrapping multiple networks onto a single row -->
				</div>
				<div class='btn-container' id='newWlan-${index}-edit'>
					<a class='btn save' 	id='newWlan-${index}-save'   onclick='saveWlan(this)'>Save</a>
					<a class='btn cancel'	id='newWlan-${index}-cancel' onclick="(function(){
					  document.getElementById('new').remove();
					  startScan();
					  })();">
					  Cancel
					</a>
				</div>					
			</div>
		</div>`;
		return child;
	}


// add a new Saved Network
	const addNew = async () => {
		console.log("Adding a new network");
		document.getElementById('addButton').classList.add('hidden');
		const container = document.getElementById('saved');	
		container.appendChild(createNewSavedRow (savedWlans.length));
		stopScan();
	}

// Connect via WPS
const wpsConnect = async () => {
	//send a message to the Eesp32 to start WPS connection
	// need some UI( element to select PIN  or Push Button
	
	let resp = await fetch(window.location.href + '/wpsConnect/pin');
	
	//let resp = await fetch(window.location.href + '/wpsConnect/push');
	
	
	// need to setup wpsInitConfig
	// get response from AP the 
}

/** 
save A set of network credentials:
New Scanned
New Saved
edited Existing Saved						https://www.w3schools.com/jsref/jsref_obj_array.asp
*/
	const saveWlan = async (element) => {
		const nameParts  = element.id.split('-');
		const wlanPrefix = nameParts[0];
		const wlanIndex  = nameParts[1];
		
		switch(wlanPrefix){
			case "scanWlan":
				// figure out if this is already saved or not...
				var wlan = scannedWlans[wlanIndex];			// gets ssid here
/*
				var found = false;
				savedWlans.forEach(testingWlan, index){
					switch(true){
						case (testingWlan.ssid == wlan.ssid):
							found=true;
							wlan.wlanID = testingWlan.wlanID;	//??
							break;
					}
				}
				if(found ==true){
					wlan.ns = `wlan-${wlanIndex}`;			// copy from scanned to saved and append
					wlan.wlanID = wlanIndex;
				} else {
*/
					wlan.ns = `wlan-${savedWlans.length}`; // increase scanned
					wlan.wlanID = savedWlans.length;
//				}
				wlan.pass = document.getElementById(`scanWlan-${wlanIndex}-password`).value
				
				document.getElementById(`${wlanPrefix}-${wlanIndex}-edit`).classList.add('hidden');
				document.getElementById(`${wlanPrefix}-${wlanIndex}-connect`).classList.remove('hidden');
				
				console.log("after-set ns "			+ wlan.ns);
				console.log("after-set id "			+ wlan.wlanID);
				console.log("after-set ssid "		+ wlan.ssid);
				console.log("after-set passsword "	+ wlan.pass);
				
				console.log("saving a scanned wifi network");
				break;
				
			case "savedWlan":
				document.getElementById(`${wlanPrefix}-${wlanIndex}-edit`).classList.add('hidden');
				document.getElementById(`${wlanPrefix}-${wlanIndex}-connect`).classList.remove('hidden');
				var wlan = savedWlans[wlanIndex];
				wlan.ns = `${wlanIndex}`;
				//wlan.ssid = document.getElementById(`savedWlan-${wlanIndex}-ssid`).value   // fixed value
				wlan.pass = document.getElementById(`savedWlan-${wlanIndex}-password`).value

				console.log("after-set ns "		+ wlan.ns);
				console.log("after-set id "		+ wlan.wlanID);
				console.log("after-set ssid "	+ wlan.ssid);
				console.log("after-set passs "	+ wlan.pass);
				
				console.log("saving a saved wifi network");
				break;
				
			case "newWlan":
				document.getElementById(`${wlanPrefix}-${wlanIndex}-edit`).classList.add('hidden');
				var wlan = {
							"ns":`wlan-${savedWlans.length}`, 
							"wlanID": savedWlans.length,
							"ssid":	 document.getElementById(`newWlan-${wlanIndex}-ssid`).value,
							"pass":  document.getElementById(`newWlan-${wlanIndex}-password`).value
							};
				
				console.log("after-set ns "		+ wlan.ns);
				console.log("after-set id "		+ wlan.wlanID);
				console.log("after-set ssid "	+ wlan.ssid);
				console.log("after-set passs "	+ wlan.pass);
				
				// validate the data !!
				var errorSsid = false;
				
				if(wlan.ssid == "") {
					errorSsid = true;
					document.getElementById('blocker-title').innerHTML='SSID cannot be blank';
				}
				
				if (wlan.ssid.indexOf("?")!= -1){
					document.getElementById('blocker-title').innerHTML='SSID cannot contain ?';
					errorSsid = true;
				}
				
				if (wlan.ssid.indexOf(`"`)!= -1){
					document.getElementById('blocker-title').innerHTML='SSID cannot contain "';
					errorSsid = true;
				}
				
				if (wlan.ssid.indexOf("$")!= -1){
					document.getElementById('blocker-title').innerHTML='SSID cannot contain $';
					errorSsid = true;
				}
				
				if (wlan.ssid.indexOf("[")!= -1){
					document.getElementById('blocker-title').innerHTML='SSID cannot contain [';
					errorSsid = true;
				}
				
				if (wlan.ssid.indexOf(`\\`)!= -1){
					document.getElementById('blocker-title').innerHTML='SSID cannot contain \\';
					errorSsid = true;
				}
				
				if (wlan.ssid.indexOf("]")!= -1){
					document.getElementById('blocker-title').innerHTML='SSID cannot contain ]';
					errorSsid = true;
				}
				
				if(errorSsid == true){	//invalid chars in SSID
					document.getElementById('blocker').classList.remove('hidden');
					document.getElementById(`newWlan-${wlanIndex}-ssid`).classList.add('error');
					setTimeout(function() {
						document.getElementById('blocker').classList.add('hidden');
						document.getElementById(`newWlan-${wlanIndex}-edit`).classList.remove('hidden');
					}, 2000);
					return false; 
				}
				
				document.getElementById('addButton').classList.remove('hidden');
				break;
				
			default:
		}
		
		// Preferences can only store key values up to 15 Chars long.. so validate the SSID maximum length
		
		console.log("before save ns "		+ wlan.ns);
		console.log("before save ID "		+ wlan.wlanID);
		console.log("before save ssid "		+ wlan.ssid);
		console.log("before save passs "	+ wlan.pass);		

		var data = new FormData();
		data.append("ns",   wlan.ns);
		data.append("id",   wlan.wlanID);
		data.append("ssid", wlan.ssid);
		data.append("pass", wlan.pass);
		
		// show saving --> saved on blocker
		document.getElementById('blocker-title').innerHTML='Saving';
		document.getElementById('blocker').classList.remove('hidden');
		
		try {
			const res = await fetch(window.location.href + '/saveWlan', {	
				method: 'POST',
				body:data
			});
		} catch (err) {
			console.error(`Error: ${err}`);
		}
		document.getElementById('blocker').classList.add('hidden');
		document.getElementById('addButton').classList.remove('hidden');
		
		purgeSaved();
	}


//  editing
	const editWlan = async (element) => {
		console.log("Edit  saved network");

		const nameParts  = element.id.split('-');
		const wlanPrefix = nameParts[0];
		const wlanIndex  = nameParts[1];

		console.log("editing " + `${wlanPrefix}-${wlanIndex}-connect`);
		
		document.getElementById(`${wlanPrefix}-${wlanIndex}-connect`).classList.add('hidden');
		document.getElementById(`${wlanPrefix}-${wlanIndex}-edit`).classList.remove('hidden');
		stopScan();
	}
	
	
//  deleting
	const deleteWlan = async (element) => {
		console.log("Delete Saved Network");

		const nameParts  = element.id.split('-');
		const wlanPrefix = nameParts[0];
		const wlanIndex  = nameParts[1];

		document.getElementById(`${wlanPrefix}-${wlanIndex}`).innerHTML='';

		var wlan = savedWlans[wlanIndex];
		wlan.ns = `${wlanIndex}`;
	
		var data = new FormData();
		data.append("ns", wlan.ns);
				
		try {
			const res = await fetch(window.location.href + '/deleteWlan', {		//
				method: 'POST',
				body:data
			});
		} catch (err) {
			console.error(`Error: ${err}`);
		}

		purgeSaved();							
	}

	function purgeSaved(){
		console.log("purging saved networks ");
		savedWlans=null;								// throw away the data
		document.getElementById('saved').innerHTML="";	// throw away the display elements
		fetchSavedSettings();							// this will re-populate the data and refresh the display
		startScan();
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