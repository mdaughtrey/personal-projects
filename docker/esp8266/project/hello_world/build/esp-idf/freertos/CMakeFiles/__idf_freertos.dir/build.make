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
include esp-idf/freertos/CMakeFiles/__idf_freertos.dir/depend.make

# Include the progress variables for this target.
include esp-idf/freertos/CMakeFiles/__idf_freertos.dir/progress.make

# Include the compile flags for this target's objects.
include esp-idf/freertos/CMakeFiles/__idf_freertos.dir/flags.make

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/stream_buffer.c.obj: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/flags.make
esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/stream_buffer.c.obj: /tools/ESP8266_RTOS_SDK/components/freertos/freertos/stream_buffer.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/stream_buffer.c.obj"
	cd /build/build/esp-idf/freertos && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_freertos.dir/freertos/stream_buffer.c.obj   -c /tools/ESP8266_RTOS_SDK/components/freertos/freertos/stream_buffer.c

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/stream_buffer.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_freertos.dir/freertos/stream_buffer.c.i"
	cd /build/build/esp-idf/freertos && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tools/ESP8266_RTOS_SDK/components/freertos/freertos/stream_buffer.c > CMakeFiles/__idf_freertos.dir/freertos/stream_buffer.c.i

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/stream_buffer.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_freertos.dir/freertos/stream_buffer.c.s"
	cd /build/build/esp-idf/freertos && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tools/ESP8266_RTOS_SDK/components/freertos/freertos/stream_buffer.c -o CMakeFiles/__idf_freertos.dir/freertos/stream_buffer.c.s

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/stream_buffer.c.obj.requires:

.PHONY : esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/stream_buffer.c.obj.requires

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/stream_buffer.c.obj.provides: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/stream_buffer.c.obj.requires
	$(MAKE) -f esp-idf/freertos/CMakeFiles/__idf_freertos.dir/build.make esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/stream_buffer.c.obj.provides.build
.PHONY : esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/stream_buffer.c.obj.provides

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/stream_buffer.c.obj.provides.build: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/stream_buffer.c.obj


esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/event_groups.c.obj: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/flags.make
esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/event_groups.c.obj: /tools/ESP8266_RTOS_SDK/components/freertos/freertos/event_groups.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/event_groups.c.obj"
	cd /build/build/esp-idf/freertos && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_freertos.dir/freertos/event_groups.c.obj   -c /tools/ESP8266_RTOS_SDK/components/freertos/freertos/event_groups.c

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/event_groups.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_freertos.dir/freertos/event_groups.c.i"
	cd /build/build/esp-idf/freertos && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tools/ESP8266_RTOS_SDK/components/freertos/freertos/event_groups.c > CMakeFiles/__idf_freertos.dir/freertos/event_groups.c.i

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/event_groups.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_freertos.dir/freertos/event_groups.c.s"
	cd /build/build/esp-idf/freertos && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tools/ESP8266_RTOS_SDK/components/freertos/freertos/event_groups.c -o CMakeFiles/__idf_freertos.dir/freertos/event_groups.c.s

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/event_groups.c.obj.requires:

.PHONY : esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/event_groups.c.obj.requires

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/event_groups.c.obj.provides: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/event_groups.c.obj.requires
	$(MAKE) -f esp-idf/freertos/CMakeFiles/__idf_freertos.dir/build.make esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/event_groups.c.obj.provides.build
.PHONY : esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/event_groups.c.obj.provides

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/event_groups.c.obj.provides.build: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/event_groups.c.obj


esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/tasks.c.obj: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/flags.make
esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/tasks.c.obj: /tools/ESP8266_RTOS_SDK/components/freertos/freertos/tasks.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/tasks.c.obj"
	cd /build/build/esp-idf/freertos && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_freertos.dir/freertos/tasks.c.obj   -c /tools/ESP8266_RTOS_SDK/components/freertos/freertos/tasks.c

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/tasks.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_freertos.dir/freertos/tasks.c.i"
	cd /build/build/esp-idf/freertos && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tools/ESP8266_RTOS_SDK/components/freertos/freertos/tasks.c > CMakeFiles/__idf_freertos.dir/freertos/tasks.c.i

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/tasks.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_freertos.dir/freertos/tasks.c.s"
	cd /build/build/esp-idf/freertos && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tools/ESP8266_RTOS_SDK/components/freertos/freertos/tasks.c -o CMakeFiles/__idf_freertos.dir/freertos/tasks.c.s

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/tasks.c.obj.requires:

.PHONY : esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/tasks.c.obj.requires

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/tasks.c.obj.provides: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/tasks.c.obj.requires
	$(MAKE) -f esp-idf/freertos/CMakeFiles/__idf_freertos.dir/build.make esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/tasks.c.obj.provides.build
.PHONY : esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/tasks.c.obj.provides

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/tasks.c.obj.provides.build: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/tasks.c.obj


esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/queue.c.obj: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/flags.make
esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/queue.c.obj: /tools/ESP8266_RTOS_SDK/components/freertos/freertos/queue.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/queue.c.obj"
	cd /build/build/esp-idf/freertos && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_freertos.dir/freertos/queue.c.obj   -c /tools/ESP8266_RTOS_SDK/components/freertos/freertos/queue.c

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/queue.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_freertos.dir/freertos/queue.c.i"
	cd /build/build/esp-idf/freertos && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tools/ESP8266_RTOS_SDK/components/freertos/freertos/queue.c > CMakeFiles/__idf_freertos.dir/freertos/queue.c.i

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/queue.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_freertos.dir/freertos/queue.c.s"
	cd /build/build/esp-idf/freertos && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tools/ESP8266_RTOS_SDK/components/freertos/freertos/queue.c -o CMakeFiles/__idf_freertos.dir/freertos/queue.c.s

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/queue.c.obj.requires:

.PHONY : esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/queue.c.obj.requires

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/queue.c.obj.provides: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/queue.c.obj.requires
	$(MAKE) -f esp-idf/freertos/CMakeFiles/__idf_freertos.dir/build.make esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/queue.c.obj.provides.build
.PHONY : esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/queue.c.obj.provides

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/queue.c.obj.provides.build: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/queue.c.obj


esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/list.c.obj: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/flags.make
esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/list.c.obj: /tools/ESP8266_RTOS_SDK/components/freertos/freertos/list.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/list.c.obj"
	cd /build/build/esp-idf/freertos && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_freertos.dir/freertos/list.c.obj   -c /tools/ESP8266_RTOS_SDK/components/freertos/freertos/list.c

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/list.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_freertos.dir/freertos/list.c.i"
	cd /build/build/esp-idf/freertos && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tools/ESP8266_RTOS_SDK/components/freertos/freertos/list.c > CMakeFiles/__idf_freertos.dir/freertos/list.c.i

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/list.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_freertos.dir/freertos/list.c.s"
	cd /build/build/esp-idf/freertos && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tools/ESP8266_RTOS_SDK/components/freertos/freertos/list.c -o CMakeFiles/__idf_freertos.dir/freertos/list.c.s

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/list.c.obj.requires:

.PHONY : esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/list.c.obj.requires

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/list.c.obj.provides: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/list.c.obj.requires
	$(MAKE) -f esp-idf/freertos/CMakeFiles/__idf_freertos.dir/build.make esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/list.c.obj.provides.build
.PHONY : esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/list.c.obj.provides

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/list.c.obj.provides.build: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/list.c.obj


esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/timers.c.obj: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/flags.make
esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/timers.c.obj: /tools/ESP8266_RTOS_SDK/components/freertos/freertos/timers.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/timers.c.obj"
	cd /build/build/esp-idf/freertos && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_freertos.dir/freertos/timers.c.obj   -c /tools/ESP8266_RTOS_SDK/components/freertos/freertos/timers.c

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/timers.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_freertos.dir/freertos/timers.c.i"
	cd /build/build/esp-idf/freertos && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tools/ESP8266_RTOS_SDK/components/freertos/freertos/timers.c > CMakeFiles/__idf_freertos.dir/freertos/timers.c.i

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/timers.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_freertos.dir/freertos/timers.c.s"
	cd /build/build/esp-idf/freertos && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tools/ESP8266_RTOS_SDK/components/freertos/freertos/timers.c -o CMakeFiles/__idf_freertos.dir/freertos/timers.c.s

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/timers.c.obj.requires:

.PHONY : esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/timers.c.obj.requires

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/timers.c.obj.provides: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/timers.c.obj.requires
	$(MAKE) -f esp-idf/freertos/CMakeFiles/__idf_freertos.dir/build.make esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/timers.c.obj.provides.build
.PHONY : esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/timers.c.obj.provides

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/timers.c.obj.provides.build: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/timers.c.obj


esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/freertos_hooks.c.obj: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/flags.make
esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/freertos_hooks.c.obj: /tools/ESP8266_RTOS_SDK/components/freertos/port/esp8266/freertos_hooks.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building C object esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/freertos_hooks.c.obj"
	cd /build/build/esp-idf/freertos && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_freertos.dir/port/esp8266/freertos_hooks.c.obj   -c /tools/ESP8266_RTOS_SDK/components/freertos/port/esp8266/freertos_hooks.c

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/freertos_hooks.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_freertos.dir/port/esp8266/freertos_hooks.c.i"
	cd /build/build/esp-idf/freertos && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tools/ESP8266_RTOS_SDK/components/freertos/port/esp8266/freertos_hooks.c > CMakeFiles/__idf_freertos.dir/port/esp8266/freertos_hooks.c.i

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/freertos_hooks.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_freertos.dir/port/esp8266/freertos_hooks.c.s"
	cd /build/build/esp-idf/freertos && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tools/ESP8266_RTOS_SDK/components/freertos/port/esp8266/freertos_hooks.c -o CMakeFiles/__idf_freertos.dir/port/esp8266/freertos_hooks.c.s

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/freertos_hooks.c.obj.requires:

.PHONY : esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/freertos_hooks.c.obj.requires

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/freertos_hooks.c.obj.provides: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/freertos_hooks.c.obj.requires
	$(MAKE) -f esp-idf/freertos/CMakeFiles/__idf_freertos.dir/build.make esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/freertos_hooks.c.obj.provides.build
.PHONY : esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/freertos_hooks.c.obj.provides

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/freertos_hooks.c.obj.provides.build: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/freertos_hooks.c.obj


esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/impure.c.obj: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/flags.make
esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/impure.c.obj: /tools/ESP8266_RTOS_SDK/components/freertos/port/esp8266/impure.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building C object esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/impure.c.obj"
	cd /build/build/esp-idf/freertos && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_freertos.dir/port/esp8266/impure.c.obj   -c /tools/ESP8266_RTOS_SDK/components/freertos/port/esp8266/impure.c

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/impure.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_freertos.dir/port/esp8266/impure.c.i"
	cd /build/build/esp-idf/freertos && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tools/ESP8266_RTOS_SDK/components/freertos/port/esp8266/impure.c > CMakeFiles/__idf_freertos.dir/port/esp8266/impure.c.i

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/impure.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_freertos.dir/port/esp8266/impure.c.s"
	cd /build/build/esp-idf/freertos && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tools/ESP8266_RTOS_SDK/components/freertos/port/esp8266/impure.c -o CMakeFiles/__idf_freertos.dir/port/esp8266/impure.c.s

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/impure.c.obj.requires:

.PHONY : esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/impure.c.obj.requires

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/impure.c.obj.provides: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/impure.c.obj.requires
	$(MAKE) -f esp-idf/freertos/CMakeFiles/__idf_freertos.dir/build.make esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/impure.c.obj.provides.build
