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

# Utility rule file for ___idf_freertos.

# Include the progress variables for this target.
include CMakeFiles/___idf_freertos.dir/progress.make

CMakeFiles/___idf_freertos:
	EXCLUDE_FROM_ALL

___idf_freertos: CMakeFiles/___idf_freertos
___idf_freertos: CMakeFiles/___idf_freertos.dir/build.make

.PHONY : ___idf_freertos

# Rule to build all files generated by this target.
CMakeFiles/___idf_freertos.dir/build: ___idf_freertos

.PHONY : CMakeFiles/___idf_freertos.dir/build

CMakeFiles/___idf_freertos.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/___idf_freertos.dir/cmake_clean.cmake
.PHONY : CMakeFiles/___idf_freertos.dir/clean

CMakeFiles/___idf_freertos.dir/depend:
	cd /build/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /build /build /build/build /build/build /build/build/CMakeFiles/___idf_freertos.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/___idf_freertos.dir/depend

