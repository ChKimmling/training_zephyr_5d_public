#ifndef EVENT_CHANNELS_H
#define EVENT_CHANNELS_H

#include <zephyr/zbus/zbus.h>

#include "event_messages.h"

ZBUS_CHAN_DECLARE(event_cmd_chan);
ZBUS_CHAN_DECLARE(event_input_chan);
ZBUS_OBS_DECLARE(event_stimulus_subscriber, event_input_listener);

#endif
