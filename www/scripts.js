updatehw = 0;
updatepg = 0;
timeoutb = 0;
led_power = 'OFF';

function show_hide_main_menu(){
	topmenu = document.getElementById('top-menu');
	if(topmenu.style.display == 'block') {
		topmenu.style.display = 'none';
	} else {
		topmenu.style.display = 'block';
	}
	show('menu-control','main');
}

function show(tab, tab_type){
	const collection_tab = document.getElementsByClassName("tab-menu");
	for (let i = 0; i < collection_tab.length; i++) {
		collection_tab[i].style.display = 'none';
	}
	if(tab_type=='main') {
		const collection_menu = document.getElementsByClassName("upper-menu");
		for (let i = 0; i < collection_menu.length; i++) {
			collection_menu[i].style.display = 'none';
		}
	}
	document.getElementById(tab).style.display = 'block';
	if(tab == 'menu-setup') show('tab-configuration','');
}

function disable_addr(disable) {
	const itens=['CLI_IP','CLI_MASK','CLI_GW','CLI_DNS'];
	for(let i=0;i<4;i++) {
		for(let c=1;c<=4;c++) {
			document.getElementById(itens[i] + '_' + c).disabled = disable;
		}
	}
}

function reboot() {
	fetch('/reboot')
		.then(x => x.text())
		.then((out) => {
			document.getElementById('bodydiv').innerHTML = out;
			timeoutb = 10;
			setInterval(function () {
				if(timeoutb >= 0) {
					msg = '<h1>' + timeoutb + '</h1>';
					if(timeoutb == 0) location.reload();
				} else {
					msg = '<h1>Reloading...</h1>';
					ip = '';
					sep = '';
					for(i=1;i<=4;c++) {
						ip += sep + document.getElementById('CLI_IP_' + i).value;
						sep = '.';
					}
					window.location.href="http://" + ip;
				}
				document.getElementById('bodydiv').innerHTML = out + msg;
				timeoutb--;
			}, 1000);
		})
		.catch(err => console.error(err));
}

function filesystem() {
	window.location.href="/fs";
}

function update_fields(data) {
	field_list = ['userName','hostname','CLI_wifi_SSID','CLI_wifi_password','AP_SSID','AP_password','password', 'timeout'];
	for(i = 0; i < field_list.length; i++) {
		key = field_list[i];
		value = data[key];
		document.getElementById(key).value = value;
	}
	document.getElementById('username').innerHTML = 'Host: ' + data['hostname'];
	document.getElementById('CLI_DHCP1').checked=(data.CLI_DHCP=='dhcp');
	document.getElementById('CLI_DHCP2').checked=(data.CLI_DHCP=='fixo');
	disable_addr(data.CLI_DHCP=='dhcp');
	list4fields =[ 'CLI_IP','CLI_MASK','CLI_GW','CLI_DNS' ];
	for(i = 1; i <= 4; i++) {
		for(j = 0; j < 4; j++) {
			key = list4fields[j] + '_' + i;
			value = data[list4fields[j]][i-1];
			document.getElementById(key).value = value;
		}
	}
	maplist =[ '', 'cpu', 'sys1', 'sys2', 'sys3', 'sys4' ];
	for(i = 1; i <= 5; i++) {
		key = 'TMAP_' + i;
		mapindex = data['map'][i-1];
		value = maplist[mapindex];
		document.getElementById(key).value = value;
		if(i > data.countmap) {
			document.getElementById(key).style.display = 'none';
		} else {
			document.getElementById(key).style.display = 'block';
		}
	}
}

function button_action(button_name) {
	if(button_name == 'power') {
		cmd = 'POWER_ON';
		if(led_power == 'ON') cmd = 'POWER_OFF';
	} else {
		cmd = 'ON';
	}
	fetch('/relay/' + button_name + "?cmd=" + cmd);
}

function apply_led(name, out) {
	el = document.getElementById('led_' + name);
	if(!el) return;
	if(out == 'ON') delclass = 'led_OFF';
	if(out == 'OFF') delclass = 'led_ON';
	if(el.classList.contains(delclass)) el.classList.remove(delclass);
	el.classList.add('led_' + out);
	el.innerHTML = out;
	if(name == 'power') led_power = out;
}

