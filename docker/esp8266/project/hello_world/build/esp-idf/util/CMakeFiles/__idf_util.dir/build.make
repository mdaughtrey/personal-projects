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
include esp-idf/util/CMakeFiles/__idf_util.dir/depend.make

# Include the progress variables for this target.
include esp-idf/util/CMakeFiles/__idf_util.dir/progress.make

# Include the compile flags for this target's objects.
include esp-idf/util/CMakeFiles/__idf_util.dir/flags.make

esp-idf/util/CMakeFiles/__idf_util.dir/src/arc4.c.obj: esp-idf/util/CMakeFiles/__idf_util.dir/flags.make
esp-idf/util/CMakeFiles/__idf_util.dir/src/arc4.c.obj: /tools/ESP8266_RTOS_SDK/components/util/src/arc4.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object esp-idf/util/CMakeFiles/__idf_util.dir/src/arc4.c.obj"
	cd /build/build/esp-idf/util && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_util.dir/src/arc4.c.obj   -c /tools/ESP8266_RTOS_SDK/components/util/src/arc4.c

esp-idf/util/CMakeFiles/__idf_util.dir/src/arc4.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_util.dir/src/arc4.c.i"
	cd /build/build/esp-idf/util && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tools/ESP8266_RTOS_SDK/components/util/src/arc4.c > CMakeFiles/__idf_util.dir/src/arc4.c.i

esp-idf/util/CMakeFiles/__idf_util.dir/src/arc4.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_util.dir/src/arc4.c.s"
	cd /build/build/esp-idf/util && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tools/ESP8266_RTOS_SDK/components/util/src/arc4.c -o CMakeFiles/__idf_util.dir/src/arc4.c.s

esp-idf/util/CMakeFiles/__idf_util.dir/src/arc4.c.obj.requires:

.PHONY : esp-idf/util/CMakeFiles/__idf_util.dir/src/arc4.c.obj.requires

esp-idf/util/CMakeFiles/__idf_util.dir/src/arc4.c.obj.provides: esp-idf/util/CMakeFiles/__idf_util.dir/src/arc4.c.obj.requires
	$(MAKE) -f esp-idf/util/CMakeFiles/__idf_util.dir/build.make esp-idf/util/CMakeFiles/__idf_util.dir/src/arc4.c.obj.provides.build
.PHONY : esp-idf/util/CMakeFiles/__idf_util.dir/src/arc4.c.obj.provides

esp-idf/util/CMakeFiles/__idf_util.dir/src/arc4.c.obj.provides.build: esp-idf/util/CMakeFiles/__idf_util.dir/src/arc4.c.obj


esp-idf/util/CMakeFiles/__idf_util.dir/src/base64.c.obj: esp-idf/util/CMakeFiles/__idf_util.dir/flags.make
esp-idf/util/CMakeFiles/__idf_util.dir/src/base64.c.obj: /tools/ESP8266_RTOS_SDK/components/util/src/base64.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object esp-idf/util/CMakeFiles/__idf_util.dir/src/base64.c.obj"
	cd /build/build/esp-idf/util && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_util.dir/src/base64.c.obj   -c /tools/ESP8266_RTOS_SDK/components/util/src/base64.c

esp-idf/util/CMakeFiles/__idf_util.dir/src/base64.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_util.dir/src/base64.c.i"
	cd /build/build/esp-idf/util && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tools/ESP8266_RTOS_SDK/components/util/src/base64.c > CMakeFiles/__idf_util.dir/src/base64.c.i

esp-idf/util/CMakeFiles/__idf_util.dir/src/base64.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_util.dir/src/base64.c.s"
	cd /build/build/esp-idf/util && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tools/ESP8266_RTOS_SDK/components/util/src/base64.c -o CMakeFiles/__idf_util.dir/src/base64.c.s

esp-idf/util/CMakeFiles/__idf_util.dir/src/base64.c.obj.requires:

.PHONY : esp-idf/util/CMakeFiles/__idf_util.dir/src/base64.c.obj.requires

