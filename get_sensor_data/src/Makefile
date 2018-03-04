SRCs := $(wildcard *.cpp)
CC := g++

# All warnings
CPPFLAGS += -Wall -Werror -Wextra

# Use a modern language
CPPFLAGS += -std=gnu++11

# Optimize the code.
CPPFLAGS += -O3

all: get_sensor_data

get_sensor_data: $(SRCs)
