cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(day3_input_channel)
target_include_directories(app PRIVATE include)
target_sources(app PRIVATE src/main.c src/event_channels.c src/event_subscriber.c
			   src/event_input_listener.c)