.PHONY : esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/impure.c.obj.provides

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/impure.c.obj.provides.build: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/impure.c.obj


esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/port.c.obj: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/flags.make
esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/port.c.obj: /tools/ESP8266_RTOS_SDK/components/freertos/port/esp8266/port.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building C object esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/port.c.obj"
	cd /build/build/esp-idf/freertos && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_freertos.dir/port/esp8266/port.c.obj   -c /tools/ESP8266_RTOS_SDK/components/freertos/port/esp8266/port.c

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/port.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_freertos.dir/port/esp8266/port.c.i"
	cd /build/build/esp-idf/freertos && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tools/ESP8266_RTOS_SDK/components/freertos/port/esp8266/port.c > CMakeFiles/__idf_freertos.dir/port/esp8266/port.c.i

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/port.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_freertos.dir/port/esp8266/port.c.s"
	cd /build/build/esp-idf/freertos && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tools/ESP8266_RTOS_SDK/components/freertos/port/esp8266/port.c -o CMakeFiles/__idf_freertos.dir/port/esp8266/port.c.s

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/port.c.obj.requires:

.PHONY : esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/port.c.obj.requires

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/port.c.obj.provides: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/port.c.obj.requires
	$(MAKE) -f esp-idf/freertos/CMakeFiles/__idf_freertos.dir/build.make esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/port.c.obj.provides.build
.PHONY : esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/port.c.obj.provides

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/port.c.obj.provides.build: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/port.c.obj


esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/panic.c.obj: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/flags.make
esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/panic.c.obj: /tools/ESP8266_RTOS_SDK/components/freertos/port/esp8266/panic.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building C object esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/panic.c.obj"
	cd /build/build/esp-idf/freertos && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_freertos.dir/port/esp8266/panic.c.obj   -c /tools/ESP8266_RTOS_SDK/components/freertos/port/esp8266/panic.c

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/panic.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_freertos.dir/port/esp8266/panic.c.i"
	cd /build/build/esp-idf/freertos && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tools/ESP8266_RTOS_SDK/components/freertos/port/esp8266/panic.c > CMakeFiles/__idf_freertos.dir/port/esp8266/panic.c.i

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/panic.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_freertos.dir/port/esp8266/panic.c.s"
	cd /build/build/esp-idf/freertos && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tools/ESP8266_RTOS_SDK/components/freertos/port/esp8266/panic.c -o CMakeFiles/__idf_freertos.dir/port/esp8266/panic.c.s

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/panic.c.obj.requires:

.PHONY : esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/panic.c.obj.requires

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/panic.c.obj.provides: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/panic.c.obj.requires
	$(MAKE) -f esp-idf/freertos/CMakeFiles/__idf_freertos.dir/build.make esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/panic.c.obj.provides.build
.PHONY : esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/panic.c.obj.provides

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/panic.c.obj.provides.build: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/panic.c.obj


esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/xtensa_vectors.S.obj: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/flags.make
esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/xtensa_vectors.S.obj: /tools/ESP8266_RTOS_SDK/components/freertos/port/esp8266/xtensa_vectors.S
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building ASM object esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/xtensa_vectors.S.obj"
	cd /build/build/esp-idf/freertos && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(ASM_DEFINES) $(ASM_INCLUDES) $(ASM_FLAGS) -o CMakeFiles/__idf_freertos.dir/port/esp8266/xtensa_vectors.S.obj -c /tools/ESP8266_RTOS_SDK/components/freertos/port/esp8266/xtensa_vectors.S

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/xtensa_vectors.S.obj.requires:

.PHONY : esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/xtensa_vectors.S.obj.requires

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/xtensa_vectors.S.obj.provides: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/xtensa_vectors.S.obj.requires
	$(MAKE) -f esp-idf/freertos/CMakeFiles/__idf_freertos.dir/build.make esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/xtensa_vectors.S.obj.provides.build
