# Analytics Choocher

## Current state

It just shows count of online Steam friends using ESP8266 and voltage needle panel. Codged in one evening, don't expect much.

## Roadmap

* add KiCad schematics
* get rid of hardcoded Steam example, modularize
* low-power sleep

## Prerequisites

* `platformio`

Afterwards you may try to compile and upload via (replace with your wifi ssid/p and Steam key/id): `PLATFORMIO_BUILD_FLAGS="-D'WIFI_PASSWORD=\"psw\"' -D'WIFI_SSID=\"ssid\"' -D'STEAM_KEY=\"A1AAA1AAAAA1A1AAAA111111AAA1A1A1\"' -D'STEAM_USER_ID=\"11111111111111111\"'" platformio run -e esp12e -t upload --verbose`
