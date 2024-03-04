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
cp arduino-cli.patch0 /tmp/arduino-cli.patch0
~/acli/bin/arduino-cli core update-index
~/acli/bin/arduino-cli core install esp8266:esp8266
~/acli/bin/arduino-cli core install esp32:esp32
~/acli/bin/arduino-cli core install rp2040:rp2040
cd ~/Arduino/libraries && git clone https://github.com/maniacbug/StandardCplusplus.git
