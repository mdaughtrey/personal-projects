/*
 * Automatically generated file. DO NOT EDIT.
 * Espressif IoT Development Framework (ESP-IDF) Configuration Header
 */
#pragma once
#define CONFIG_IDF_TARGET_ESP8266 1
#define CONFIG_IDF_CMAKE 1
#define CONFIG_IDF_TARGET "esp8266"
#define CONFIG_SDK_TOOLPREFIX "xtensa-esp32-elf-"
#define CONFIG_ESPTOOLPY_PORT "/dev/ttyUSB0"
#define CONFIG_ESPTOOLPY_BAUD_115200B 1
#define CONFIG_ESPTOOLPY_BAUD_OTHER_VAL 115200
#define CONFIG_ESPTOOLPY_BAUD 115200
#define CONFIG_ESPTOOLPY_COMPRESSED 1
#define CONFIG_ESPTOOLPY_FLASHMODE_DIO 1
#define CONFIG_ESPTOOLPY_FLASHMODE "dio"
#define CONFIG_SPI_FLASH_MODE 0x2
#define CONFIG_ESPTOOLPY_FLASHFREQ_40M 1
#define CONFIG_ESPTOOLPY_FLASHFREQ "40m"
#define CONFIG_SPI_FLASH_FREQ 0x0
#define CONFIG_ESPTOOLPY_FLASHSIZE_4MB 1
#define CONFIG_ESPTOOLPY_FLASHSIZE "4MB"
#define CONFIG_SPI_FLASH_SIZE 0x400000
#define CONFIG_ESPTOOLPY_BEFORE_RESET 1
#define CONFIG_ESPTOOLPY_BEFORE "default_reset"
#define CONFIG_ESPTOOLPY_AFTER_HARD_RESET 1
#define CONFIG_ESPTOOLPY_AFTER "hard_reset"
#define CONFIG_ESPTOOLPY_MONITOR_BAUD_115200B 1
#define CONFIG_ESPTOOLPY_MONITOR_BAUD_OTHER_VAL 74880
#define CONFIG_ESPTOOLPY_MONITOR_BAUD 115200
#define CONFIG_BOOTLOADER_INIT_SPI_FLASH 1
#define CONFIG_LOG_BOOTLOADER_LEVEL_INFO 1
#define CONFIG_LOG_BOOTLOADER_LEVEL 3
#define CONFIG_BOOTLOADER_CHECK_APP_SUM 1
#define CONFIG_PARTITION_TABLE_SINGLE_APP 1
#define CONFIG_PARTITION_TABLE_CUSTOM_FILENAME "partitions.csv"
#define CONFIG_PARTITION_TABLE_OFFSET 0x8000
#define CONFIG_PARTITION_TABLE_FILENAME "partitions_singleapp.csv"
#define CONFIG_COMPILER_OPTIMIZATION_LEVEL_DEBUG 1
#define CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_ENABLE 1
#define CONFIG_COMPILER_STACK_CHECK_MODE_NONE 1
#define CONFIG_LOG_DEFAULT_LEVEL_INFO 1
#define CONFIG_LOG_DEFAULT_LEVEL 3
#define CONFIG_LOG_COLORS 1
#define CONFIG_ESP8266_DEFAULT_CPU_FREQ_80 1
#define CONFIG_ESP8266_DEFAULT_CPU_FREQ_MHZ 80
#define CONFIG_NEWLIB_STDOUT_LINE_ENDING_CRLF 1
#define CONFIG_ESP_FILENAME_MACRO_NO_PATH 1
#define CONFIG_USING_NEW_ETS_VPRINTF 1
#define CONFIG_SOC_IRAM_SIZE 0xC000
#define CONFIG_CONSOLE_UART_DEFAULT 1
#define CONFIG_CONSOLE_UART_NUM 0
#define CONFIG_CONSOLE_UART_BAUDRATE 115200
#define CONFIG_ESP_PANIC_PRINT_REBOOT 1
#define CONFIG_MAIN_TASK_STACK_SIZE 3584
#define CONFIG_TASK_WDT 1
#define CONFIG_TASK_WDT_TIMEOUT_15N 1
#define CONFIG_TASK_WDT_TIMEOUT_S 15
#define CONFIG_RESET_REASON 1
#define CONFIG_WIFI_PPT_TASKSTACK_SIZE 2048
#define CONFIG_EVENT_LOOP_STACK_SIZE 2048
#define CONFIG_ESP8266_CORE_GLOBAL_DATA_LINK_IRAM 1
#define CONFIG_CRYSTAL_USED_26MHZ 1
#define CONFIG_ESP_ERR_TO_NAME_LOOKUP 1
#define CONFIG_SCAN_AP_MAX 32
#define CONFIG_WIFI_TX_RATE_SEQUENCE_FROM_HIGH 1
#define CONFIG_ESP_PHY_CALIBRATION_AND_DATA_STORAGE 1
#define CONFIG_ESP_PHY_INIT_DATA_VDD33_CONST 33
#define CONFIG_ESP8266_PHY_MAX_WIFI_TX_POWER 20
#define CONFIG_ESP_SHA 1
#define CONFIG_ESP_AES 1
#define CONFIG_ESP_MD5 1
#define CONFIG_ESP_ARC4 1

