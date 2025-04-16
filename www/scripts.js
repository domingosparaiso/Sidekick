updatehw = 0;
updatepg = 0;
timeoutb = 0;

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
	field_list = ['serverName','CLI_wifi_SSID','CLI_wifi_password','AP_SSID','AP_password','password', 'timeout'];
	for(i = 0; i < field_list.length; i++) {
		key = field_list[i];
		value = data[key];
		document.getElementById(key).value = value;
	}
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
	fetch('/button/' + button_name);
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
			result += "<div class='control-button' onclick=button_action('" + value + "')>" + value + "</div>";
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
						result += "<div class='control-button' onclick=button_action('" + value + "')>" + value + "</div>";
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
		document.getElementById('temperature-table').style.display = 'block';
	}
	Htable = "<table>" +
		"<tr><td>Serial</td><td>" + data.serialNumber + "</td></tr>" +
		"<tr><td>Version</td><td>" + data.version + "</td></tr>" +
		"<tr><td>Board</td><td>" + data.board + "</td></tr>" +
		"<tr><td>Frquency</td><td>" + data.mhz + " Mhz</td></tr>" +
		"<tr><td>Display</td><td>" + data.display + "</td></tr>" +
		buttons +
		leds +
		relays +
		rpms +
		temperature +
		"</table>";
	document.getElementById('configuration-table').innerHTML = Htable;
	document.getElementById('menu-control').innerHTML = make_buttons(data.button, data.led);
}

function update_page() {
	updatepg++;
	if(updatepg == 2) show('menu-control','main');
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

