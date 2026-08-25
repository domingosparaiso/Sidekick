# Webserver API

All endpoints below (except `/login`) require a valid session cookie (`mySessionID`), obtained by
logging in at `/login` with the `userName`/`password` stored in the configuration.
The cookie is renewed on every authenticated request; a session that stays idle for 5 minutes is
invalidated server-side (the cookie itself is set with `Max-Age=3600`, i.e. 1h, as a browser-side cap).

Two pages can be customized by uploading a file with the matching name to the LittleFS storage
(via `/fs`): `login.html` replaces the login page, `fs.html` replaces the file manager page. When
the file isn't present, a minimal built-in fallback page is served instead.

## Authentication

| Endpoint | Method | Description |
|---|---|---|
| `/login` | GET | Show the login page (`login.html` from storage, or a hardcoded fallback) |
| `/login` | POST (`userName`, `password`) | Validate against the stored configuration, set the session cookie and redirect to `/` |
| `/logout` | GET | Invalidate the current session cookie |

## Application / storage

| Endpoint | Method | Description |
|---|---|---|
| `/` , `/index.html` | GET | Main application page (`index.html` from storage) |
| `/upload` | GET | Firmware upload page (OTA) |
| `/update` | POST (upload) | Firmware update backend (OTA) |
| `/reboot` | GET | Reboot device |
| `/setup` | POST | Configuration backend, receives data from the setup form |
| `/resources.json` | GET | All resources available at compile time, plus device uptime |
| `/config.json` | GET | Current configuration, in JSON format |
| `/config.bin` | GET | Always returns 403 — the raw config file (holds userName/password) is never exposed over HTTP |
| `/fs` | GET | Storage file manager: serves `fs.html` from storage if present, otherwise a built-in file list page |
| `/filelist` | GET | List of files in storage as JSON (used by the `/fs` page) |
| `/fs-upload` | POST (upload) | File upload backend for the storage file manager |
| `/delete?arq=<file>` | GET | Delete one or more files from storage (repeat `arq` per file) |
| `/format` | GET | Format the storage (delete all files) |

## LED status

| Endpoint | Method | Description |
|---|---|---|
| `/led/power` | GET | Read the Power LED value (ON/OFF) |
| `/led/hdd` | GET | Read the HDD LED value (ON/OFF) |
| `/led-ws` | WebSocket (ESP32 only) | Pushes `{ "led_power": "ON"/"OFF" }` / `{ "led_hdd": "ON"/"OFF" }` whenever a value changes, instead of the browser polling `/led/power` and `/led/hdd`. On ESP8266 (no WebSocket support), the browser only reads the initial value from the HTTP endpoints above |

## Console

| Endpoint | Method | Description |
|---|---|---|
| `/console-ws` | WebSocket (ESP32 only) | Bridges `HOST_SERIAL` to the browser as a remote ANSI terminal: bytes received on the UART are pushed to connected clients, and keystrokes sent by the client are written back to the UART |

## Buttons

| Endpoint | Method | Description |
|---|---|---|
| `/button/power` | GET | Press the power button (liga/desliga) |
| `/button/get/power` | GET | Read pressed power button value |
| `/button/reset` | GET | Press the reset button; if there is no reset relay, uses a power off/power on sequence instead |
| `/button/get/reset` | GET | Read pressed reset button value |
| `/button/reconfigure` | GET | Reconfigure WiFi into Access Point (AP) mode |
| `/button/get/reconfigure` | GET | Read pressed reconfigure button value |

## Relays

To send values to relays, the `cmd` value can be one of:

| Command | Effect |
|---|---|
| `ON` | Turn ON relay and stay in this state |
| `OFF` | Turn OFF relay and stay in this state |
| `POWER_ON` | Turn ON relay, after a timeout turn it OFF (used by the Power relay) |
| `POWER_OFF` | Turn OFF relay, after a timeout turn it OFF (used by the Power relay) |
| `POWER_OFF_ON` | Turn ON relay, wait and turn OFF, wait and turn ON again, wait and turn OFF (used to restart the computer when no reset relay exists) |
| `RESET` | Turn ON relay, after a timeout turn it OFF (used by the Reset relay) |

All timeouts are configured in `config.h`, values in ms.

| Endpoint | Method | Description |
|---|---|---|
| `/relay/power?cmd=<COMMAND>` | GET | Send command to the Power relay |
| `/relay/reset?cmd=<COMMAND>` | GET | Send command to the Reset relay |
| `/relay/sys1?cmd=<COMMAND>` | GET | Send command to the relay connected to the cooler at System #1 |
| `/relay/sys2?cmd=<COMMAND>` | GET | Send command to the relay connected to the cooler at System #2 |
| `/relay/sys3?cmd=<COMMAND>` | GET | Send command to the relay connected to the cooler at System #3 |
| `/relay/sys4?cmd=<COMMAND>` | GET | Send command to the relay connected to the cooler at System #4 |
| `/backlight/on` , `/backlight/off` | GET | Turn the display backlight relay ON/OFF |

## RPM

| Endpoint | Method | Description |
|---|---|---|
| `/rpm/cpu` | GET | Get CPU cooler RPM value |
| `/rpm/sys1` | GET | Get System #1 cooler RPM value |
| `/rpm/sys2` | GET | Get System #2 cooler RPM value |
| `/rpm/sys3` | GET | Get System #3 cooler RPM value |
| `/rpm/sys4` | GET | Get System #4 cooler RPM value |

## Temperature

| Endpoint | Method | Description |
|---|---|---|
| `/temperature?location=<LOCATION>` | GET | `LOCATION`: location of the sensor (`cpu`, `sys1`, `sys2`, `sys3` or `sys4`) |
| `/temperature/map?pos=<POS>&map=<MAP>` | GET | `POS`: sensor position between 1 and 5, identifies the sensor. `MAP`: sensor location (same values as `LOCATION` above) |
