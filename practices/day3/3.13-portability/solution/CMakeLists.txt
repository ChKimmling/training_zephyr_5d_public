cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(day3_portability_variant_b)
set(day3_app "${CMAKE_CURRENT_LIST_DIR}/../../3.12-event-service/solution")
if(CONFIG_BOARD_NRF52840DONGLE_NRF52840)
	set(stimulus_source "${CMAKE_CURRENT_LIST_DIR}/src/stimulus_nrf52840dongle.c")
else()
	set(stimulus_source "${day3_app}/src/stimulus.c")
endif()
target_include_directories(app PRIVATE "${day3_app}/include")
target_sources(app PRIVATE
	"${day3_app}/src/main.c"
	"${day3_app}/src/event_channels.c"
	"${day3_app}/src/event_input.c"
	"${day3_app}/src/event_service.c"
	"${stimulus_source}"
	"${day3_app}/src/stimulus_subscriber.c"
	"${day3_app}/src/event_input_listener.c"
	"${day3_app}/src/event_shell.c")
