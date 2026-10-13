#include <errno.h>
#include <zephyr/kernel.h>

#include "stimulus.h"

static struct stimulus_status current_status = {
	.mode = "idle",
	.last_error = -ENOSYS,
};

int stimulus_init(const struct gpio_dt_spec *input)
{
	if (input == NULL || !gpio_is_ready_dt(input)) {
		return -ENODEV;
	}
	/* TODO: Store the GPIO spec and configure a known inactive emulator level. */
	current_status = (struct stimulus_status){
		.input_active = false,
		.mode = "idle",
		.remaining = 0U,
		.last_error = -ENOSYS,
	};
	return 0;
}

int stimulus_set_input(bool active)
{
	/* TODO: Set the emulated GPIO level; reject manual changes while busy. */
	ARG_UNUSED(active);
	return -ENOSYS;
}

int stimulus_start_pulse(uint32_t duration_ms)
{
	/* TODO: Validate duration and schedule an active/inactive pulse without blocking. */
	ARG_UNUSED(duration_ms);
	return -ENOSYS;
}

int stimulus_start_burst(uint32_t count, uint32_t gap_ms)
{
	/* TODO: Schedule exactly count pulses and reject overlapping sequences. */
	ARG_UNUSED(count);
	ARG_UNUSED(gap_ms);
	return -ENOSYS;
}

int stimulus_cancel(void)
{
	/* TODO: Cancel scheduled work and guarantee the inactive end level. */
	return -ENOSYS;
}

void stimulus_get_status(struct stimulus_status *status)
{
	if (status != NULL) {
		/* TODO: Return a synchronized snapshot of the stimulus state. */
		*status = current_status;
	}
}
