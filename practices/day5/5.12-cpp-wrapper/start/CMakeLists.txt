cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(day5_cpp_wrapper_start)

target_include_directories(app PRIVATE ../common/include src)
target_sources(app PRIVATE
  ../common/src/sensor_api.c
  src/main.cpp
  src/sensor.cpp
)
