#ifndef EVENT_CHANNELS_H
#define EVENT_CHANNELS_H

#include <zephyr/zbus/zbus.h>

#include "event_messages.h"

ZBUS_CHAN_DECLARE(event_cmd_chan);
ZBUS_OBS_DECLARE(event_command_subscriber);

#endif
