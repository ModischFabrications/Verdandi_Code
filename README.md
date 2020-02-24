# Verdandi
Cooperative project to display an analog clock with digital RGB LEDs.

PCB can be found at https://github.com/ModischFabrications/Verdandi_PCB.

## Usage
Connect the clock to an adequate power supply, 5V and >200mA should be fine. 
We assume that your network allows outgoing connections, a WiFi connection equals 
network connectivity for us.

## First setup
1. Power the clock
2. Check for a wifi network called "Verdandi"
3. Connect and select your network and set your password
4. Repeat 2+ if it does not work
5. If everything worked the hotspot should disappear and the clock should show up

## Find out the IP address
1. Open up the small hatch in the back of the clock
1. Connect the wemos to a PC with an usb cable
1. Open a serial terminal (I know, sorry...)
1. Read the terminal output to find out your IP

## Installation
VSCode should offer you a task to do everything automatically:
1. Close Serial Monitor
2. CTRL+ALT+T -> "deploy project"

Call "/deploy/deploy.sh" manually if that doesn't work or you don't believe in VSCode.

### Uploading the file system
In order to increase transmission speed and reduce memory usage, files are stored in the GNU zip file format on the ESP8266 and unpacked in your browser. 

VSCode Task: "deploy file system" with the respective script "/deploy/deploy_fs.sh". 

## Contributing
### CPP Formatting
We are using the Clang Formatter with the style configuration from the included ´.clang-format´ file. For that install the [Clang compiler](https://github.com/nextcloud/desktop/wiki) as well as the Clang-Format extension for vscode.

## Related projects and links

This is a follow up to a now deprecated previous project of mine: https://github.com/ModischFabrications/RGB_Clock

### RGB clocks
https://www.instructables.com/id/Easy-Infinity-Mirror-Clock/

https://github.com/marijoblaz/iOWatch

https://www.reddit.com/r/functionalprint/comments/e5marv/led_wall_clock/ -> https://github.com/Petrovjan/3D-printed-LED-clock

https://github.com/dheera/shadow-clock

https://github.com/leonvandenbeukel/Round-LED-Clock

https://learn.adafruit.com/flora-geo-watch

### Similar components

https://www.instructables.com/id/Network-Time-Digital-Clock-Using-the-ESP8266/

https://lastminuteengineers.com/esp8266-ntp-server-date-time-tutorial/

