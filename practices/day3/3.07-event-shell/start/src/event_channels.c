#include "event_channels.h"

/* TODO: Attach the message subscriber to the command channel. */
ZBUS_CHAN_DEFINE(event_cmd_chan, struct event_command, NULL, NULL,
		 ZBUS_OBSERVERS_EMPTY,
		 ZBUS_MSG_INIT(.type = EVENT_CMD_START));
