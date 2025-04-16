# Computer Sidekick

The main goal of this projet is make an assistant to monitor, control power on/off and display informations about a PC computer. They can be instaled inside the case and an LCD or OLED display can be placed on a plate on the diskette or cdrom bay.

When this text refer to ESP board it refer to any ESP32 or ESP8266 boards, the code has many "#ifdef" preprocessor commands to use the correct software for each architeture.

Before compile and transfer this software to an ESP board, first you need to edit the 'config.h' file and set all hardware pins you will use in your setup, this file will be used to insert all the software parts to each item you enable.


The components you can add in the 'config.h' are:

* __Buttons__: You can use the power button from PC case or other button to control the power on/off (this can be controlled remotelly by a web page too), the reset button (same way, you can use the PC case button or another one) and the configuration button, your PC case don't have an button to this feature, you can add a new button or use the case reset button to do this function, if you need to reset, just turn off and turn on your computer, in fact this will be done when you don't configure a reset button and use the web page to request a reset. You need to select the level when the button is pressed, usually I set to LOW and this software will setup the pin to input with a internal pull up resistor, so I attach one button pin to the GPIO that will read the value and the other pin to the ground (GND).

* __LEDs__: The common PC motherboard has two LEDs output used to indicate the Power ON and the HDD access. You can use the positive (+) pin from this LED at the motherboard to read this information, if you do this, the software will use the information to perform the power on and power off cicly monitoring the status of the motherboard Power LED, the HDD led is just to show the HDD status on display.

* __Displays__: You can use text displays (16x2 or 16x4), or OLED displays (126x32 or 128x64) both connected over an SPI interface or just send the information trought the Serial port attached in the USB connector. When you select an SPI display, you need to provide the SPI pins (SDA, SCL).

* __Relays__: The power pins on the motherboard usually is attached to power button, using this software, if you attach this pins to a relay, you can control power on/power off. If the power button was configured, they will control the power relay, otherwise you can control just from the web interface. The same way you can use another relay to connect to the reset pins at motherboard. You can use 4 relays to control fans in your case, just use the relays as a key to provide power, the control can be performed manually by the web interface or automatically setting a temperature trigger when you use a termometer. You need to set the default level to turn on the relay, some relay boards use the LOW level to active. note: you can use a common mecanic relay or a solid state relay, the last one is silent while the first one will sound a click when change state.

* __RPMs__: Is possible to measure the coolers/fans RPMs of the CPU and almost 4 other fans in the computer case. You need to configure what GPIO pin is connected to the sensor from the fan.

* __Temperature__: The software was prepared to use the DS18B20 temperature sensors, you just need to configure one pin and use it to connect to all sensors. When you start the ESP they will search for all sensors using 1wire protocol, so you need to enter in the setup web page to map each sensor with your location (CPU, System#1 to System#4).

* __UART__: If you like to show computer statistics like CPU/Disk/Memory usage, load average or other informations, you need to connect an UART (Aka serial port) to the computer and use a software to send the statistics. This can be done on Windows ou Linux environments. You need to configure the TX and RX pins, and don't forget to use a MAX232 to compatibilize the ESP Uart level to the real RS-232 Serial port or you will burn you ESP.

The ESP board has a LED than can be controlled by the user program, you can use this LED to see the activity of the board or use a GPIO to connect an external LED. The software will flash the LED to sign any activity or status.

When you setup your ESP for the first time, you don't have a setup stored at the file system yet, so the software will configure the ESP as a AP (Access Point), the default SSID and password can be setted in the 'config.h'. When the AP mode was active, the ESP assume the static IP 192.168.100.1 and you can access the web interface by this IP.

The last configuration you can do is the date and time settings, you can set the timezone (see the 'config.h' file to find the url to valids timezone informations), you can set the NTP server list (the software will try one by one until get the date and time), and the date and time format used to display.

After first boot the web page will inform you that there is no 'index.html' file and place a link to file manager, you need to upload the files from 'www' directory. When you finnish, the main page can be acessed and the setup toolbar will be activated by click at the logo on top. Now you can set your wifi network information (SSID and password), the IP can be fixed or via dhcp (in this case you can see the assigned IP at the display). You can set the AP mode SSID and password to be used when you enter in the AP mode insted the default ones, the AP mode will be activated by pressing the configuration button or if the ESP can't connect to your wifi network tree times in a row. The AP mode is just needed to change the client wifi network SSID and password.

I'm testing on ESP32 WROOM (30 pin model) and ESP-01 (ESP8266) with one relay board.

Um esquema completo de conexões usando o ESP32.

![Esquema com ESP32]https://github.com/domingosparaiso/Sidekick/blob/main/doc/Esquema.jpg