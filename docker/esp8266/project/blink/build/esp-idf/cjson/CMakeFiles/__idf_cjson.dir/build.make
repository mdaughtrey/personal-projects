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
include esp-idf/cjson/CMakeFiles/__idf_cjson.dir/depend.make

# Include the progress variables for this target.
include esp-idf/cjson/CMakeFiles/__idf_cjson.dir/progress.make

# Include the compile flags for this target's objects.
include esp-idf/cjson/CMakeFiles/__idf_cjson.dir/flags.make

esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/cJSON_Utils.c.obj: esp-idf/cjson/CMakeFiles/__idf_cjson.dir/flags.make
esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/cJSON_Utils.c.obj: /tools/ESP8266_RTOS_SDK/components/cjson/cJSON/cJSON_Utils.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/cJSON_Utils.c.obj"
	cd /build/build/esp-idf/cjson && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_cjson.dir/cJSON/cJSON_Utils.c.obj   -c /tools/ESP8266_RTOS_SDK/components/cjson/cJSON/cJSON_Utils.c

esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/cJSON_Utils.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_cjson.dir/cJSON/cJSON_Utils.c.i"
	cd /build/build/esp-idf/cjson && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tools/ESP8266_RTOS_SDK/components/cjson/cJSON/cJSON_Utils.c > CMakeFiles/__idf_cjson.dir/cJSON/cJSON_Utils.c.i

esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/cJSON_Utils.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_cjson.dir/cJSON/cJSON_Utils.c.s"
	cd /build/build/esp-idf/cjson && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tools/ESP8266_RTOS_SDK/components/cjson/cJSON/cJSON_Utils.c -o CMakeFiles/__idf_cjson.dir/cJSON/cJSON_Utils.c.s

esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/cJSON_Utils.c.obj.requires:

.PHONY : esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/cJSON_Utils.c.obj.requires

esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/cJSON_Utils.c.obj.provides: esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/cJSON_Utils.c.obj.requires
	$(MAKE) -f esp-idf/cjson/CMakeFiles/__idf_cjson.dir/build.make esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/cJSON_Utils.c.obj.provides.build
.PHONY : esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/cJSON_Utils.c.obj.provides

esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/cJSON_Utils.c.obj.provides.build: esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/cJSON_Utils.c.obj


esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/test.c.obj: esp-idf/cjson/CMakeFiles/__idf_cjson.dir/flags.make
esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/test.c.obj: /tools/ESP8266_RTOS_SDK/components/cjson/cJSON/test.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/test.c.obj"
	cd /build/build/esp-idf/cjson && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_cjson.dir/cJSON/test.c.obj   -c /tools/ESP8266_RTOS_SDK/components/cjson/cJSON/test.c

esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/test.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_cjson.dir/cJSON/test.c.i"
	cd /build/build/esp-idf/cjson && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tools/ESP8266_RTOS_SDK/components/cjson/cJSON/test.c > CMakeFiles/__idf_cjson.dir/cJSON/test.c.i

esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/test.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_cjson.dir/cJSON/test.c.s"
	cd /build/build/esp-idf/cjson && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tools/ESP8266_RTOS_SDK/components/cjson/cJSON/test.c -o CMakeFiles/__idf_cjson.dir/cJSON/test.c.s

esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/test.c.obj.requires:

.PHONY : esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/test.c.obj.requires

esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/test.c.obj.provides: esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/test.c.obj.requires
	$(MAKE) -f esp-idf/cjson/CMakeFiles/__idf_cjson.dir/build.make esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/test.c.obj.provides.build
.PHONY : esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/test.c.obj.provides

esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/test.c.obj.provides.build: esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/test.c.obj


esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/cJSON.c.obj: esp-idf/cjson/CMakeFiles/__idf_cjson.dir/flags.make
esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/cJSON.c.obj: /tools/ESP8266_RTOS_SDK/components/cjson/cJSON/cJSON.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/cJSON.c.obj"
	cd /build/build/esp-idf/cjson && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_cjson.dir/cJSON/cJSON.c.obj   -c /tools/ESP8266_RTOS_SDK/components/cjson/cJSON/cJSON.c

esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/cJSON.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_cjson.dir/cJSON/cJSON.c.i"
	cd /build/build/esp-idf/cjson && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tools/ESP8266_RTOS_SDK/components/cjson/cJSON/cJSON.c > CMakeFiles/__idf_cjson.dir/cJSON/cJSON.c.i

esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/cJSON.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_cjson.dir/cJSON/cJSON.c.s"
	cd /build/build/esp-idf/cjson && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tools/ESP8266_RTOS_SDK/components/cjson/cJSON/cJSON.c -o CMakeFiles/__idf_cjson.dir/cJSON/cJSON.c.s

esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/cJSON.c.obj.requires:

.PHONY : esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/cJSON.c.obj.requires

esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/cJSON.c.obj.provides: esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/cJSON.c.obj.requires
	$(MAKE) -f esp-idf/cjson/CMakeFiles/__idf_cjson.dir/build.make esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/cJSON.c.obj.provides.build
.PHONY : esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/cJSON.c.obj.provides

esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/cJSON.c.obj.provides.build: esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/cJSON.c.obj


# Object files for target __idf_cjson
__idf_cjson_OBJECTS = \
"CMakeFiles/__idf_cjson.dir/cJSON/cJSON_Utils.c.obj" \
"CMakeFiles/__idf_cjson.dir/cJSON/test.c.obj" \
"CMakeFiles/__idf_cjson.dir/cJSON/cJSON.c.obj"

# External object files for target __idf_cjson
__idf_cjson_EXTERNAL_OBJECTS =

esp-idf/cjson/libcjson.a: esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/cJSON_Utils.c.obj
esp-idf/cjson/libcjson.a: esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/test.c.obj
esp-idf/cjson/libcjson.a: esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/cJSON.c.obj
esp-idf/cjson/libcjson.a: esp-idf/cjson/CMakeFiles/__idf_cjson.dir/build.make
esp-idf/cjson/libcjson.a: esp-idf/cjson/CMakeFiles/__idf_cjson.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX static library libcjson.a"
	cd /build/build/esp-idf/cjson && $(CMAKE_COMMAND) -P CMakeFiles/__idf_cjson.dir/cmake_clean_target.cmake
	cd /build/build/esp-idf/cjson && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/__idf_cjson.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
esp-idf/cjson/CMakeFiles/__idf_cjson.dir/build: esp-idf/cjson/libcjson.a

.PHONY : esp-idf/cjson/CMakeFiles/__idf_cjson.dir/build

esp-idf/cjson/CMakeFiles/__idf_cjson.dir/requires: esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/cJSON_Utils.c.obj.requires
esp-idf/cjson/CMakeFiles/__idf_cjson.dir/requires: esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/test.c.obj.requires
esp-idf/cjson/CMakeFiles/__idf_cjson.dir/requires: esp-idf/cjson/CMakeFiles/__idf_cjson.dir/cJSON/cJSON.c.obj.requires

.PHONY : esp-idf/cjson/CMakeFiles/__idf_cjson.dir/requires

esp-idf/cjson/CMakeFiles/__idf_cjson.dir/clean:
	cd /build/build/esp-idf/cjson && $(CMAKE_COMMAND) -P CMakeFiles/__idf_cjson.dir/cmake_clean.cmake
.PHONY : esp-idf/cjson/CMakeFiles/__idf_cjson.dir/clean

esp-idf/cjson/CMakeFiles/__idf_cjson.dir/depend:
	cd /build/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /build /tools/ESP8266_RTOS_SDK/components/cjson /build/build /build/build/esp-idf/cjson /build/build/esp-idf/cjson/CMakeFiles/__idf_cjson.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : esp-idf/cjson/CMakeFiles/__idf_cjson.dir/depend

