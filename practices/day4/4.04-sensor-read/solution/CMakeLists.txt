cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(portable_motion_monitor)

target_include_directories(app PRIVATE include)
target_sources(app PRIVATE
  src/main.c
  src/sensor_source.c
)

