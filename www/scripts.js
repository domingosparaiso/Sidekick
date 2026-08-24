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
				}
				document.getElementById('bodydiv').innerHTML = out + msg;
				timeoutb--;
			}, 1000);
	}).catch(err => console.error(err));
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

function update_led(name) {
	fetch('/led/' + name)
		.then(x => x.json())
		.then((j) => {
			out = j['led_' + name];
			if(out == 'ON') delclass = 'led_OFF';
			if(out == 'OFF') delclass = 'led_ON';
			el = document.getElementById('led_' + name);
			if(el.classList.contains(delclass)) el.classList.remove(delclass);
			el.classList.add('led_' + out);
			el.innerHTML = out;
			if(name == 'power') led_power = out;
		}).catch(err => console.error(err));
}

function update_led_power() {
	update_led('power');
}

function update_led_hdd() {
	update_led('hdd');
}

function make_buttons(data_buttons, data_leds) {
	var result = "";
	var powerButton = false;
	if(data_buttons != undefined) {
		for(i = 0; i < data_buttons.length; i++) {
			value = data_buttons[i];
			if(value == 'power') powerButton = true;
			result += "<div class='control-button' onclick=button_action('" + value + "')><img src='power.png' width=32 height=32></div>";
		}
	}
	if(data_leds != undefined) {
		for(i = 0; i < data_leds.length; i++) {
			value = data_leds[i];
			result += "<div class='control-led led_OFF' id='led_" + value + "'>OFF</div>";
			switch(value) {
				case 'power':
					setInterval(function () { update_led_power(); }, 1000);
					if(!powerButton) {
						result += "<div class='control-button' onclick=button_action('" + value + "')><img src='power.png' width=50px height=50px></div>";
					}
					break;
				case 'hdd':
					setInterval(function () { update_led_hdd(); }, 1000);
					break;
			}
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
	document.getElementById('menu-console-button').style.display = (data.console != undefined) ? 'block' : 'none';
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
	document.getElementById('control-contents').innerHTML = make_buttons(data.button, data.led);
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