.PHONY : esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/xtensa_vectors.S.obj.provides

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/xtensa_vectors.S.obj.provides.build: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/xtensa_vectors.S.obj


esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/os_cpu_a.S.obj: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/flags.make
esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/os_cpu_a.S.obj: /tools/ESP8266_RTOS_SDK/components/freertos/port/esp8266/os_cpu_a.S
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building ASM object esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/os_cpu_a.S.obj"
	cd /build/build/esp-idf/freertos && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(ASM_DEFINES) $(ASM_INCLUDES) $(ASM_FLAGS) -o CMakeFiles/__idf_freertos.dir/port/esp8266/os_cpu_a.S.obj -c /tools/ESP8266_RTOS_SDK/components/freertos/port/esp8266/os_cpu_a.S

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/os_cpu_a.S.obj.requires:

.PHONY : esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/os_cpu_a.S.obj.requires

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/os_cpu_a.S.obj.provides: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/os_cpu_a.S.obj.requires
	$(MAKE) -f esp-idf/freertos/CMakeFiles/__idf_freertos.dir/build.make esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/os_cpu_a.S.obj.provides.build
.PHONY : esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/os_cpu_a.S.obj.provides

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/os_cpu_a.S.obj.provides.build: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/os_cpu_a.S.obj


esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/xtensa_context.S.obj: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/flags.make
esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/xtensa_context.S.obj: /tools/ESP8266_RTOS_SDK/components/freertos/port/esp8266/xtensa_context.S
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Building ASM object esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/xtensa_context.S.obj"
	cd /build/build/esp-idf/freertos && /tools/toolchain/bin/xtensa-lx106-elf-gcc  $(ASM_DEFINES) $(ASM_INCLUDES) $(ASM_FLAGS) -o CMakeFiles/__idf_freertos.dir/port/esp8266/xtensa_context.S.obj -c /tools/ESP8266_RTOS_SDK/components/freertos/port/esp8266/xtensa_context.S

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/xtensa_context.S.obj.requires:

.PHONY : esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/xtensa_context.S.obj.requires

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/xtensa_context.S.obj.provides: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/xtensa_context.S.obj.requires
	$(MAKE) -f esp-idf/freertos/CMakeFiles/__idf_freertos.dir/build.make esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/xtensa_context.S.obj.provides.build
.PHONY : esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/xtensa_context.S.obj.provides

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/xtensa_context.S.obj.provides.build: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/xtensa_context.S.obj


# Object files for target __idf_freertos
__idf_freertos_OBJECTS = \
"CMakeFiles/__idf_freertos.dir/freertos/stream_buffer.c.obj" \
"CMakeFiles/__idf_freertos.dir/freertos/event_groups.c.obj" \
"CMakeFiles/__idf_freertos.dir/freertos/tasks.c.obj" \
"CMakeFiles/__idf_freertos.dir/freertos/queue.c.obj" \
"CMakeFiles/__idf_freertos.dir/freertos/list.c.obj" \
"CMakeFiles/__idf_freertos.dir/freertos/timers.c.obj" \
"CMakeFiles/__idf_freertos.dir/port/esp8266/freertos_hooks.c.obj" \
"CMakeFiles/__idf_freertos.dir/port/esp8266/impure.c.obj" \
"CMakeFiles/__idf_freertos.dir/port/esp8266/port.c.obj" \
"CMakeFiles/__idf_freertos.dir/port/esp8266/panic.c.obj" \
"CMakeFiles/__idf_freertos.dir/port/esp8266/xtensa_vectors.S.obj" \
"CMakeFiles/__idf_freertos.dir/port/esp8266/os_cpu_a.S.obj" \
"CMakeFiles/__idf_freertos.dir/port/esp8266/xtensa_context.S.obj"

# External object files for target __idf_freertos
__idf_freertos_EXTERNAL_OBJECTS =

