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

# Utility rule file for __ldgen_output_esp8266.project.ld.

# Include the progress variables for this target.
include esp-idf/esp8266/CMakeFiles/__ldgen_output_esp8266.project.ld.dir/progress.make

esp-idf/esp8266/CMakeFiles/__ldgen_output_esp8266.project.ld: esp-idf/esp8266/ld/esp8266.project.ld


esp-idf/esp8266/ld/esp8266.project.ld: /tools/ESP8266_RTOS_SDK/components/esp8266/ld/esp8266.project.ld.in
esp-idf/esp8266/ld/esp8266.project.ld: /tools/ESP8266_RTOS_SDK/components/spi_flash/linker.lf
esp-idf/esp8266/ld/esp8266.project.ld: /tools/ESP8266_RTOS_SDK/components/esp8266/linker.lf
esp-idf/esp8266/ld/esp8266.project.ld: /tools/ESP8266_RTOS_SDK/components/esp8266/ld/esp8266_fragments.lf
esp-idf/esp8266/ld/esp8266.project.ld: /tools/ESP8266_RTOS_SDK/components/log/linker.lf
esp-idf/esp8266/ld/esp8266.project.ld: esp-idf/util/libutil.a
esp-idf/esp8266/ld/esp8266.project.ld: esp-idf/wpa_supplicant/libwpa_supplicant.a
esp-idf/esp8266/ld/esp8266.project.ld: esp-idf/vfs/libvfs.a
esp-idf/esp8266/ld/esp8266.project.ld: esp-idf/tcpip_adapter/libtcpip_adapter.a
esp-idf/esp8266/ld/esp8266.project.ld: esp-idf/lwip/liblwip.a
esp-idf/esp8266/ld/esp8266.project.ld: esp-idf/ssl/libssl.a
esp-idf/esp8266/ld/esp8266.project.ld: esp-idf/bootloader_support/libbootloader_support.a
esp-idf/esp8266/ld/esp8266.project.ld: esp-idf/spi_flash/libspi_flash.a
esp-idf/esp8266/ld/esp8266.project.ld: esp-idf/esp_ringbuf/libesp_ringbuf.a
esp-idf/esp8266/ld/esp8266.project.ld: esp-idf/nvs_flash/libnvs_flash.a
esp-idf/esp8266/ld/esp8266.project.ld: esp-idf/esp8266/libesp8266.a
esp-idf/esp8266/ld/esp8266.project.ld: esp-idf/heap/libheap.a
esp-idf/esp8266/ld/esp8266.project.ld: esp-idf/freertos/libfreertos.a
esp-idf/esp8266/ld/esp8266.project.ld: esp-idf/newlib/libnewlib.a
esp-idf/esp8266/ld/esp8266.project.ld: esp-idf/log/liblog.a
esp-idf/esp8266/ld/esp8266.project.ld: esp-idf/protobuf-c/libprotobuf-c.a
esp-idf/esp8266/ld/esp8266.project.ld: esp-idf/http_parser/libhttp_parser.a
esp-idf/esp8266/ld/esp8266.project.ld: esp-idf/esp_http_server/libesp_http_server.a
esp-idf/esp8266/ld/esp8266.project.ld: esp-idf/esp-tls/libesp-tls.a
esp-idf/esp8266/ld/esp8266.project.ld: esp-idf/tcp_transport/libtcp_transport.a
esp-idf/esp8266/ld/esp8266.project.ld: esp-idf/esp_http_client/libesp_http_client.a
esp-idf/esp8266/ld/esp8266.project.ld: esp-idf/app_update/libapp_update.a
esp-idf/esp8266/ld/esp8266.project.ld: esp-idf/esp_https_ota/libesp_https_ota.a
esp-idf/esp8266/ld/esp8266.project.ld: esp-idf/smartconfig_ack/libsmartconfig_ack.a
esp-idf/esp8266/ld/esp8266.project.ld: esp-idf/esp_common/libesp_common.a
esp-idf/esp8266/ld/esp8266.project.ld: esp-idf/coap/libcoap.a
esp-idf/esp8266/ld/esp8266.project.ld: esp-idf/jsmn/libjsmn.a
esp-idf/esp8266/ld/esp8266.project.ld: esp-idf/spi_ram/libspi_ram.a
esp-idf/esp8266/ld/esp8266.project.ld: esp-idf/libsodium/liblibsodium.a
esp-idf/esp8266/ld/esp8266.project.ld: esp-idf/mqtt/libmqtt.a
esp-idf/esp8266/ld/esp8266.project.ld: esp-idf/wifi_provisioning/libwifi_provisioning.a
esp-idf/esp8266/ld/esp8266.project.ld: esp-idf/cjson/libcjson.a
esp-idf/esp8266/ld/esp8266.project.ld: esp-idf/main/libmain.a
esp-idf/esp8266/ld/esp8266.project.ld: esp-idf/components/libcomponents.a
esp-idf/esp8266/ld/esp8266.project.ld: /build/sdkconfig
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating ld/esp8266.project.ld"
	cd /build/build/esp-idf/esp8266 && /usr/bin/python /tools/ESP8266_RTOS_SDK/tools/ldgen/ldgen.py --config /build/sdkconfig --fragments /tools/ESP8266_RTOS_SDK/components/spi_flash/linker.lf	/tools/ESP8266_RTOS_SDK/components/esp8266/linker.lf	/tools/ESP8266_RTOS_SDK/components/esp8266/ld/esp8266_fragments.lf	/tools/ESP8266_RTOS_SDK/components/log/linker.lf --input /tools/ESP8266_RTOS_SDK/components/esp8266/ld/esp8266.project.ld.in --output /build/build/esp-idf/esp8266/ld/esp8266.project.ld --kconfig /tools/ESP8266_RTOS_SDK/Kconfig --env-file /build/build/config.env --libraries-file /build/build/ldgen_libraries --objdump /tools/toolchain/bin/xtensa-lx106-elf-objdump

__ldgen_output_esp8266.project.ld: esp-idf/esp8266/CMakeFiles/__ldgen_output_esp8266.project.ld
__ldgen_output_esp8266.project.ld: esp-idf/esp8266/ld/esp8266.project.ld
__ldgen_output_esp8266.project.ld: esp-idf/esp8266/CMakeFiles/__ldgen_output_esp8266.project.ld.dir/build.make

.PHONY : __ldgen_output_esp8266.project.ld

# Rule to build all files generated by this target.
esp-idf/esp8266/CMakeFiles/__ldgen_output_esp8266.project.ld.dir/build: __ldgen_output_esp8266.project.ld

.PHONY : esp-idf/esp8266/CMakeFiles/__ldgen_output_esp8266.project.ld.dir/build

esp-idf/esp8266/CMakeFiles/__ldgen_output_esp8266.project.ld.dir/clean:
	cd /build/build/esp-idf/esp8266 && $(CMAKE_COMMAND) -P CMakeFiles/__ldgen_output_esp8266.project.ld.dir/cmake_clean.cmake
.PHONY : esp-idf/esp8266/CMakeFiles/__ldgen_output_esp8266.project.ld.dir/clean

esp-idf/esp8266/CMakeFiles/__ldgen_output_esp8266.project.ld.dir/depend:
	cd /build/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /build /tools/ESP8266_RTOS_SDK/components/esp8266 /build/build /build/build/esp-idf/esp8266 /build/build/esp-idf/esp8266/CMakeFiles/__ldgen_output_esp8266.project.ld.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : esp-idf/esp8266/CMakeFiles/__ldgen_output_esp8266.project.ld.dir/depend
