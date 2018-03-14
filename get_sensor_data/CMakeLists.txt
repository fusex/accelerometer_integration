cmake_minimum_required (VERSION 3.1)

# Name our project
project(get_sensor_data)

# CXX options
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -O3")
# libs path
set(WPI_PATH lib/wiringPi/wiringPi)
set(LIB_PATH lib/build)

# libs
include_directories(include ${WPI_PATH})
find_library(WPI_LIB wiringPi HINTS ${LIB_PATH} NO_CMAKE_FIND_ROOT_PATH)
# Alert the user if we do not find it
if(NOT WPI_LIB)
  message(FATAL_ERROR “wiringPi library not found”)
endif()

# Add all the *.cpp files in our source directory to our executable output
FILE(GLOB SRC_FILES src/*.cpp)
add_executable(get_sensor_data ${SRC_FILES})
target_link_libraries(get_sensor_data pthread)
target_link_libraries(get_sensor_data ${WPI_LIB})