esp-idf/util/CMakeFiles/__idf_util.dir/src/base64.c.obj.provides: esp-idf/util/CMakeFiles/__idf_util.dir/src/base64.c.obj.requires
	$(MAKE) -f esp-idf/util/CMakeFiles/__idf_util.dir/build.make esp-idf/util/CMakeFiles/__idf_util.dir/src/base64.c.obj.provides.build
.PHONY : esp-idf/util/CMakeFiles/__idf_util.dir/src/base64.c.obj.provides

esp-idf/util/CMakeFiles/__idf_util.dir/src/base64.c.obj.provides.build: esp-idf/util/CMakeFiles/__idf_util.dir/src/base64.c.obj


esp-idf/util/CMakeFiles/__idf_util.dir/src/aes.c.obj: esp-idf/util/CMakeFiles/__idf_util.dir/flags.make
esp-idf/util/CMakeFiles/__idf_util.dir/src/aes.c.obj: /tools/ESP8266_RTOS_SDK/components/util/src/aes.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object esp-idf/util/CMakeFiles/__idf_util.dir/src/aes.c.obj"
	cd /build/build/esp-idf/util && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_util.dir/src/aes.c.obj   -c /tools/ESP8266_RTOS_SDK/components/util/src/aes.c

esp-idf/util/CMakeFiles/__idf_util.dir/src/aes.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_util.dir/src/aes.c.i"
	cd /build/build/esp-idf/util && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tools/ESP8266_RTOS_SDK/components/util/src/aes.c > CMakeFiles/__idf_util.dir/src/aes.c.i

esp-idf/util/CMakeFiles/__idf_util.dir/src/aes.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_util.dir/src/aes.c.s"
	cd /build/build/esp-idf/util && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tools/ESP8266_RTOS_SDK/components/util/src/aes.c -o CMakeFiles/__idf_util.dir/src/aes.c.s

esp-idf/util/CMakeFiles/__idf_util.dir/src/aes.c.obj.requires:

.PHONY : esp-idf/util/CMakeFiles/__idf_util.dir/src/aes.c.obj.requires

esp-idf/util/CMakeFiles/__idf_util.dir/src/aes.c.obj.provides: esp-idf/util/CMakeFiles/__idf_util.dir/src/aes.c.obj.requires
	$(MAKE) -f esp-idf/util/CMakeFiles/__idf_util.dir/build.make esp-idf/util/CMakeFiles/__idf_util.dir/src/aes.c.obj.provides.build
.PHONY : esp-idf/util/CMakeFiles/__idf_util.dir/src/aes.c.obj.provides

esp-idf/util/CMakeFiles/__idf_util.dir/src/aes.c.obj.provides.build: esp-idf/util/CMakeFiles/__idf_util.dir/src/aes.c.obj


esp-idf/util/CMakeFiles/__idf_util.dir/src/crc.c.obj: esp-idf/util/CMakeFiles/__idf_util.dir/flags.make
esp-idf/util/CMakeFiles/__idf_util.dir/src/crc.c.obj: /tools/ESP8266_RTOS_SDK/components/util/src/crc.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object esp-idf/util/CMakeFiles/__idf_util.dir/src/crc.c.obj"
	cd /build/build/esp-idf/util && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_util.dir/src/crc.c.obj   -c /tools/ESP8266_RTOS_SDK/components/util/src/crc.c

esp-idf/util/CMakeFiles/__idf_util.dir/src/crc.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_util.dir/src/crc.c.i"
	cd /build/build/esp-idf/util && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tools/ESP8266_RTOS_SDK/components/util/src/crc.c > CMakeFiles/__idf_util.dir/src/crc.c.i

esp-idf/util/CMakeFiles/__idf_util.dir/src/crc.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_util.dir/src/crc.c.s"
	cd /build/build/esp-idf/util && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tools/ESP8266_RTOS_SDK/components/util/src/crc.c -o CMakeFiles/__idf_util.dir/src/crc.c.s

esp-idf/util/CMakeFiles/__idf_util.dir/src/crc.c.obj.requires:

.PHONY : esp-idf/util/CMakeFiles/__idf_util.dir/src/crc.c.obj.requires

esp-idf/util/CMakeFiles/__idf_util.dir/src/crc.c.obj.provides: esp-idf/util/CMakeFiles/__idf_util.dir/src/crc.c.obj.requires
	$(MAKE) -f esp-idf/util/CMakeFiles/__idf_util.dir/build.make esp-idf/util/CMakeFiles/__idf_util.dir/src/crc.c.obj.provides.build
