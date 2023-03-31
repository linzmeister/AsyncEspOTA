<body>
    <div class="spacer">
	</div>
	
	<div class="container" id="gpio-container">
		<div id="remote-header-bar" class="header-container">
			<h2>Install from Internet</h2>
		</div>
		
		<div id="remote" class="column">
			<div class="row" id="rowWlan-0">
					<div class="general-info">
						<div class="repo-name">
							IoTEasiestOTAforEndUsers
						</div>
						<div class="repo-description">
							The world's easiest IoT device for END USERS to perform WiFi provisioning with OTA updates
						</div>
					</div>
					<div class="btn-container">
						<a onclick="fetchWlanInfo(this)" id="getWlanInfo-0" class="btn edit">Check versions</a>
					</div>
				</div>
				
				<div class="row" id="rowWlan-1">
					<div class="general-info">
						<div class="repo-name">
							BasicAsyncEspOTA
						</div>
						<div class="repo-description">
							a simplified example of OTA
						</div>
					</div>
					<div class="btn-container">
						<a onclick="fetchWlanInfo(this)" id="getWlanInfo-1" class="btn edit">Check versions</a>
					</div>
					<div class="row more-info" id="rowRepoInfo-1">
						<div class="repo-info">
							<select id="version-1" name="versions"><option value="2.5.19">2.5.19 (last)</option><option value="1.2">1.2</option><option value="1.0.0">1.0.0</option></select>
						</div>
						<div class="btn-container">
							<a onclick="install(this)" id="getRepoInfo-1" class="btn save">Install</a>
						</div>
					</div>
				</div>
				
				<div class="row" id="rowWlan-2">
					<div class="general-info">
						<div class="repo-name">
							app 1
						</div>
						<div class="repo-description">
							some whizz bang doohickey
						</div>
					</div>
					<div class="btn-container">
						<a onclick="fetchWlanInfo(this)" id="getWlanInfo-2" class="btn edit">Check versions</a>
					</div>
				</div>
				
				<div class="row" id="rowWlan-3">
					<div class="general-info">
						<div class="repo-name">
							app 2
						</div>
						<div class="repo-description">
							just an above average IoT OTA example - with SPIFFS Files System data
						</div>
					</div>
					<div class="btn-container">
						<a onclick="fetchWlanInfo(this)" id="getWlanInfo-3" class="btn edit">Check versions</a>
					</div>
				</div>
			</div>
		</div>

		<div class="container" id="automation-container">
			<div id="local-header-bar" class="header-container">
				<h2>Install from custom URL</h2>
			</div>
			<div id="local" class="column">
				<div class="set">
					<div class="set-inputs">
						<div class="row">
							<label for="spiff-path">Spiff file URL:</label>
							<input type="text" name="spiff" id="spiff-path-input">
						</div>
						<div class="row">
							<label for="bin-path">Bin file URL</label>
							<input type="text" name="bin" id="bin-path-input">
						</div>
						<a id="submit-update-file" onclick="installCustomPaths()" class="btn save disable">Update</a>
					</div>
				</div>
			</div>
		</div>
		
		
		<div id="blocker" class="hidden">
			<h2 id="blocker-title">Loading</h2>
			<div id="Updating'">
				<div id="spiffsProgressDiv" width:80%="">
					<div id="spiffsProgressBar" class="w3-container w3-green" style="height:24px width:0%">
					</div>
				</div>
				<p id="spiffsP">
					SPIFFS: Writing <span id="spiffsBytes">0</span> bytes of <span id="totalSpiffsBytes">0</span>.
				</p>
				<div id="binProgressDiv" width:80%="">
					<div id="binProgressBar" class="w3-container w3-green" style="height:24px width:0%">
					</div>
				</div>
				<p id="binP">
					FIRMWARE: Writing <span id="binBytes">0</span> bytes of <span id="totalBinBytes">0</span>.
				</p>
				<p id="finished"></p>
		</div>