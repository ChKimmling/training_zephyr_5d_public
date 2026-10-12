#ifndef EVENT_INPUT_H
#define EVENT_INPUT_H

#include <zephyr/drivers/gpio.h>

int event_input_init(void);
const struct gpio_dt_spec *event_input_spec(void);

#endif