.PHONY : esp-idf/util/CMakeFiles/__idf_util.dir/src/crc.c.obj.provides

esp-idf/util/CMakeFiles/__idf_util.dir/src/crc.c.obj.provides.build: esp-idf/util/CMakeFiles/__idf_util.dir/src/crc.c.obj


esp-idf/util/CMakeFiles/__idf_util.dir/src/sha.c.obj: esp-idf/util/CMakeFiles/__idf_util.dir/flags.make
esp-idf/util/CMakeFiles/__idf_util.dir/src/sha.c.obj: /tools/ESP8266_RTOS_SDK/components/util/src/sha.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object esp-idf/util/CMakeFiles/__idf_util.dir/src/sha.c.obj"
	cd /build/build/esp-idf/util && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_util.dir/src/sha.c.obj   -c /tools/ESP8266_RTOS_SDK/components/util/src/sha.c

esp-idf/util/CMakeFiles/__idf_util.dir/src/sha.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_util.dir/src/sha.c.i"
	cd /build/build/esp-idf/util && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tools/ESP8266_RTOS_SDK/components/util/src/sha.c > CMakeFiles/__idf_util.dir/src/sha.c.i

esp-idf/util/CMakeFiles/__idf_util.dir/src/sha.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_util.dir/src/sha.c.s"
	cd /build/build/esp-idf/util && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tools/ESP8266_RTOS_SDK/components/util/src/sha.c -o CMakeFiles/__idf_util.dir/src/sha.c.s

esp-idf/util/CMakeFiles/__idf_util.dir/src/sha.c.obj.requires:

.PHONY : esp-idf/util/CMakeFiles/__idf_util.dir/src/sha.c.obj.requires

esp-idf/util/CMakeFiles/__idf_util.dir/src/sha.c.obj.provides: esp-idf/util/CMakeFiles/__idf_util.dir/src/sha.c.obj.requires
	$(MAKE) -f esp-idf/util/CMakeFiles/__idf_util.dir/build.make esp-idf/util/CMakeFiles/__idf_util.dir/src/sha.c.obj.provides.build
.PHONY : esp-idf/util/CMakeFiles/__idf_util.dir/src/sha.c.obj.provides

esp-idf/util/CMakeFiles/__idf_util.dir/src/sha.c.obj.provides.build: esp-idf/util/CMakeFiles/__idf_util.dir/src/sha.c.obj


esp-idf/util/CMakeFiles/__idf_util.dir/src/md5.c.obj: esp-idf/util/CMakeFiles/__idf_util.dir/flags.make
esp-idf/util/CMakeFiles/__idf_util.dir/src/md5.c.obj: /tools/ESP8266_RTOS_SDK/components/util/src/md5.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object esp-idf/util/CMakeFiles/__idf_util.dir/src/md5.c.obj"
	cd /build/build/esp-idf/util && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_util.dir/src/md5.c.obj   -c /tools/ESP8266_RTOS_SDK/components/util/src/md5.c

esp-idf/util/CMakeFiles/__idf_util.dir/src/md5.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_util.dir/src/md5.c.i"
	cd /build/build/esp-idf/util && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tools/ESP8266_RTOS_SDK/components/util/src/md5.c > CMakeFiles/__idf_util.dir/src/md5.c.i

esp-idf/util/CMakeFiles/__idf_util.dir/src/md5.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_util.dir/src/md5.c.s"
	cd /build/build/esp-idf/util && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tools/ESP8266_RTOS_SDK/components/util/src/md5.c -o CMakeFiles/__idf_util.dir/src/md5.c.s

esp-idf/util/CMakeFiles/__idf_util.dir/src/md5.c.obj.requires:

.PHONY : esp-idf/util/CMakeFiles/__idf_util.dir/src/md5.c.obj.requires

esp-idf/util/CMakeFiles/__idf_util.dir/src/md5.c.obj.provides: esp-idf/util/CMakeFiles/__idf_util.dir/src/md5.c.obj.requires
	$(MAKE) -f esp-idf/util/CMakeFiles/__idf_util.dir/build.make esp-idf/util/CMakeFiles/__idf_util.dir/src/md5.c.obj.provides.build
