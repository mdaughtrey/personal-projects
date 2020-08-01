# The set of languages for which implicit dependencies are needed:
set(CMAKE_DEPENDS_LANGUAGES
  "C"
  )
# The set of files for implicit dependencies of each language:
set(CMAKE_DEPENDS_CHECK_C
  "/tools/ESP8266_RTOS_SDK/components/bootloader_support/src/bootloader_clock.c" "/build/build/esp-idf/bootloader_support/CMakeFiles/__idf_bootloader_support.dir/src/bootloader_clock.c.obj"
  "/tools/ESP8266_RTOS_SDK/components/bootloader_support/src/bootloader_common.c" "/build/build/esp-idf/bootloader_support/CMakeFiles/__idf_bootloader_support.dir/src/bootloader_common.c.obj"
  "/tools/ESP8266_RTOS_SDK/components/bootloader_support/src/bootloader_flash.c" "/build/build/esp-idf/bootloader_support/CMakeFiles/__idf_bootloader_support.dir/src/bootloader_flash.c.obj"
  "/tools/ESP8266_RTOS_SDK/components/bootloader_support/src/bootloader_init.c" "/build/build/esp-idf/bootloader_support/CMakeFiles/__idf_bootloader_support.dir/src/bootloader_init.c.obj"
  "/tools/ESP8266_RTOS_SDK/components/bootloader_support/src/bootloader_random.c" "/build/build/esp-idf/bootloader_support/CMakeFiles/__idf_bootloader_support.dir/src/bootloader_random.c.obj"
  "/tools/ESP8266_RTOS_SDK/components/bootloader_support/src/bootloader_sha.c" "/build/build/esp-idf/bootloader_support/CMakeFiles/__idf_bootloader_support.dir/src/bootloader_sha.c.obj"
  "/tools/ESP8266_RTOS_SDK/components/bootloader_support/src/bootloader_utility.c" "/build/build/esp-idf/bootloader_support/CMakeFiles/__idf_bootloader_support.dir/src/bootloader_utility.c.obj"
  "/tools/ESP8266_RTOS_SDK/components/bootloader_support/src/efuse.c" "/build/build/esp-idf/bootloader_support/CMakeFiles/__idf_bootloader_support.dir/src/efuse.c.obj"
  "/tools/ESP8266_RTOS_SDK/components/bootloader_support/src/esp_image_format.c" "/build/build/esp-idf/bootloader_support/CMakeFiles/__idf_bootloader_support.dir/src/esp_image_format.c.obj"
  "/tools/ESP8266_RTOS_SDK/components/bootloader_support/src/flash_encrypt.c" "/build/build/esp-idf/bootloader_support/CMakeFiles/__idf_bootloader_support.dir/src/flash_encrypt.c.obj"
  "/tools/ESP8266_RTOS_SDK/components/bootloader_support/src/flash_partitions.c" "/build/build/esp-idf/bootloader_support/CMakeFiles/__idf_bootloader_support.dir/src/flash_partitions.c.obj"
  "/tools/ESP8266_RTOS_SDK/components/bootloader_support/src/flash_qio_mode.c" "/build/build/esp-idf/bootloader_support/CMakeFiles/__idf_bootloader_support.dir/src/flash_qio_mode.c.obj"
  "/tools/ESP8266_RTOS_SDK/components/bootloader_support/src/secure_boot.c" "/build/build/esp-idf/bootloader_support/CMakeFiles/__idf_bootloader_support.dir/src/secure_boot.c.obj"
  "/tools/ESP8266_RTOS_SDK/components/bootloader_support/src/secure_boot_signatures.c" "/build/build/esp-idf/bootloader_support/CMakeFiles/__idf_bootloader_support.dir/src/secure_boot_signatures.c.obj"
  )
set(CMAKE_C_COMPILER_ID "GNU")

# Preprocessor definitions for this target.
set(CMAKE_TARGET_DEFINITIONS_C
  "USING_IBUS_FASTER_GET"
  "_CLOCKS_PER_SEC_=CONFIG_FREERTOS_HZ"
  "_POSIX_THREADS=1"
  "_UNIX98_THREAD_MUTEX_ATTRIBUTES=1"
  "__ESP_FILE__=__FILE__"
  )

