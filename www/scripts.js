updatehw = 0;

function show_hide_main_menu(){
	topmenu = document.getElementById('top-menu');
	if(topmenu.style.display == 'block') {
		topmenu.style.display = 'none';
	} else {
		topmenu.style.display = 'block';
	}
	show('menu-controle','main');
}

function show(tab, tipo){
	const main_menu = ['menu-controle','menu-config','menu-update'];
	const conf_menu = ['tab-server','tab-wifi-cli','tab-wifi-ap','tab-hardware'];
	for (let i = 0; i < 4; i++) {
		document.getElementById(conf_menu[i]).style.display = 'none';
	}
	if(tipo=='main') {
		for (let i = 0; i < 3; i++) {
			document.getElementById(main_menu[i]).style.display = 'none';
		}
	}
	document.getElementById(tab).style.display = 'block';
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

function filesystem() {
	window.location.href="/fs";
}

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
	list4fields =[ 'CLI_IP','CLI_MASK','CLI_GW','CLI_DNS' ];
	for(i = 1; i <= 4; i++) {
		for(j = 0; j < 4; j++) {
			key = list4fields[j] + '_' + i;
			value = data[list4fields[j]][i-1];
			document.getElementById(key).value = value;
		}
	}
	document.getElementById('information').innerHTML = 'Serial: ' + data.serialNumber +
													'&nbsp;Vers&atilde;o: ' + data.version;
}

window.onload = function() {
	fetch('/config.json')
		.then(res => res.json())
		.then((out) => {
			update_fields(out);
			show('menu-controle','main');
	}).catch(err => console.error(err));
};

