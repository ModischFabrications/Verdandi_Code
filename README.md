# Verdandi
Cooperative project to display an analog clock with digital RGB LEDs.

Links can be found at https://github.com/ModischFabrications/Verdandi.

## Installation
VSCode should offer you a task to do everything automatically:
1. Close Serial Monitor
2. CTRL+ALT+T -> "deploy project"

Call "/deploy/deploy.sh" manually if that doesn't work or you don't believe in VSCode.
It still depends on platformIO, be aware that you might have to install the toolchain manually without VSCode.

### Uploading the file system
*This is included in the previous step, use this only if you know why you want it.*
In order to increase transmission speed and reduce memory usage, files are stored in the GNU zip file format on the ESP8266 and unpacked in your browser. 

VSCode Task: "deploy file system" with the respective script "/deploy/deploy_fs.sh". 

## Contributing
### CPP Formatting
We are using the Clang Formatter with the style configuration from the included ´.clang-format´ file. For that install the [Clang compiler](https://github.com/nextcloud/desktop/wiki) as well as the Clang-Format extension for vscode.