function update_led(name) {
	fetch('/led/' + name)
		.then(x => x.json())
		.then((j) => apply_led(name, j['led_' + name]))
		.catch(err => console.error(err));
}

ledWs = null;
ledStarted = false;

function led_connect() {
	ledWs = new WebSocket('ws://' + location.host + '/led-ws');
	ledWs.onmessage = (event) => {
		data = {};
		try { data = JSON.parse(event.data); } catch(err) { return; }
		if(data.led_power != undefined) apply_led('power', data.led_power);
		if(data.led_hdd != undefined) apply_led('hdd', data.led_hdd);
	};
	ledWs.onclose = () => setTimeout(led_connect, 2000);
	ledWs.onerror = () => ledWs.close();
}

function make_buttons(data_relays, data_leds) {
	var result = "";
	if(data_leds != undefined) {
		for(i = 0; i < data_leds.length; i++) {
			value = data_leds[i];
			if(value == 'power') {
				txt = "POWER";
			}
			if(value == 'hdd') {
				txt = "HDD";
			}
			result += "<div class='group-ctrl'><div class='control-led led_OFF' id='led_" + value + "'>OFF</div><div>" + txt + "</div></div>";
			update_led(value); // initial led state, further updates arrive over the '/led-ws' websocket
		}
		if(data_leds.length > 0 && !ledStarted) {
			ledStarted = true;
			led_connect();
		}
	}
	if(data_relays != undefined) {
		for(i = 0; i < data_relays.length; i++) {
			value = data_relays[i];
			if(value == 'power') {
				img = "power.png";
				txt = "POWER";
			}
			if(value == 'reset') {
				img = "reset.png";
				txt = "RESET";
			}
			result += "<div class='group-ctrl'><div class='control-button' onclick=button_action('" + value + "')><img src='" + img + "' width=32 height=32></div><div>" + txt + "</div></div>";
		}
	}
	return(result);
}

function make_list(header, data) {
	var result = "";
	if(data != undefined && data.length > 0) {
		result = "<tr><td>" + header + ":</td><td>[ ";
		var sep = "";
		for(i = 0; i < data.length; i++) {
			value = data[i];
			result += sep + value;
			sep = ", ";
		}
		result += " ]</td></tr>";
	}
	return(result);
}

function update_resources(data) {
	buttons = make_list('Buttons', data.button);
	leds = make_list('Leds', data.led);
	relays = make_list('Relays', data.relay);
	rpms = make_list('Coolers', data.rpm);
	temperature = make_list('Temperature', data.temperature);
	if(temperature != '') {
		document.getElementById('no-hardware').style.display = 'none';
		document.getElementById('temperature-table').style.display = 'block';
	}
	const consolebtn = document.getElementById('menu-console-button');
	if(consolebtn) consolebtn.style.display = (data.console != undefined) ? 'block' : 'none';
	Htable = "<table>" +
		"<tr><td>Serial</td><td>" + data.serialNumber + "</td></tr>" +
		"<tr><td>Version</td><td>" + data.version + "</td></tr>" +
		"<tr><td>Board</td><td>" + data.board + "</td></tr>" +
		"<tr><td>Frequency</td><td>" + data.mhz + " Mhz</td></tr>" +
		"<tr><td>Uptime</td><td>" + data.uptime + "</td></tr>" +
		"<tr><td>Serial Console</td><td>" + data.serialconsole + "</td></tr>" +
		"<tr><td>Display</td><td>" + data.display + "</td></tr>" +
		buttons +
		leds +
		relays +
		rpms +
		temperature +
		"</table>";
	document.getElementById('configuration-table').innerHTML = Htable;
	document.getElementById('control-contents').innerHTML = make_buttons(data.relay, data.led);
	if(data.serialconsole.startsWith('yes')) {
		lazyload();
	}
}

function update_page() {
	updatepg++;
	if(updatepg == 2) show('menu-control','main');
}

