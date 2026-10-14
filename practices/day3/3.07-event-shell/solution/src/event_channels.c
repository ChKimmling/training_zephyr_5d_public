#include "event_channels.h"

ZBUS_CHAN_DEFINE(event_cmd_chan, struct event_command, NULL, NULL,
		 ZBUS_OBSERVERS(event_command_subscriber),
		 ZBUS_MSG_INIT(.type = EVENT_CMD_START));