# The include file search paths:
set(CMAKE_C_TARGET_INCLUDE_PATH
  "config"
  "/tools/ESP8266_RTOS_SDK/components/bootloader_support/include"
  "/tools/ESP8266_RTOS_SDK/components/bootloader_support/include_priv"
  "/tools/ESP8266_RTOS_SDK/components/newlib/newlib/include"
  "/tools/ESP8266_RTOS_SDK/components/newlib/newlib/port/include"
  "/tools/ESP8266_RTOS_SDK/components/freertos/include"
  "/tools/ESP8266_RTOS_SDK/components/freertos/include/freertos"
  "/tools/ESP8266_RTOS_SDK/components/freertos/include/freertos/private"
  "/tools/ESP8266_RTOS_SDK/components/freertos/port/esp8266/include"
  "/tools/ESP8266_RTOS_SDK/components/freertos/port/esp8266/include/freertos"
  "/tools/ESP8266_RTOS_SDK/components/heap/include"
  "/tools/ESP8266_RTOS_SDK/components/heap/port/esp8266/include"
  "/tools/ESP8266_RTOS_SDK/components/log/include"
  "/tools/ESP8266_RTOS_SDK/components/esp8266/include"
  "/tools/ESP8266_RTOS_SDK/components/esp8266/include/driver"
  "/tools/ESP8266_RTOS_SDK/components/spi_flash/include"
  "/tools/ESP8266_RTOS_SDK/components/util/include"
  "/tools/ESP8266_RTOS_SDK/components/ssl/mbedtls/port/openssl/include"
  "/tools/ESP8266_RTOS_SDK/components/ssl/mbedtls/mbedtls/include"
  "/tools/ESP8266_RTOS_SDK/components/ssl/mbedtls/port/esp8266/include"
  "/tools/ESP8266_RTOS_SDK/components/lwip/include/lwip"
  "/tools/ESP8266_RTOS_SDK/components/lwip/include/lwip/apps"
  "/tools/ESP8266_RTOS_SDK/components/lwip/lwip/src/include"
  "/tools/ESP8266_RTOS_SDK/components/lwip/lwip/src/include/posix"
  "/tools/ESP8266_RTOS_SDK/components/lwip/port/esp8266/include"
  "/tools/ESP8266_RTOS_SDK/components/lwip/port/esp8266/include/port"
  "/tools/ESP8266_RTOS_SDK/components/vfs/include"
  )

# Targets to which this target links.
set(CMAKE_TARGET_LINKED_INFO_FILES
  "/build/build/esp-idf/newlib/CMakeFiles/__idf_newlib.dir/DependInfo.cmake"
  "/build/build/esp-idf/freertos/CMakeFiles/__idf_freertos.dir/DependInfo.cmake"
  "/build/build/esp-idf/heap/CMakeFiles/__idf_heap.dir/DependInfo.cmake"
  "/build/build/esp-idf/log/CMakeFiles/__idf_log.dir/DependInfo.cmake"
  "/build/build/esp-idf/esp8266/CMakeFiles/__idf_esp8266.dir/DependInfo.cmake"
  "/build/build/esp-idf/spi_flash/CMakeFiles/__idf_spi_flash.dir/DependInfo.cmake"
  "/build/build/esp-idf/util/CMakeFiles/__idf_util.dir/DependInfo.cmake"
  "/build/build/esp-idf/ssl/CMakeFiles/__idf_ssl.dir/DependInfo.cmake"
  "/build/build/esp-idf/vfs/CMakeFiles/__idf_vfs.dir/DependInfo.cmake"
  "/build/build/esp-idf/lwip/CMakeFiles/__idf_lwip.dir/DependInfo.cmake"
  "/build/build/esp-idf/wpa_supplicant/CMakeFiles/__idf_wpa_supplicant.dir/DependInfo.cmake"
  "/build/build/esp-idf/tcpip_adapter/CMakeFiles/__idf_tcpip_adapter.dir/DependInfo.cmake"
  "/build/build/esp-idf/esp_ringbuf/CMakeFiles/__idf_esp_ringbuf.dir/DependInfo.cmake"
  "/build/build/esp-idf/nvs_flash/CMakeFiles/__idf_nvs_flash.dir/DependInfo.cmake"
  )

# Fortran module output directory.
set(CMAKE_Fortran_TARGET_MODULE_DIR "")