consoleTerm = null;
consoleFit = null;
consoleWs = null;
consoleStarted = false;

function console_connect() {
	statusEl = document.getElementById('console-status');
	consoleWs = new WebSocket('ws://' + location.host + '/console-ws');
	consoleWs.binaryType = 'arraybuffer';

	consoleWs.onopen = () => {
		statusEl.textContent = 'connected';
		statusEl.className = 'console-status console-online';
		consoleTerm.writeln('\r\n\x1b[32m*** WebSocket connected ***\x1b[0m\r\n');
	};

	consoleWs.onclose = () => {
		statusEl.textContent = 'disconnected';
		statusEl.className = 'console-status console-offline';
		consoleTerm.writeln('\r\n\x1b[31m*** WebSocket disconnected. Reconnecting... ***\x1b[0m\r\n');
		setTimeout(console_connect, 2000);
	};

	consoleWs.onerror = () => consoleWs.close();

	consoleWs.onmessage = (event) => {
		if(event.data instanceof ArrayBuffer) {
			consoleTerm.write(new Uint8Array(event.data));
		} else {
			consoleTerm.write(event.data);
		}
	};
}

function open_console() {
	if(consoleStarted) {
		if(consoleFit) consoleFit.fit();
		return;
	}
	consoleStarted = true;
	consoleTerm = new Terminal({ cursorBlink: true, convertEol: true, fontSize: 14, theme: { background: '#1e1e1e' } });
	consoleFit = new FitAddon.FitAddon();
	consoleTerm.loadAddon(consoleFit);
	consoleTerm.open(document.getElementById('console-terminal'));
	consoleFit.fit();
	window.addEventListener('resize', () => { if(consoleFit) consoleFit.fit(); });

	// Everything typed in the terminal is sent to the ESP32
	consoleTerm.onData((data) => {
		if(consoleWs && consoleWs.readyState === WebSocket.OPEN) consoleWs.send(data);
	});

	console_connect();
}

function logout() {
	fetch('/logout', {
		method: 'GET'
	})
	.then(response => {
		if (!response.ok) throw new Error(`Erro: ${response.status}`);
		window.location.href = '/';
	});
}

lazyLoaded = false;

function lazyload() {
	// Lazy load of scripts and css only when console serial is enabled
	// https://cdn.jsdelivr.net/npm/xterm@5.3.0/css/xterm.css
	// https://cdn.jsdelivr.net/npm/xterm@5.3.0/lib/xterm.js
	// https://cdn.jsdelivr.net/npm/xterm-addon-fit@0.8.0/lib/xterm-addon-fit.js
	if(lazyLoaded) return Promise.resolve();
	lazyLoaded = true;

	const link = document.createElement('link');
	link.rel = 'stylesheet';
	link.href = 'xterm.css';
	document.head.appendChild(link);

	return new Promise((resolve) => {
		const script1 = document.createElement('script');
		script1.src = 'xterm.js';
		script1.onload = () => {
			const script2 = document.createElement('script');
			script2.src = 'xterm-addon-fit.js';
			script2.onload = resolve;
			document.head.appendChild(script2);
		};
		document.head.appendChild(script1);
	});
}

let modalCallback = null;

function showConfirm(message, callback) {
	document.getElementById('confirmMessage').innerHTML = message;
	modalCallback = callback;
	document.getElementById('confirmModal').classList.add('modal-show');
}

function modalConfirm() {
	document.getElementById('confirmModal').classList.remove('modal-show');
	if(modalCallback) {
		modalCallback();
	}
	modalCallback = null;
}

function modalCancel() {
	document.getElementById('confirmModal').classList.remove('modal-show');
	modalCallback = null;
}

function confirmReboot() {
	showConfirm('This will reboot the device, confirm?', function() {
		reboot();
	});
}

window.onload = function() {
	fetch('/config.json')
		.then(res => res.json())
		.then((out) => {
			update_fields(out);
			update_page();
	}).catch(err => console.error(err));
	fetch('/resources.json')
		.then(res => res.json())
		.then((out) => {
			update_resources(out);
			update_page();
	}).catch(err => console.error(err));
};

