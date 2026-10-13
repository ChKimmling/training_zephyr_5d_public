#ifndef EVENT_INPUT_H
#define EVENT_INPUT_H

#include <stdbool.h>
#include <zephyr/drivers/gpio.h>

int event_input_init(void);
int event_input_enable(void);
const struct gpio_dt_spec *event_input_spec(void);
int event_input_get_level(bool *active);

#endif