/* List of deprecated options */
#ifdef CONFIG_COMPILER_CXX_EXCEPTIONS
#define CONFIG_CXX_EXCEPTIONS CONFIG_COMPILER_CXX_EXCEPTIONS
#endif

#ifdef CONFIG_ESPTOOLPY_FLASHMODE_DIO
#define CONFIG_FLASHMODE_DIO CONFIG_ESPTOOLPY_FLASHMODE_DIO
#endif

#ifdef CONFIG_ESPTOOLPY_FLASHMODE_DOUT
#define CONFIG_FLASHMODE_DOUT CONFIG_ESPTOOLPY_FLASHMODE_DOUT
#endif

#ifdef CONFIG_ESPTOOLPY_FLASHMODE_QIO
#define CONFIG_FLASHMODE_QIO CONFIG_ESPTOOLPY_FLASHMODE_QIO
#endif

#ifdef CONFIG_ESPTOOLPY_FLASHMODE_QOUT
#define CONFIG_FLASHMODE_QOUT CONFIG_ESPTOOLPY_FLASHMODE_QOUT
#endif

#ifdef CONFIG_SDK_MAKE_WARN_UNDEFINED_VARIABLES
#define CONFIG_MAKE_WARN_UNDEFINED_VARIABLES CONFIG_SDK_MAKE_WARN_UNDEFINED_VARIABLES
#endif

#ifdef CONFIG_ESPTOOLPY_MONITOR_BAUD
#define CONFIG_MONITOR_BAUD CONFIG_ESPTOOLPY_MONITOR_BAUD
#endif

#ifdef CONFIG_ESPTOOLPY_MONITOR_BAUD_115200B
#define CONFIG_MONITOR_BAUD_115200B CONFIG_ESPTOOLPY_MONITOR_BAUD_115200B
#endif

#ifdef CONFIG_ESPTOOLPY_MONITOR_BAUD_230400B
#define CONFIG_MONITOR_BAUD_230400B CONFIG_ESPTOOLPY_MONITOR_BAUD_230400B
#endif

#ifdef CONFIG_ESPTOOLPY_MONITOR_BAUD_2MB
#define CONFIG_MONITOR_BAUD_2MB CONFIG_ESPTOOLPY_MONITOR_BAUD_2MB
#endif

#ifdef CONFIG_ESPTOOLPY_MONITOR_BAUD_57600B
#define CONFIG_MONITOR_BAUD_57600B CONFIG_ESPTOOLPY_MONITOR_BAUD_57600B
#endif

#ifdef CONFIG_ESPTOOLPY_MONITOR_BAUD_74880B
#define CONFIG_MONITOR_BAUD_74880B CONFIG_ESPTOOLPY_MONITOR_BAUD_74880B
#endif

#ifdef CONFIG_ESPTOOLPY_MONITOR_BAUD_921600B
#define CONFIG_MONITOR_BAUD_921600B CONFIG_ESPTOOLPY_MONITOR_BAUD_921600B
#endif

