# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

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
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/yuvaldvir/CLionProjects/OS_Ex_1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/yuvaldvir/CLionProjects/OS_Ex_1/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/OS_Ex_1.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/OS_Ex_1.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/OS_Ex_1.dir/flags.make

CMakeFiles/OS_Ex_1.dir/os.c.o: CMakeFiles/OS_Ex_1.dir/flags.make
CMakeFiles/OS_Ex_1.dir/os.c.o: ../os.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/yuvaldvir/CLionProjects/OS_Ex_1/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/OS_Ex_1.dir/os.c.o"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/OS_Ex_1.dir/os.c.o   -c /Users/yuvaldvir/CLionProjects/OS_Ex_1/os.c

CMakeFiles/OS_Ex_1.dir/os.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/OS_Ex_1.dir/os.c.i"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/yuvaldvir/CLionProjects/OS_Ex_1/os.c > CMakeFiles/OS_Ex_1.dir/os.c.i

CMakeFiles/OS_Ex_1.dir/os.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/OS_Ex_1.dir/os.c.s"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/yuvaldvir/CLionProjects/OS_Ex_1/os.c -o CMakeFiles/OS_Ex_1.dir/os.c.s

CMakeFiles/OS_Ex_1.dir/pt.c.o: CMakeFiles/OS_Ex_1.dir/flags.make
CMakeFiles/OS_Ex_1.dir/pt.c.o: ../pt.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/yuvaldvir/CLionProjects/OS_Ex_1/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/OS_Ex_1.dir/pt.c.o"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/OS_Ex_1.dir/pt.c.o   -c /Users/yuvaldvir/CLionProjects/OS_Ex_1/pt.c

CMakeFiles/OS_Ex_1.dir/pt.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/OS_Ex_1.dir/pt.c.i"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/yuvaldvir/CLionProjects/OS_Ex_1/pt.c > CMakeFiles/OS_Ex_1.dir/pt.c.i

CMakeFiles/OS_Ex_1.dir/pt.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/OS_Ex_1.dir/pt.c.s"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/yuvaldvir/CLionProjects/OS_Ex_1/pt.c -o CMakeFiles/OS_Ex_1.dir/pt.c.s

# Object files for target OS_Ex_1
OS_Ex_1_OBJECTS = \
"CMakeFiles/OS_Ex_1.dir/os.c.o" \
"CMakeFiles/OS_Ex_1.dir/pt.c.o"

# External object files for target OS_Ex_1
OS_Ex_1_EXTERNAL_OBJECTS =

OS_Ex_1: CMakeFiles/OS_Ex_1.dir/os.c.o
OS_Ex_1: CMakeFiles/OS_Ex_1.dir/pt.c.o
OS_Ex_1: CMakeFiles/OS_Ex_1.dir/build.make
OS_Ex_1: CMakeFiles/OS_Ex_1.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/yuvaldvir/CLionProjects/OS_Ex_1/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable OS_Ex_1"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/OS_Ex_1.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/OS_Ex_1.dir/build: OS_Ex_1

.PHONY : CMakeFiles/OS_Ex_1.dir/build

CMakeFiles/OS_Ex_1.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/OS_Ex_1.dir/cmake_clean.cmake
.PHONY : CMakeFiles/OS_Ex_1.dir/clean

CMakeFiles/OS_Ex_1.dir/depend:
	cd /Users/yuvaldvir/CLionProjects/OS_Ex_1/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/yuvaldvir/CLionProjects/OS_Ex_1 /Users/yuvaldvir/CLionProjects/OS_Ex_1 /Users/yuvaldvir/CLionProjects/OS_Ex_1/cmake-build-debug /Users/yuvaldvir/CLionProjects/OS_Ex_1/cmake-build-debug /Users/yuvaldvir/CLionProjects/OS_Ex_1/cmake-build-debug/CMakeFiles/OS_Ex_1.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/OS_Ex_1.dir/depend
