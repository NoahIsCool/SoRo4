cmake_minimum_required(VERSION 3.12)
project(ballFinderMK1)

set(CMAKE_CXX_STANDARD 17)
find_package( OpenCV REQUIRED )
find_package (Threads REQUIRED)


find_package(GTest REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS}  ${GTEST_INCLUDE_DIRS} ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
add_executable(ballTest BallFinder.cpp BallFinder.h Tests.cpp )
target_link_libraries(ballTest  ${OpenCV_LIBS} ${GTEST_LIBRARIES} pthread)

set(SOURCE_FILES main.cpp BallFinder.cpp BallFinder.h)
add_executable(ballFinderMK1 ${SOURCE_FILES})
include_directories(${OpenCV_INCLUDE_DIRS} )
target_link_libraries( ballFinderMK1 ${OpenCV_LIBS} pthread)