#ifdef CONFIG_ESPTOOLPY_MONITOR_BAUD_9600B
#define CONFIG_MONITOR_BAUD_9600B CONFIG_ESPTOOLPY_MONITOR_BAUD_9600B
#endif

#ifdef CONFIG_ESPTOOLPY_MONITOR_BAUD_OTHER
#define CONFIG_MONITOR_BAUD_OTHER CONFIG_ESPTOOLPY_MONITOR_BAUD_OTHER
#endif

#ifdef CONFIG_ESPTOOLPY_MONITOR_BAUD_OTHER_VAL
#define CONFIG_MONITOR_BAUD_OTHER_VAL CONFIG_ESPTOOLPY_MONITOR_BAUD_OTHER_VAL
#endif

#ifdef CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_DISABLE
#define CONFIG_OPTIMIZATION_ASSERTIONS_DISABLED CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_DISABLE
#endif

#ifdef CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_ENABLE
#define CONFIG_OPTIMIZATION_ASSERTIONS_ENABLED CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_ENABLE
#endif

#ifdef CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_SILENT
#define CONFIG_OPTIMIZATION_ASSERTIONS_SILENT CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_SILENT
#endif

#ifdef CONFIG_COMPILER_OPTIMIZATION_ASSERTION_LEVEL
#define CONFIG_OPTIMIZATION_ASSERTION_LEVEL CONFIG_COMPILER_OPTIMIZATION_ASSERTION_LEVEL
#endif

#ifdef CONFIG_COMPILER_OPTIMIZATION
#define CONFIG_OPTIMIZATION_COMPILER CONFIG_COMPILER_OPTIMIZATION
#endif

#ifdef CONFIG_COMPILER_OPTIMIZATION_LEVEL_DEBUG
#define CONFIG_OPTIMIZATION_LEVEL_DEBUG CONFIG_COMPILER_OPTIMIZATION_LEVEL_DEBUG
#endif

#ifdef CONFIG_COMPILER_OPTIMIZATION_LEVEL_RELEASE
#define CONFIG_OPTIMIZATION_LEVEL_RELEASE CONFIG_COMPILER_OPTIMIZATION_LEVEL_RELEASE
#endif

#ifdef CONFIG_COMPILER_STACK_CHECK
#define CONFIG_STACK_CHECK CONFIG_COMPILER_STACK_CHECK
#endif

#ifdef CONFIG_COMPILER_STACK_CHECK_MODE_ALL
#define CONFIG_STACK_CHECK_ALL CONFIG_COMPILER_STACK_CHECK_MODE_ALL
#endif

#ifdef CONFIG_COMPILER_STACK_CHECK_MODE
#define CONFIG_STACK_CHECK_MODE CONFIG_COMPILER_STACK_CHECK_MODE
#endif

#ifdef CONFIG_COMPILER_STACK_CHECK_MODE_NONE
#define CONFIG_STACK_CHECK_NONE CONFIG_COMPILER_STACK_CHECK_MODE_NONE
#endif

#ifdef CONFIG_COMPILER_STACK_CHECK_MODE_NORM
#define CONFIG_STACK_CHECK_NORM CONFIG_COMPILER_STACK_CHECK_MODE_NORM
#endif

#ifdef CONFIG_COMPILER_STACK_CHECK_MODE_STRONG
#define CONFIG_STACK_CHECK_STRONG CONFIG_COMPILER_STACK_CHECK_MODE_STRONG
#endif

#ifdef CONFIG_IDF_TARGET
#define CONFIG_TARGET_PLATFORM CONFIG_IDF_TARGET
#endif

#ifdef CONFIG_SDK_TOOLPREFIX
#define CONFIG_TOOLPREFIX CONFIG_SDK_TOOLPREFIX
#endif

#ifdef CONFIG_COMPILER_WARN_WRITE_STRINGS
#define CONFIG_WARN_WRITE_STRINGS CONFIG_COMPILER_WARN_WRITE_STRINGS
#endif