esp-idf/freertos/libfreertos.a: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/stream_buffer.c.obj
esp-idf/freertos/libfreertos.a: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/event_groups.c.obj
esp-idf/freertos/libfreertos.a: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/tasks.c.obj
esp-idf/freertos/libfreertos.a: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/queue.c.obj
esp-idf/freertos/libfreertos.a: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/list.c.obj
esp-idf/freertos/libfreertos.a: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/timers.c.obj
esp-idf/freertos/libfreertos.a: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/freertos_hooks.c.obj
esp-idf/freertos/libfreertos.a: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/impure.c.obj
esp-idf/freertos/libfreertos.a: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/port.c.obj
esp-idf/freertos/libfreertos.a: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/panic.c.obj
esp-idf/freertos/libfreertos.a: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/xtensa_vectors.S.obj
esp-idf/freertos/libfreertos.a: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/os_cpu_a.S.obj
esp-idf/freertos/libfreertos.a: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/xtensa_context.S.obj
esp-idf/freertos/libfreertos.a: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/build.make
esp-idf/freertos/libfreertos.a: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/build/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_14) "Linking CXX static library libfreertos.a"
	cd /build/build/esp-idf/freertos && $(CMAKE_COMMAND) -P CMakeFiles/__idf_freertos.dir/cmake_clean_target.cmake
	cd /build/build/esp-idf/freertos && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/__idf_freertos.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
esp-idf/freertos/CMakeFiles/__idf_freertos.dir/build: esp-idf/freertos/libfreertos.a

.PHONY : esp-idf/freertos/CMakeFiles/__idf_freertos.dir/build

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/requires: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/stream_buffer.c.obj.requires
esp-idf/freertos/CMakeFiles/__idf_freertos.dir/requires: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/event_groups.c.obj.requires
esp-idf/freertos/CMakeFiles/__idf_freertos.dir/requires: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/tasks.c.obj.requires
esp-idf/freertos/CMakeFiles/__idf_freertos.dir/requires: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/queue.c.obj.requires
esp-idf/freertos/CMakeFiles/__idf_freertos.dir/requires: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/list.c.obj.requires
esp-idf/freertos/CMakeFiles/__idf_freertos.dir/requires: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/freertos/timers.c.obj.requires
esp-idf/freertos/CMakeFiles/__idf_freertos.dir/requires: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/freertos_hooks.c.obj.requires
esp-idf/freertos/CMakeFiles/__idf_freertos.dir/requires: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/impure.c.obj.requires
esp-idf/freertos/CMakeFiles/__idf_freertos.dir/requires: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/port.c.obj.requires
esp-idf/freertos/CMakeFiles/__idf_freertos.dir/requires: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/panic.c.obj.requires
esp-idf/freertos/CMakeFiles/__idf_freertos.dir/requires: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/xtensa_vectors.S.obj.requires
esp-idf/freertos/CMakeFiles/__idf_freertos.dir/requires: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/os_cpu_a.S.obj.requires
esp-idf/freertos/CMakeFiles/__idf_freertos.dir/requires: esp-idf/freertos/CMakeFiles/__idf_freertos.dir/port/esp8266/xtensa_context.S.obj.requires

.PHONY : esp-idf/freertos/CMakeFiles/__idf_freertos.dir/requires

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/clean:
	cd /build/build/esp-idf/freertos && $(CMAKE_COMMAND) -P CMakeFiles/__idf_freertos.dir/cmake_clean.cmake
.PHONY : esp-idf/freertos/CMakeFiles/__idf_freertos.dir/clean

esp-idf/freertos/CMakeFiles/__idf_freertos.dir/depend:
	cd /build/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /build /tools/ESP8266_RTOS_SDK/components/freertos /build/build /build/build/esp-idf/freertos /build/build/esp-idf/freertos/CMakeFiles/__idf_freertos.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : esp-idf/freertos/CMakeFiles/__idf_freertos.dir/depend

