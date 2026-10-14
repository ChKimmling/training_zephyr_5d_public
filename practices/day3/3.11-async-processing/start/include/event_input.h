#ifndef EVENT_INPUT_H
#define EVENT_INPUT_H

#include <stdbool.h>
#include <stdint.h>

enum event_input_source {
	EVENT_SOURCE_SHELL,
	EVENT_SOURCE_EMULATED,
	EVENT_SOURCE_GPIO,
};

struct event_input {
	enum event_input_source source;
	uint32_t sequence;
	bool active;
	int64_t timestamp;
};

#endif
