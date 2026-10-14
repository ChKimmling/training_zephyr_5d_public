#ifndef EVENT_CHANNEL_H
#define EVENT_CHANNEL_H

#include <zephyr/zbus/zbus.h>

#include "event_input.h"

ZBUS_CHAN_DECLARE(event_input_chan);
ZBUS_OBS_DECLARE(event_input_listener);

#endif
