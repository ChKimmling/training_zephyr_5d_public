#include <errno.h>
#include <zephyr/drivers/gpio/gpio_emul.h>
#include <zephyr/kernel.h>

#include "event_service.h"
#include "stimulus.h"

#define PULSE_WIDTH_MS 100U
#define PULSE_MIN_MS 20U
#define PULSE_MAX_MS 2000U
#define BURST_MAX_COUNT 20U
#define GAP_MIN_MS 20U
#define GAP_MAX_MS 2000U

enum sequence_mode {
	SEQUENCE_IDLE,
	SEQUENCE_PULSE,
	SEQUENCE_BURST,
};

static struct gpio_dt_spec input;
K_MUTEX_DEFINE(stimulus_mutex);
static struct k_work_delayable sequence_work;
static enum sequence_mode mode;
static bool input_active;
static bool phase_active;
static uint32_t remaining;
static uint32_t pulse_duration_ms;
static uint32_t gap_ms;

static int set_level_locked(bool active)
{
	int ret = gpio_emul_input_set_dt(&input, active ? 1 : 0);

	if (ret == 0) {
		input_active = active;
		event_service_set_error(0);
	} else {
		event_service_set_error(ret);
	}
	return ret;
}

static void sequence_handler(struct k_work *work)
{
	int ret = 0;

	ARG_UNUSED(work);
	k_mutex_lock(&stimulus_mutex, K_FOREVER);
	if (mode == SEQUENCE_IDLE) {
		k_mutex_unlock(&stimulus_mutex);
		return;
	}
	if (phase_active) {
		ret = set_level_locked(false);
		phase_active = false;
		if (mode == SEQUENCE_PULSE || remaining <= 1U) {
			mode = SEQUENCE_IDLE;
			remaining = 0U;
		} else {
			remaining--;
			k_work_reschedule(&sequence_work, K_MSEC(gap_ms));
		}
	} else {
		ret = set_level_locked(true);
		if (ret == 0) {
			phase_active = true;
			k_work_reschedule(&sequence_work, K_MSEC(PULSE_WIDTH_MS));
		}
	}
	if (ret != 0) {
		(void)set_level_locked(false);
		mode = SEQUENCE_IDLE;
		remaining = 0U;
		phase_active = false;
	}
	k_mutex_unlock(&stimulus_mutex);
}

int stimulus_init(const struct gpio_dt_spec *gpio)
{
	int ret;

	if (gpio == NULL || !gpio_is_ready_dt(gpio)) {
		return -ENODEV;
	}
	input = *gpio;
	k_work_init_delayable(&sequence_work, sequence_handler);
	k_mutex_lock(&stimulus_mutex, K_FOREVER);
	mode = SEQUENCE_IDLE;
	input_active = false;
	remaining = 0U;
	ret = set_level_locked(false);
	k_mutex_unlock(&stimulus_mutex);
	return ret;
}

int stimulus_set_input(bool active)
{
	int ret;

	k_mutex_lock(&stimulus_mutex, K_FOREVER);
	if (mode != SEQUENCE_IDLE) {
		k_mutex_unlock(&stimulus_mutex);
		return -EBUSY;
	}
	ret = set_level_locked(active);
	k_mutex_unlock(&stimulus_mutex);
	return ret;
}

int stimulus_start_pulse(uint32_t duration_ms)
{
	int ret;

	if (duration_ms < PULSE_MIN_MS || duration_ms > PULSE_MAX_MS) {
		return -ERANGE;
	}
	k_mutex_lock(&stimulus_mutex, K_FOREVER);
	if (mode != SEQUENCE_IDLE) {
		k_mutex_unlock(&stimulus_mutex);
		return -EBUSY;
	}
	mode = SEQUENCE_PULSE;
	remaining = 1U;
	pulse_duration_ms = duration_ms;
	ret = set_level_locked(false);
	if (ret == 0) {
		ret = set_level_locked(true);
	}
	if (ret == 0) {
		phase_active = true;
		k_work_reschedule(&sequence_work, K_MSEC(pulse_duration_ms));
	} else {
		mode = SEQUENCE_IDLE;
		remaining = 0U;
	}
	k_mutex_unlock(&stimulus_mutex);
	return ret;
}

int stimulus_start_burst(uint32_t count, uint32_t gap_interval_ms)
{
	int ret;

	if (count == 0U || count > BURST_MAX_COUNT ||
	    gap_interval_ms < GAP_MIN_MS || gap_interval_ms > GAP_MAX_MS) {
		return -ERANGE;
	}
	k_mutex_lock(&stimulus_mutex, K_FOREVER);
	if (mode != SEQUENCE_IDLE) {
		k_mutex_unlock(&stimulus_mutex);
		return -EBUSY;
	}
	mode = SEQUENCE_BURST;
	remaining = count;
	gap_ms = gap_interval_ms;
	ret = set_level_locked(false);
	if (ret == 0) {
		ret = set_level_locked(true);
	}
	if (ret == 0) {
		phase_active = true;
		k_work_reschedule(&sequence_work, K_MSEC(PULSE_WIDTH_MS));
	} else {
		mode = SEQUENCE_IDLE;
		remaining = 0U;
	}
	k_mutex_unlock(&stimulus_mutex);
	return ret;
}

int stimulus_cancel(void)
{
	int ret;

	(void)k_work_cancel_delayable(&sequence_work);
	k_mutex_lock(&stimulus_mutex, K_FOREVER);
	ret = set_level_locked(false);
	mode = SEQUENCE_IDLE;
	remaining = 0U;
	phase_active = false;
	k_mutex_unlock(&stimulus_mutex);
	return ret;
}

void stimulus_get_status(bool *active, const char **mode_name, uint32_t *left)
{
	k_mutex_lock(&stimulus_mutex, K_FOREVER);
	if (active != NULL) {
		*active = input_active;
	}
	if (left != NULL) {
		*left = remaining;
	}
	if (mode_name != NULL) {
		*mode_name = mode == SEQUENCE_PULSE ? "pulse" :
			     mode == SEQUENCE_BURST ? "burst" : "idle";
	}
	k_mutex_unlock(&stimulus_mutex);
}
