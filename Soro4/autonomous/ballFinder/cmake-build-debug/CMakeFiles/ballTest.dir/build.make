# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.12

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
CMAKE_COMMAND = /home/zen/Downloads/CLion-2018.2.5/clion-2018.2.5/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/zen/Downloads/CLion-2018.2.5/clion-2018.2.5/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/zen/CLionProjects/ballFinderMK1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/zen/CLionProjects/ballFinderMK1/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/ballTest.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/ballTest.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ballTest.dir/flags.make

CMakeFiles/ballTest.dir/BallFinder.cpp.o: CMakeFiles/ballTest.dir/flags.make
CMakeFiles/ballTest.dir/BallFinder.cpp.o: ../BallFinder.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zen/CLionProjects/ballFinderMK1/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/ballTest.dir/BallFinder.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ballTest.dir/BallFinder.cpp.o -c /home/zen/CLionProjects/ballFinderMK1/BallFinder.cpp

CMakeFiles/ballTest.dir/BallFinder.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ballTest.dir/BallFinder.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/zen/CLionProjects/ballFinderMK1/BallFinder.cpp > CMakeFiles/ballTest.dir/BallFinder.cpp.i

CMakeFiles/ballTest.dir/BallFinder.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ballTest.dir/BallFinder.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/zen/CLionProjects/ballFinderMK1/BallFinder.cpp -o CMakeFiles/ballTest.dir/BallFinder.cpp.s

CMakeFiles/ballTest.dir/Tests.cpp.o: CMakeFiles/ballTest.dir/flags.make
CMakeFiles/ballTest.dir/Tests.cpp.o: ../Tests.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zen/CLionProjects/ballFinderMK1/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/ballTest.dir/Tests.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ballTest.dir/Tests.cpp.o -c /home/zen/CLionProjects/ballFinderMK1/Tests.cpp

CMakeFiles/ballTest.dir/Tests.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ballTest.dir/Tests.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/zen/CLionProjects/ballFinderMK1/Tests.cpp > CMakeFiles/ballTest.dir/Tests.cpp.i

CMakeFiles/ballTest.dir/Tests.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ballTest.dir/Tests.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/zen/CLionProjects/ballFinderMK1/Tests.cpp -o CMakeFiles/ballTest.dir/Tests.cpp.s

# Object files for target ballTest
ballTest_OBJECTS = \
"CMakeFiles/ballTest.dir/BallFinder.cpp.o" \
"CMakeFiles/ballTest.dir/Tests.cpp.o"

# External object files for target ballTest
ballTest_EXTERNAL_OBJECTS =

ballTest: CMakeFiles/ballTest.dir/BallFinder.cpp.o
ballTest: CMakeFiles/ballTest.dir/Tests.cpp.o
ballTest: CMakeFiles/ballTest.dir/build.make
ballTest: /usr/local/lib/libopencv_dnn.so.4.0.0
ballTest: /usr/local/lib/libopencv_gapi.so.4.0.0
ballTest: /usr/local/lib/libopencv_ml.so.4.0.0
ballTest: /usr/local/lib/libopencv_objdetect.so.4.0.0
ballTest: /usr/local/lib/libopencv_photo.so.4.0.0
ballTest: /usr/local/lib/libopencv_stitching.so.4.0.0
ballTest: /usr/local/lib/libopencv_video.so.4.0.0
ballTest: /usr/local/lib/libgtest.so
ballTest: /usr/local/lib/libopencv_calib3d.so.4.0.0
ballTest: /usr/local/lib/libopencv_features2d.so.4.0.0
ballTest: /usr/local/lib/libopencv_flann.so.4.0.0
ballTest: /usr/local/lib/libopencv_highgui.so.4.0.0
ballTest: /usr/local/lib/libopencv_videoio.so.4.0.0
ballTest: /usr/local/lib/libopencv_imgcodecs.so.4.0.0
ballTest: /usr/local/lib/libopencv_imgproc.so.4.0.0
ballTest: /usr/local/lib/libopencv_core.so.4.0.0
ballTest: CMakeFiles/ballTest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/zen/CLionProjects/ballFinderMK1/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable ballTest"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ballTest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ballTest.dir/build: ballTest

.PHONY : CMakeFiles/ballTest.dir/build

CMakeFiles/ballTest.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ballTest.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ballTest.dir/clean

CMakeFiles/ballTest.dir/depend:
	cd /home/zen/CLionProjects/ballFinderMK1/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/zen/CLionProjects/ballFinderMK1 /home/zen/CLionProjects/ballFinderMK1 /home/zen/CLionProjects/ballFinderMK1/cmake-build-debug /home/zen/CLionProjects/ballFinderMK1/cmake-build-debug /home/zen/CLionProjects/ballFinderMK1/cmake-build-debug/CMakeFiles/ballTest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ballTest.dir/depend
