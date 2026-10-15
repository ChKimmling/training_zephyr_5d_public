# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(day5_5_13_power_management)

target_sources(app PRIVATE src/main.c src/training_sensor.c)
