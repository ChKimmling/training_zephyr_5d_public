#include "event_channels.h"

/* TODO: Bind the command subscriber and input listener to their channels. */
ZBUS_CHAN_DEFINE(event_cmd_chan, struct event_command, NULL, NULL,
		 ZBUS_OBSERVERS_EMPTY,
		 ZBUS_MSG_INIT(.type = EVENT_CMD_RESET));
ZBUS_CHAN_DEFINE(event_input_chan, struct event_input, NULL, NULL,
		 ZBUS_OBSERVERS_EMPTY,
		 ZBUS_MSG_INIT(.source = EVENT_SOURCE_EMULATED));
