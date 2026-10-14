#include "event_channels.h"

ZBUS_CHAN_DEFINE(event_cmd_chan, struct event_command, NULL, NULL,
		 ZBUS_OBSERVERS(event_stimulus_subscriber),
		 ZBUS_MSG_INIT(.type = EVENT_CMD_RESET));
ZBUS_CHAN_DEFINE(event_input_chan, struct event_input, NULL, NULL,
		 ZBUS_OBSERVERS(event_input_listener),
		 ZBUS_MSG_INIT(.source = EVENT_SOURCE_EMULATED));
