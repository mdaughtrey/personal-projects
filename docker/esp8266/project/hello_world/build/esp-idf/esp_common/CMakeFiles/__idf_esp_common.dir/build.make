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
include esp-idf/esp_common/CMakeFiles/__idf_esp_common.dir/depend.make

# Include the progress variables for this target.
include esp-idf/esp_common/CMakeFiles/__idf_esp_common.dir/progress.make

# Include the compile flags for this target's objects.
include esp-idf/esp_common/CMakeFiles/__idf_esp_common.dir/flags.make

esp-idf/esp_common/CMakeFiles/__idf_esp_common.dir/src/stack_check.c.obj: esp-idf/esp_common/CMakeFiles/__idf_esp_common.dir/flags.make
esp-idf/esp_common/CMakeFiles/__idf_esp_common.dir/src/stack_check.c.obj: /tools/ESP8266_RTOS_SDK/components/esp_common/src/stack_check.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object esp-idf/esp_common/CMakeFiles/__idf_esp_common.dir/src/stack_check.c.obj"
	cd /build/build/esp-idf/esp_common && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -fno-stack-protector -o CMakeFiles/__idf_esp_common.dir/src/stack_check.c.obj   -c /tools/ESP8266_RTOS_SDK/components/esp_common/src/stack_check.c

esp-idf/esp_common/CMakeFiles/__idf_esp_common.dir/src/stack_check.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_esp_common.dir/src/stack_check.c.i"
	cd /build/build/esp-idf/esp_common && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -fno-stack-protector -E /tools/ESP8266_RTOS_SDK/components/esp_common/src/stack_check.c > CMakeFiles/__idf_esp_common.dir/src/stack_check.c.i

esp-idf/esp_common/CMakeFiles/__idf_esp_common.dir/src/stack_check.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_esp_common.dir/src/stack_check.c.s"
	cd /build/build/esp-idf/esp_common && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -fno-stack-protector -S /tools/ESP8266_RTOS_SDK/components/esp_common/src/stack_check.c -o CMakeFiles/__idf_esp_common.dir/src/stack_check.c.s

esp-idf/esp_common/CMakeFiles/__idf_esp_common.dir/src/stack_check.c.obj.requires:

.PHONY : esp-idf/esp_common/CMakeFiles/__idf_esp_common.dir/src/stack_check.c.obj.requires

esp-idf/esp_common/CMakeFiles/__idf_esp_common.dir/src/stack_check.c.obj.provides: esp-idf/esp_common/CMakeFiles/__idf_esp_common.dir/src/stack_check.c.obj.requires
	$(MAKE) -f esp-idf/esp_common/CMakeFiles/__idf_esp_common.dir/build.make esp-idf/esp_common/CMakeFiles/__idf_esp_common.dir/src/stack_check.c.obj.provides.build
.PHONY : esp-idf/esp_common/CMakeFiles/__idf_esp_common.dir/src/stack_check.c.obj.provides

esp-idf/esp_common/CMakeFiles/__idf_esp_common.dir/src/stack_check.c.obj.provides.build: esp-idf/esp_common/CMakeFiles/__idf_esp_common.dir/src/stack_check.c.obj


# Object files for target __idf_esp_common
__idf_esp_common_OBJECTS = \
"CMakeFiles/__idf_esp_common.dir/src/stack_check.c.obj"

# External object files for target __idf_esp_common
__idf_esp_common_EXTERNAL_OBJECTS =

esp-idf/esp_common/libesp_common.a: esp-idf/esp_common/CMakeFiles/__idf_esp_common.dir/src/stack_check.c.obj
esp-idf/esp_common/libesp_common.a: esp-idf/esp_common/CMakeFiles/__idf_esp_common.dir/build.make
esp-idf/esp_common/libesp_common.a: esp-idf/esp_common/CMakeFiles/__idf_esp_common.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libesp_common.a"
	cd /build/build/esp-idf/esp_common && $(CMAKE_COMMAND) -P CMakeFiles/__idf_esp_common.dir/cmake_clean_target.cmake
	cd /build/build/esp-idf/esp_common && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/__idf_esp_common.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
esp-idf/esp_common/CMakeFiles/__idf_esp_common.dir/build: esp-idf/esp_common/libesp_common.a

.PHONY : esp-idf/esp_common/CMakeFiles/__idf_esp_common.dir/build

esp-idf/esp_common/CMakeFiles/__idf_esp_common.dir/requires: esp-idf/esp_common/CMakeFiles/__idf_esp_common.dir/src/stack_check.c.obj.requires

.PHONY : esp-idf/esp_common/CMakeFiles/__idf_esp_common.dir/requires

esp-idf/esp_common/CMakeFiles/__idf_esp_common.dir/clean:
	cd /build/build/esp-idf/esp_common && $(CMAKE_COMMAND) -P CMakeFiles/__idf_esp_common.dir/cmake_clean.cmake
.PHONY : esp-idf/esp_common/CMakeFiles/__idf_esp_common.dir/clean

esp-idf/esp_common/CMakeFiles/__idf_esp_common.dir/depend:
	cd /build/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /build /tools/ESP8266_RTOS_SDK/components/esp_common /build/build /build/build/esp-idf/esp_common /build/build/esp-idf/esp_common/CMakeFiles/__idf_esp_common.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : esp-idf/esp_common/CMakeFiles/__idf_esp_common.dir/depend

