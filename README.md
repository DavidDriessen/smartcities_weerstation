
## Smartsities Weather Station

This weather station is compatible with an ESP32 and ESP8266.

#### How to flash

To flash the weather station you need to install platformio, 
follow the guide [here](http://docs.platformio.org/en/latest/installation.html).

After installing platformio you have to initiate the project with `pio init`
and install some dependencies:

Id  | Name              | Platform
----|-------------------|------------
64  | ArduinoJson       | ESP32 and ESP8266
562 | ESP8266_SSD1306   | ESP32 and ESP8266
2007| ESP32WebServer    | ESP32

To install the dependencies run: `pio lib install <dependencie>`

After installing all the dependencies you can flash th ESP with 
the following command `pio run`.