.PHONY : esp-idf/util/CMakeFiles/__idf_util.dir/src/md5.c.obj.provides

esp-idf/util/CMakeFiles/__idf_util.dir/src/md5.c.obj.provides.build: esp-idf/util/CMakeFiles/__idf_util.dir/src/md5.c.obj


# Object files for target __idf_util
__idf_util_OBJECTS = \
"CMakeFiles/__idf_util.dir/src/arc4.c.obj" \
"CMakeFiles/__idf_util.dir/src/base64.c.obj" \
"CMakeFiles/__idf_util.dir/src/aes.c.obj" \
"CMakeFiles/__idf_util.dir/src/crc.c.obj" \
"CMakeFiles/__idf_util.dir/src/sha.c.obj" \
"CMakeFiles/__idf_util.dir/src/md5.c.obj"

# External object files for target __idf_util
__idf_util_EXTERNAL_OBJECTS =

esp-idf/util/libutil.a: esp-idf/util/CMakeFiles/__idf_util.dir/src/arc4.c.obj
esp-idf/util/libutil.a: esp-idf/util/CMakeFiles/__idf_util.dir/src/base64.c.obj
esp-idf/util/libutil.a: esp-idf/util/CMakeFiles/__idf_util.dir/src/aes.c.obj
esp-idf/util/libutil.a: esp-idf/util/CMakeFiles/__idf_util.dir/src/crc.c.obj
esp-idf/util/libutil.a: esp-idf/util/CMakeFiles/__idf_util.dir/src/sha.c.obj
esp-idf/util/libutil.a: esp-idf/util/CMakeFiles/__idf_util.dir/src/md5.c.obj
esp-idf/util/libutil.a: esp-idf/util/CMakeFiles/__idf_util.dir/build.make
esp-idf/util/libutil.a: esp-idf/util/CMakeFiles/__idf_util.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Linking CXX static library libutil.a"
	cd /build/build/esp-idf/util && $(CMAKE_COMMAND) -P CMakeFiles/__idf_util.dir/cmake_clean_target.cmake
	cd /build/build/esp-idf/util && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/__idf_util.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
esp-idf/util/CMakeFiles/__idf_util.dir/build: esp-idf/util/libutil.a

.PHONY : esp-idf/util/CMakeFiles/__idf_util.dir/build

esp-idf/util/CMakeFiles/__idf_util.dir/requires: esp-idf/util/CMakeFiles/__idf_util.dir/src/arc4.c.obj.requires
esp-idf/util/CMakeFiles/__idf_util.dir/requires: esp-idf/util/CMakeFiles/__idf_util.dir/src/base64.c.obj.requires
esp-idf/util/CMakeFiles/__idf_util.dir/requires: esp-idf/util/CMakeFiles/__idf_util.dir/src/aes.c.obj.requires
esp-idf/util/CMakeFiles/__idf_util.dir/requires: esp-idf/util/CMakeFiles/__idf_util.dir/src/crc.c.obj.requires
esp-idf/util/CMakeFiles/__idf_util.dir/requires: esp-idf/util/CMakeFiles/__idf_util.dir/src/sha.c.obj.requires
esp-idf/util/CMakeFiles/__idf_util.dir/requires: esp-idf/util/CMakeFiles/__idf_util.dir/src/md5.c.obj.requires

.PHONY : esp-idf/util/CMakeFiles/__idf_util.dir/requires

esp-idf/util/CMakeFiles/__idf_util.dir/clean:
	cd /build/build/esp-idf/util && $(CMAKE_COMMAND) -P CMakeFiles/__idf_util.dir/cmake_clean.cmake
.PHONY : esp-idf/util/CMakeFiles/__idf_util.dir/clean

esp-idf/util/CMakeFiles/__idf_util.dir/depend:
	cd /build/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /build /tools/ESP8266_RTOS_SDK/components/util /build/build /build/build/esp-idf/util /build/build/esp-idf/util/CMakeFiles/__idf_util.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : esp-idf/util/CMakeFiles/__idf_util.dir/depend

