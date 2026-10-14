#include "event_channels.h"

ZBUS_CHAN_DEFINE(event_cmd_chan, struct event_command, NULL, NULL,
		 ZBUS_OBSERVERS_EMPTY, ZBUS_MSG_INIT(.type = EVENT_CMD_START));
ZBUS_CHAN_DEFINE(event_input_chan, struct event_input, NULL, NULL,
		 ZBUS_OBSERVERS_EMPTY, ZBUS_MSG_INIT(.source = EVENT_SOURCE_SHELL));
