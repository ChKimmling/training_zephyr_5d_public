#ifndef STIMULUS_H
#define STIMULUS_H

#include <stdbool.h>
#include <stdint.h>
#include <zephyr/drivers/gpio.h>

struct stimulus_status {
	bool input_active;
	const char *mode;
	uint32_t remaining;
	int last_error;
};

int stimulus_init(const struct gpio_dt_spec *input);
int stimulus_set_input(bool active);
int stimulus_start_pulse(uint32_t duration_ms);
int stimulus_start_burst(uint32_t count, uint32_t gap_ms);
int stimulus_cancel(void);
void stimulus_get_status(struct stimulus_status *status);

#endif
