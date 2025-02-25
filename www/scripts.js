updatehw = 0;
function show(tab, tipo){
	const main_menu = ['menu-info','menu-config','menu-teste','menu-update'];
	const conf_menu = ['tab-server','tab-wifi-cli','tab-wifi-ap','tab-password','tab-hardware'];
	for (let i = 0; i < 5; i++) {
		document.getElementById(conf_menu[i]).style.display = 'none';
	}
	if(tipo=='main') {
		for (let i = 0; i < 4; i++) {
			document.getElementById(main_menu[i]).style.display = 'none';
		}
	}
	document.getElementById(tab).style.display = 'block';
	updatehw = 0;
	if(tab == 'menu-teste') updatehw = 1;
}

function desabilita_addr(desabilitar) {
	const itens=['CLI_IP','CLI_MASK','CLI_GW','CLI_DNS'];
	for(let i=0;i<4;i++) {
		for(let c=1;c<=4;c++) {
			document.getElementById(itens[i] + '_' + c).disabled = desabilitar;
		}
	}
}

function reboot() {
	url = '/reboot';
	var xhr = new XMLHttpRequest();
	xhr.open('GET', url, true);
	xhr.send(null);
}

function test_rl(num) {
	url = '/test_relay/' + num;
	var xhr = new XMLHttpRequest();
	xhr.open('GET', url, true);
	xhr.send(null);
}

function test_led(num) {
	url = '/test_led/' + num;
	var xhr = new XMLHttpRequest();
	xhr.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200) {
			resp = xhr.responseText;
			stat = resp.substring(0,1);
			document.getElementById('ld1').style.backgroundColor = (stat=='1')?'yellow':'grey';
			stat = resp.substring(1,2);
			document.getElementById('ld2').style.backgroundColor = (stat=='1')?'blue':'grey';
		}
	};
	xhr.open('GET', url, true);
	xhr.send(null);
}

function test_bt() {
	if(updatehw==1) {
		var xhr = new XMLHttpRequest();
		xhr.onreadystatechange = function() {
			if (this.readyState == 4 && this.status == 200) {
				resp = xhr.responseText;
				for(let i=1;i<=5;i++) {
					stat = resp.substring(i-1,i);
					var e = document.getElementById('tb' + i);
					e.style.backgroundColor = (stat=='1')?'green':'red';
				}
			}
		};
		xhr.open("GET", "/buttons", true);
		xhr.send();
		test_led(0);
	}
}
function filesystem() {
	window.location.href="/fs";
}
setInterval(test_bt, 1000);

function update_fields(data) {
	field_list = ['serverName','CLI_wifi_SSID','CLI_wifi_password','AP_SSID','AP_password','password'];
	for(i = 0; i < field_list.length; i++) {
		key = field_list[i];
		value = data[key];
		document.getElementById(key).value = value;
	}
	document.getElementById('CLI_DHCP1').checked=(data.CLI_DHCP=='dhcp');
	document.getElementById('CLI_DHCP2').checked=(data.CLI_DHCP=='fixo');
	desabilita_addr(data.CLI_DHCP=='dhcp');
	list4fields =[ 'CLI_IP','CLI_MASK','CLI_GW','CLI_DNS','digitalInput','relayOutput' ];
	for(i = 1; i <= 4; i++) {
		for(j = 0; j < 6; j++) {
			key = list4fields[j] + '_' + i;
			value = data[list4fields[j]][i-1];
			document.getElementById(key).value = value;
			console.log('[' + key + '> ' + value);
		}
	}
	document.getElementById('version').innerHTML = 'Vers&atilde;o: ' + data.version;
	document.getElementById('serialNumber').innerHTML = 'Serial: ' + data.serialNumber;
}

function update_config() {
	fetch('/config.json')
		.then(res => res.json())
		.then((out) => {
			update_fields(out);
			show('menu-info','main');
	}).catch(err => console.error(err));
}