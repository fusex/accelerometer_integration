cmake_minimum_required (VERSION 3.1)
# Name our project
project (get_sensor_data)

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS "-Wall -Wextra -O3")
# Add all the *.cpp files in our source directory to our executable output
FILE(GLOB SRC_FILES src/*.cpp)
add_executable(get_sensor_data ${SRC_FILES})
