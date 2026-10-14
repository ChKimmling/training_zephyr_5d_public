#include <errno.h>
#include <zephyr/kernel.h>

#include "event_service.h"
#include "stimulus.h"

static struct gpio_dt_spec input;
static bool initialized;

int stimulus_init(const struct gpio_dt_spec *gpio)
{
	if (gpio == NULL || !gpio_is_ready_dt(gpio)) {
		return -ENODEV;
	}
	input = *gpio;
	initialized = true;
	return 0;
}

int stimulus_set_input(bool active)
{
	ARG_UNUSED(active);
	return initialized ? -ENOTSUP : -EAGAIN;
}

int stimulus_start_pulse(uint32_t duration_ms)
{
	ARG_UNUSED(duration_ms);
	return initialized ? -ENOTSUP : -EAGAIN;
}

int stimulus_start_burst(uint32_t count, uint32_t gap_ms)
{
	ARG_UNUSED(count);
	ARG_UNUSED(gap_ms);
	return initialized ? -ENOTSUP : -EAGAIN;
}

int stimulus_cancel(void)
{
	return initialized ? 0 : -EAGAIN;
}

void stimulus_get_status(bool *active, const char **mode, uint32_t *remaining)
{
	int level = initialized ? gpio_pin_get_dt(&input) : -EAGAIN;

	if (active != NULL) {
		*active = level > 0;
		if (level < 0) {
			event_service_set_error(level);
		}
	}
	if (mode != NULL) {
		*mode = "hardware";
	}
	if (remaining != NULL) {
		*remaining = 0U;
	}
}
