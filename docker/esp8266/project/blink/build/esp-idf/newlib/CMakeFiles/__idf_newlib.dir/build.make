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

# Include any dependencies generated for this target.
include esp-idf/newlib/CMakeFiles/__idf_newlib.dir/depend.make

# Include the progress variables for this target.
include esp-idf/newlib/CMakeFiles/__idf_newlib.dir/progress.make

# Include the compile flags for this target's objects.
include esp-idf/newlib/CMakeFiles/__idf_newlib.dir/flags.make

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/time.c.obj: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/flags.make
esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/time.c.obj: /tools/ESP8266_RTOS_SDK/components/newlib/newlib/port/time.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/time.c.obj"
	cd /build/build/esp-idf/newlib && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_newlib.dir/newlib/port/time.c.obj   -c /tools/ESP8266_RTOS_SDK/components/newlib/newlib/port/time.c

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/time.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_newlib.dir/newlib/port/time.c.i"
	cd /build/build/esp-idf/newlib && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tools/ESP8266_RTOS_SDK/components/newlib/newlib/port/time.c > CMakeFiles/__idf_newlib.dir/newlib/port/time.c.i

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/time.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_newlib.dir/newlib/port/time.c.s"
	cd /build/build/esp-idf/newlib && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tools/ESP8266_RTOS_SDK/components/newlib/newlib/port/time.c -o CMakeFiles/__idf_newlib.dir/newlib/port/time.c.s

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/time.c.obj.requires:

.PHONY : esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/time.c.obj.requires

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/time.c.obj.provides: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/time.c.obj.requires
	$(MAKE) -f esp-idf/newlib/CMakeFiles/__idf_newlib.dir/build.make esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/time.c.obj.provides.build
.PHONY : esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/time.c.obj.provides

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/time.c.obj.provides.build: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/time.c.obj


esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/select.c.obj: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/flags.make
esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/select.c.obj: /tools/ESP8266_RTOS_SDK/components/newlib/newlib/port/select.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/select.c.obj"
	cd /build/build/esp-idf/newlib && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_newlib.dir/newlib/port/select.c.obj   -c /tools/ESP8266_RTOS_SDK/components/newlib/newlib/port/select.c

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/select.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_newlib.dir/newlib/port/select.c.i"
	cd /build/build/esp-idf/newlib && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tools/ESP8266_RTOS_SDK/components/newlib/newlib/port/select.c > CMakeFiles/__idf_newlib.dir/newlib/port/select.c.i

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/select.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_newlib.dir/newlib/port/select.c.s"
	cd /build/build/esp-idf/newlib && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tools/ESP8266_RTOS_SDK/components/newlib/newlib/port/select.c -o CMakeFiles/__idf_newlib.dir/newlib/port/select.c.s

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/select.c.obj.requires:

.PHONY : esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/select.c.obj.requires

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/select.c.obj.provides: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/select.c.obj.requires
	$(MAKE) -f esp-idf/newlib/CMakeFiles/__idf_newlib.dir/build.make esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/select.c.obj.provides.build
.PHONY : esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/select.c.obj.provides

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/select.c.obj.provides.build: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/select.c.obj


esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/termios.c.obj: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/flags.make
esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/termios.c.obj: /tools/ESP8266_RTOS_SDK/components/newlib/newlib/port/termios.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/termios.c.obj"
	cd /build/build/esp-idf/newlib && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_newlib.dir/newlib/port/termios.c.obj   -c /tools/ESP8266_RTOS_SDK/components/newlib/newlib/port/termios.c

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/termios.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_newlib.dir/newlib/port/termios.c.i"
	cd /build/build/esp-idf/newlib && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tools/ESP8266_RTOS_SDK/components/newlib/newlib/port/termios.c > CMakeFiles/__idf_newlib.dir/newlib/port/termios.c.i

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/termios.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_newlib.dir/newlib/port/termios.c.s"
	cd /build/build/esp-idf/newlib && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tools/ESP8266_RTOS_SDK/components/newlib/newlib/port/termios.c -o CMakeFiles/__idf_newlib.dir/newlib/port/termios.c.s

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/termios.c.obj.requires:

.PHONY : esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/termios.c.obj.requires

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/termios.c.obj.provides: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/termios.c.obj.requires
	$(MAKE) -f esp-idf/newlib/CMakeFiles/__idf_newlib.dir/build.make esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/termios.c.obj.provides.build
.PHONY : esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/termios.c.obj.provides

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/termios.c.obj.provides.build: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/termios.c.obj


esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/locks.c.obj: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/flags.make
esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/locks.c.obj: /tools/ESP8266_RTOS_SDK/components/newlib/newlib/port/locks.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/locks.c.obj"
	cd /build/build/esp-idf/newlib && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_newlib.dir/newlib/port/locks.c.obj   -c /tools/ESP8266_RTOS_SDK/components/newlib/newlib/port/locks.c

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/locks.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_newlib.dir/newlib/port/locks.c.i"
	cd /build/build/esp-idf/newlib && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tools/ESP8266_RTOS_SDK/components/newlib/newlib/port/locks.c > CMakeFiles/__idf_newlib.dir/newlib/port/locks.c.i

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/locks.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_newlib.dir/newlib/port/locks.c.s"
	cd /build/build/esp-idf/newlib && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tools/ESP8266_RTOS_SDK/components/newlib/newlib/port/locks.c -o CMakeFiles/__idf_newlib.dir/newlib/port/locks.c.s

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/locks.c.obj.requires:

.PHONY : esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/locks.c.obj.requires

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/locks.c.obj.provides: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/locks.c.obj.requires
	$(MAKE) -f esp-idf/newlib/CMakeFiles/__idf_newlib.dir/build.make esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/locks.c.obj.provides.build
.PHONY : esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/locks.c.obj.provides

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/locks.c.obj.provides.build: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/locks.c.obj


esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/esp_newlib.c.obj: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/flags.make
esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/esp_newlib.c.obj: /tools/ESP8266_RTOS_SDK/components/newlib/newlib/port/esp_newlib.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/esp_newlib.c.obj"
	cd /build/build/esp-idf/newlib && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_newlib.dir/newlib/port/esp_newlib.c.obj   -c /tools/ESP8266_RTOS_SDK/components/newlib/newlib/port/esp_newlib.c

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/esp_newlib.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_newlib.dir/newlib/port/esp_newlib.c.i"
	cd /build/build/esp-idf/newlib && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tools/ESP8266_RTOS_SDK/components/newlib/newlib/port/esp_newlib.c > CMakeFiles/__idf_newlib.dir/newlib/port/esp_newlib.c.i

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/esp_newlib.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_newlib.dir/newlib/port/esp_newlib.c.s"
	cd /build/build/esp-idf/newlib && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tools/ESP8266_RTOS_SDK/components/newlib/newlib/port/esp_newlib.c -o CMakeFiles/__idf_newlib.dir/newlib/port/esp_newlib.c.s

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/esp_newlib.c.obj.requires:

.PHONY : esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/esp_newlib.c.obj.requires

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/esp_newlib.c.obj.provides: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/esp_newlib.c.obj.requires
	$(MAKE) -f esp-idf/newlib/CMakeFiles/__idf_newlib.dir/build.make esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/esp_newlib.c.obj.provides.build
.PHONY : esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/esp_newlib.c.obj.provides

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/esp_newlib.c.obj.provides.build: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/esp_newlib.c.obj


esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/syscall.c.obj: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/flags.make
esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/syscall.c.obj: /tools/ESP8266_RTOS_SDK/components/newlib/newlib/port/syscall.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/syscall.c.obj"
	cd /build/build/esp-idf/newlib && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_newlib.dir/newlib/port/syscall.c.obj   -c /tools/ESP8266_RTOS_SDK/components/newlib/newlib/port/syscall.c

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/syscall.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_newlib.dir/newlib/port/syscall.c.i"
	cd /build/build/esp-idf/newlib && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tools/ESP8266_RTOS_SDK/components/newlib/newlib/port/syscall.c > CMakeFiles/__idf_newlib.dir/newlib/port/syscall.c.i

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/syscall.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_newlib.dir/newlib/port/syscall.c.s"
	cd /build/build/esp-idf/newlib && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tools/ESP8266_RTOS_SDK/components/newlib/newlib/port/syscall.c -o CMakeFiles/__idf_newlib.dir/newlib/port/syscall.c.s

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/syscall.c.obj.requires:

.PHONY : esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/syscall.c.obj.requires

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/syscall.c.obj.provides: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/syscall.c.obj.requires
	$(MAKE) -f esp-idf/newlib/CMakeFiles/__idf_newlib.dir/build.make esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/syscall.c.obj.provides.build
.PHONY : esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/syscall.c.obj.provides

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/syscall.c.obj.provides.build: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/syscall.c.obj


esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/esp_malloc.c.obj: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/flags.make
esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/esp_malloc.c.obj: /tools/ESP8266_RTOS_SDK/components/newlib/newlib/port/esp_malloc.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building C object esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/esp_malloc.c.obj"
	cd /build/build/esp-idf/newlib && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_newlib.dir/newlib/port/esp_malloc.c.obj   -c /tools/ESP8266_RTOS_SDK/components/newlib/newlib/port/esp_malloc.c

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/esp_malloc.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_newlib.dir/newlib/port/esp_malloc.c.i"
	cd /build/build/esp-idf/newlib && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tools/ESP8266_RTOS_SDK/components/newlib/newlib/port/esp_malloc.c > CMakeFiles/__idf_newlib.dir/newlib/port/esp_malloc.c.i

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/esp_malloc.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_newlib.dir/newlib/port/esp_malloc.c.s"
	cd /build/build/esp-idf/newlib && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tools/ESP8266_RTOS_SDK/components/newlib/newlib/port/esp_malloc.c -o CMakeFiles/__idf_newlib.dir/newlib/port/esp_malloc.c.s

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/esp_malloc.c.obj.requires:

.PHONY : esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/esp_malloc.c.obj.requires

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/esp_malloc.c.obj.provides: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/esp_malloc.c.obj.requires
	$(MAKE) -f esp-idf/newlib/CMakeFiles/__idf_newlib.dir/build.make esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/esp_malloc.c.obj.provides.build
.PHONY : esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/esp_malloc.c.obj.provides

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/esp_malloc.c.obj.provides.build: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/esp_malloc.c.obj


# Object files for target __idf_newlib
__idf_newlib_OBJECTS = \
"CMakeFiles/__idf_newlib.dir/newlib/port/time.c.obj" \
"CMakeFiles/__idf_newlib.dir/newlib/port/select.c.obj" \
"CMakeFiles/__idf_newlib.dir/newlib/port/termios.c.obj" \
"CMakeFiles/__idf_newlib.dir/newlib/port/locks.c.obj" \
"CMakeFiles/__idf_newlib.dir/newlib/port/esp_newlib.c.obj" \
"CMakeFiles/__idf_newlib.dir/newlib/port/syscall.c.obj" \
"CMakeFiles/__idf_newlib.dir/newlib/port/esp_malloc.c.obj"

# External object files for target __idf_newlib
__idf_newlib_EXTERNAL_OBJECTS =

esp-idf/newlib/libnewlib.a: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/time.c.obj
esp-idf/newlib/libnewlib.a: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/select.c.obj
esp-idf/newlib/libnewlib.a: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/termios.c.obj
esp-idf/newlib/libnewlib.a: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/locks.c.obj
esp-idf/newlib/libnewlib.a: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/esp_newlib.c.obj
esp-idf/newlib/libnewlib.a: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/syscall.c.obj
esp-idf/newlib/libnewlib.a: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/esp_malloc.c.obj
esp-idf/newlib/libnewlib.a: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/build.make
esp-idf/newlib/libnewlib.a: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Linking CXX static library libnewlib.a"
	cd /build/build/esp-idf/newlib && $(CMAKE_COMMAND) -P CMakeFiles/__idf_newlib.dir/cmake_clean_target.cmake
	cd /build/build/esp-idf/newlib && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/__idf_newlib.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
esp-idf/newlib/CMakeFiles/__idf_newlib.dir/build: esp-idf/newlib/libnewlib.a

.PHONY : esp-idf/newlib/CMakeFiles/__idf_newlib.dir/build

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/requires: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/time.c.obj.requires
esp-idf/newlib/CMakeFiles/__idf_newlib.dir/requires: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/select.c.obj.requires
esp-idf/newlib/CMakeFiles/__idf_newlib.dir/requires: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/termios.c.obj.requires
esp-idf/newlib/CMakeFiles/__idf_newlib.dir/requires: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/locks.c.obj.requires
esp-idf/newlib/CMakeFiles/__idf_newlib.dir/requires: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/esp_newlib.c.obj.requires
esp-idf/newlib/CMakeFiles/__idf_newlib.dir/requires: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/syscall.c.obj.requires
esp-idf/newlib/CMakeFiles/__idf_newlib.dir/requires: esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib/port/esp_malloc.c.obj.requires

.PHONY : esp-idf/newlib/CMakeFiles/__idf_newlib.dir/requires

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/clean:
	cd /build/build/esp-idf/newlib && $(CMAKE_COMMAND) -P CMakeFiles/__idf_newlib.dir/cmake_clean.cmake
.PHONY : esp-idf/newlib/CMakeFiles/__idf_newlib.dir/clean

esp-idf/newlib/CMakeFiles/__idf_newlib.dir/depend:
	cd /build/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /build /tools/ESP8266_RTOS_SDK/components/newlib /build/build /build/build/esp-idf/newlib /build/build/esp-idf/newlib/CMakeFiles/__idf_newlib.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : esp-idf/newlib/CMakeFiles/__idf_newlib.dir/depend

