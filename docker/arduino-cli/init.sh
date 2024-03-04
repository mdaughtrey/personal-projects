mkdir ~/acli
mkdir ~/tmp
cd ~/acli && curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
PATH=~/acli/bin:${PATH}
export PATH=~/acli/bin:${PATH}
mkdir -p ~/acli/Arduino/libraries
cd ~/acli && wget https://github.com/ZinggJM/GxEPD/archive/master.zip && unzip master.zip -d ~/acli/Arduino/libraries && rm master.zip
~/acli/bin/arduino-cli config init
~/acli/bin/arduino-cli lib install 'Adafruit GFX Library'
~/acli/bin/arduino-cli lib install 'Adafruit APDS9960 Library'
~/acli/bin/arduino-cli lib install gxepd2
~/acli/bin/arduino-cli lib install Adafruit_VL53L0X
~/acli/bin/arduino-cli lib install AsyncStepperLib
yq -yi '.board_manager.additional_urls=["https://arduino.esp8266.com/stable/package_esp8266com_index.json","https://dl.espressif.com/dl/package_esp32_index.json","https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json"]' ~/.arduino15/arduino-cli.yaml
~/acli/bin/arduino-cli core update-index
~/acli/bin/arduino-cli core install esp8266:esp8266
~/acli/bin/arduino-cli core install esp32:esp32
~/acli/bin/arduino-cli core install rp2040:rp2040
cd ~/Arduino/libraries && git clone https://github.com/maniacbug/StandardCplusplus.git
