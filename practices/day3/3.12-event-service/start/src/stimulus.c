#include <errno.h>
#include <zephyr/kernel.h>

#include "stimulus.h"

static struct gpio_dt_spec input;
static bool initialized;

int stimulus_init(const struct gpio_dt_spec *gpio)
{
	if (gpio == NULL || !gpio_is_ready_dt(gpio)) {
		return -ENODEV;
	}
	input = *gpio;
	/* TODO: Set the emulated GPIO to a known inactive level before enabling edges. */
	initialized = true;
	return 0;
}

int stimulus_set_input(bool active)
{
	if (!initialized) {
		return -EAGAIN;
	}
	/* TODO: Set the GPIO-emulator level and update shared status safely. */
	ARG_UNUSED(active);
	return -ENOSYS;
}

int stimulus_start_pulse(uint32_t duration_ms)
{
	/* TODO: Validate duration and schedule a non-blocking pulse. */
	ARG_UNUSED(duration_ms);
	return initialized ? -ENOSYS : -EAGAIN;
}

int stimulus_start_burst(uint32_t count, uint32_t gap_ms)
{
	/* TODO: Schedule count pulses with gap_ms after each pulse ends. */
	ARG_UNUSED(count);
	ARG_UNUSED(gap_ms);
	return initialized ? -ENOSYS : -EAGAIN;
}

int stimulus_cancel(void)
{
	/* TODO: Cancel delayed work and guarantee an inactive GPIO level. */
	return initialized ? -ENOSYS : -EAGAIN;
}

void stimulus_get_status(bool *active, const char **mode, uint32_t *remaining)
{
	if (active != NULL) {
		*active = false;
	}
	if (mode != NULL) {
		*mode = "idle";
	}
	if (remaining != NULL) {
		*remaining = 0U;
	}
	/* TODO: Return a synchronized snapshot of the live stimulus state. */
}
