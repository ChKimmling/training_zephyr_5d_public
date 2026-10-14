#include "event_channel.h"

ZBUS_CHAN_DEFINE(event_input_chan, struct event_input, NULL, NULL,
		 ZBUS_OBSERVERS(event_input_listener),
		 ZBUS_MSG_INIT(.source = EVENT_SOURCE_EMULATED));
