# The set of languages for which implicit dependencies are needed:
set(CMAKE_DEPENDS_LANGUAGES
  "C"
  )
# The set of files for implicit dependencies of each language:
set(CMAKE_DEPENDS_CHECK_C
  "/tools/ESP8266_RTOS_SDK/components/coap/libcoap/src/address.c" "/build/build/esp-idf/coap/CMakeFiles/__idf_coap.dir/libcoap/src/address.c.obj"
  "/tools/ESP8266_RTOS_SDK/components/coap/libcoap/src/async.c" "/build/build/esp-idf/coap/CMakeFiles/__idf_coap.dir/libcoap/src/async.c.obj"
  "/tools/ESP8266_RTOS_SDK/components/coap/libcoap/src/block.c" "/build/build/esp-idf/coap/CMakeFiles/__idf_coap.dir/libcoap/src/block.c.obj"
  "/tools/ESP8266_RTOS_SDK/components/coap/libcoap/src/coap_time.c" "/build/build/esp-idf/coap/CMakeFiles/__idf_coap.dir/libcoap/src/coap_time.c.obj"
  "/tools/ESP8266_RTOS_SDK/components/coap/libcoap/src/debug.c" "/build/build/esp-idf/coap/CMakeFiles/__idf_coap.dir/libcoap/src/debug.c.obj"
  "/tools/ESP8266_RTOS_SDK/components/coap/libcoap/src/encode.c" "/build/build/esp-idf/coap/CMakeFiles/__idf_coap.dir/libcoap/src/encode.c.obj"
  "/tools/ESP8266_RTOS_SDK/components/coap/libcoap/src/hashkey.c" "/build/build/esp-idf/coap/CMakeFiles/__idf_coap.dir/libcoap/src/hashkey.c.obj"
  "/tools/ESP8266_RTOS_SDK/components/coap/libcoap/src/mem.c" "/build/build/esp-idf/coap/CMakeFiles/__idf_coap.dir/libcoap/src/mem.c.obj"
  "/tools/ESP8266_RTOS_SDK/components/coap/libcoap/src/net.c" "/build/build/esp-idf/coap/CMakeFiles/__idf_coap.dir/libcoap/src/net.c.obj"
  "/tools/ESP8266_RTOS_SDK/components/coap/libcoap/src/option.c" "/build/build/esp-idf/coap/CMakeFiles/__idf_coap.dir/libcoap/src/option.c.obj"
  "/tools/ESP8266_RTOS_SDK/components/coap/libcoap/src/pdu.c" "/build/build/esp-idf/coap/CMakeFiles/__idf_coap.dir/libcoap/src/pdu.c.obj"
  "/tools/ESP8266_RTOS_SDK/components/coap/libcoap/src/resource.c" "/build/build/esp-idf/coap/CMakeFiles/__idf_coap.dir/libcoap/src/resource.c.obj"
  "/tools/ESP8266_RTOS_SDK/components/coap/libcoap/src/str.c" "/build/build/esp-idf/coap/CMakeFiles/__idf_coap.dir/libcoap/src/str.c.obj"
  "/tools/ESP8266_RTOS_SDK/components/coap/libcoap/src/subscribe.c" "/build/build/esp-idf/coap/CMakeFiles/__idf_coap.dir/libcoap/src/subscribe.c.obj"
  "/tools/ESP8266_RTOS_SDK/components/coap/libcoap/src/uri.c" "/build/build/esp-idf/coap/CMakeFiles/__idf_coap.dir/libcoap/src/uri.c.obj"
  "/tools/ESP8266_RTOS_SDK/components/coap/port/coap_io_socket.c" "/build/build/esp-idf/coap/CMakeFiles/__idf_coap.dir/port/coap_io_socket.c.obj"
  )
set(CMAKE_C_COMPILER_ID "GNU")

# Preprocessor definitions for this target.
set(CMAKE_TARGET_DEFINITIONS_C
  "WITH_POSIX"
  "_CLOCKS_PER_SEC_=CONFIG_FREERTOS_HZ"
  "_POSIX_THREADS=1"
  "_UNIX98_THREAD_MUTEX_ATTRIBUTES=1"
  "__ESP_FILE__=__FILE__"
  )

# The include file search paths:
set(CMAKE_C_TARGET_INCLUDE_PATH
  "config"
  "/tools/ESP8266_RTOS_SDK/components/coap/port/include"
  "/tools/ESP8266_RTOS_SDK/components/coap/port/include/coap"
  "/tools/ESP8266_RTOS_SDK/components/coap/libcoap/include"
  "/tools/ESP8266_RTOS_SDK/components/coap/libcoap/include/coap"
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
  "/build/build/esp-idf/lwip/CMakeFiles/__idf_lwip.dir/DependInfo.cmake"
  "/build/build/esp-idf/vfs/CMakeFiles/__idf_vfs.dir/DependInfo.cmake"
  "/build/build/esp-idf/wpa_supplicant/CMakeFiles/__idf_wpa_supplicant.dir/DependInfo.cmake"
  "/build/build/esp-idf/spi_flash/CMakeFiles/__idf_spi_flash.dir/DependInfo.cmake"
  "/build/build/esp-idf/tcpip_adapter/CMakeFiles/__idf_tcpip_adapter.dir/DependInfo.cmake"
  "/build/build/esp-idf/esp_ringbuf/CMakeFiles/__idf_esp_ringbuf.dir/DependInfo.cmake"
  "/build/build/esp-idf/bootloader_support/CMakeFiles/__idf_bootloader_support.dir/DependInfo.cmake"
  "/build/build/esp-idf/nvs_flash/CMakeFiles/__idf_nvs_flash.dir/DependInfo.cmake"
  "/build/build/esp-idf/util/CMakeFiles/__idf_util.dir/DependInfo.cmake"
  "/build/build/esp-idf/ssl/CMakeFiles/__idf_ssl.dir/DependInfo.cmake"
  )

# Fortran module output directory.
set(CMAKE_Fortran_TARGET_MODULE_DIR "")
