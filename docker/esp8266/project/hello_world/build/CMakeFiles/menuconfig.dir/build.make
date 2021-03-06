# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /tools/cmake-3.5.2-Linux-x86_64/bin/cmake

# The command to remove a file.
RM = /tools/cmake-3.5.2-Linux-x86_64/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /build

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /build/build

# Utility rule file for menuconfig.

# Include the progress variables for this target.
include CMakeFiles/menuconfig.dir/progress.make

CMakeFiles/menuconfig:
	/usr/bin/python /tools/ESP8266_RTOS_SDK/tools/kconfig_new/confgen.py --kconfig /tools/ESP8266_RTOS_SDK/Kconfig --config /build/sdkconfig --env-file /build/build/config.env --env IDF_TARGET=esp8266 --output config /build/sdkconfig
	/tools/cmake-3.5.2-Linux-x86_64/bin/cmake -E env COMPONENT_KCONFIGS=/tools/ESP8266_RTOS_SDK/components/log/Kconfig\ /tools/ESP8266_RTOS_SDK/components/lwip/Kconfig\ /tools/ESP8266_RTOS_SDK/components/spiffs/Kconfig\ /tools/ESP8266_RTOS_SDK/components/esp_http_server/Kconfig\ /tools/ESP8266_RTOS_SDK/components/vfs/Kconfig\ /tools/ESP8266_RTOS_SDK/components/ssl/Kconfig\ /tools/ESP8266_RTOS_SDK/components/esp_http_client/Kconfig\ /tools/ESP8266_RTOS_SDK/components/tcpip_adapter/Kconfig\ /tools/ESP8266_RTOS_SDK/components/app_update/Kconfig\ /tools/ESP8266_RTOS_SDK/components/console/Kconfig\ /tools/ESP8266_RTOS_SDK/components/esp8266/Kconfig\ /tools/ESP8266_RTOS_SDK/components/util/Kconfig\ /tools/ESP8266_RTOS_SDK/components/pthread/Kconfig\ /tools/ESP8266_RTOS_SDK/components/freertos/Kconfig\ /tools/ESP8266_RTOS_SDK/components/libsodium/Kconfig\ /tools/ESP8266_RTOS_SDK/components/mqtt/Kconfig\ /tools/ESP8266_RTOS_SDK/components/wpa_supplicant/Kconfig\ /tools/ESP8266_RTOS_SDK/components/mdns/Kconfig\ /tools/ESP8266_RTOS_SDK/components/wifi_provisioning/Kconfig\ /tools/ESP8266_RTOS_SDK/components/newlib/Kconfig\ /tools/ESP8266_RTOS_SDK/components/aws_iot/Kconfig COMPONENT_KCONFIGS_PROJBUILD=/tools/ESP8266_RTOS_SDK/components/esptool_py/Kconfig.projbuild\ /tools/ESP8266_RTOS_SDK/components/bootloader/Kconfig.projbuild\ /tools/ESP8266_RTOS_SDK/components/partition_table/Kconfig.projbuild IDF_CMAKE=y KCONFIG_CONFIG=/build/sdkconfig IDF_TARGET=esp8266 /build/build/kconfig_bin/mconf-idf /tools/ESP8266_RTOS_SDK/Kconfig
	/usr/bin/python /tools/ESP8266_RTOS_SDK/tools/kconfig_new/confgen.py --kconfig /tools/ESP8266_RTOS_SDK/Kconfig --config /build/sdkconfig --env-file /build/build/config.env --env IDF_TARGET=esp8266 --output config /build/sdkconfig

menuconfig: CMakeFiles/menuconfig
menuconfig: CMakeFiles/menuconfig.dir/build.make

.PHONY : menuconfig

# Rule to build all files generated by this target.
CMakeFiles/menuconfig.dir/build: menuconfig

.PHONY : CMakeFiles/menuconfig.dir/build

CMakeFiles/menuconfig.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/menuconfig.dir/cmake_clean.cmake
.PHONY : CMakeFiles/menuconfig.dir/clean

CMakeFiles/menuconfig.dir/depend:
	cd /build/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /build /build /build/build /build/build /build/build/CMakeFiles/menuconfig.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/menuconfig.dir/depend

