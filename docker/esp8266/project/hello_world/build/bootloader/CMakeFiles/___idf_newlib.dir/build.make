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
CMAKE_SOURCE_DIR = /tools/ESP8266_RTOS_SDK/components/bootloader/subproject

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /build/build/bootloader

# Utility rule file for ___idf_newlib.

# Include the progress variables for this target.
include CMakeFiles/___idf_newlib.dir/progress.make

CMakeFiles/___idf_newlib:
	EXCLUDE_FROM_ALL

___idf_newlib: CMakeFiles/___idf_newlib
___idf_newlib: CMakeFiles/___idf_newlib.dir/build.make

.PHONY : ___idf_newlib

# Rule to build all files generated by this target.
CMakeFiles/___idf_newlib.dir/build: ___idf_newlib

.PHONY : CMakeFiles/___idf_newlib.dir/build

CMakeFiles/___idf_newlib.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/___idf_newlib.dir/cmake_clean.cmake
.PHONY : CMakeFiles/___idf_newlib.dir/clean

CMakeFiles/___idf_newlib.dir/depend:
	cd /build/build/bootloader && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /tools/ESP8266_RTOS_SDK/components/bootloader/subproject /tools/ESP8266_RTOS_SDK/components/bootloader/subproject /build/build/bootloader /build/build/bootloader /build/build/bootloader/CMakeFiles/___idf_newlib.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/___idf_newlib.dir/depend

