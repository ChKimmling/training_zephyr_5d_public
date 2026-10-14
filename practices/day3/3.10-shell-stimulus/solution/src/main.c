#include <errno.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "event_channels.h"
#include "event_service.h"
#include "stimulus.h"

LOG_MODULE_REGISTER(day3_shell_stimulus, LOG_LEVEL_INF);

static const struct gpio_dt_spec input =
	GPIO_DT_SPEC_GET(DT_ALIAS(event_input), gpios);
static struct gpio_callback input_callback;
static uint32_t input_sequence;

static void input_changed(const struct device *port, struct gpio_callback *callback,
			  gpio_port_pins_t pins)
{
	ARG_UNUSED(port);
	ARG_UNUSED(callback);
	if ((pins & BIT(input.pin)) != 0U) {
		int active = gpio_pin_get_dt(&input);

		if (active <= 0) {
			return;
		}
		struct event_input event = {
			.source = EVENT_SOURCE_EMULATED,
			.sequence = ++input_sequence,
			.active = true,
			.timestamp = k_uptime_get(),
		};
		int ret = zbus_chan_pub(&event_input_chan, &event, K_NO_WAIT);

		if (ret == 0) {
			event_service_record_input_published();
		} else {
			event_service_record_publish_failure(ret);
			LOG_ERR("input publish failed: %d", ret);
		}
	}
}

int main(void)
{
	int ret = event_service_init();

	if (ret != 0) {
		LOG_ERR("Event service initialization failed: %d", ret);
		return ret;
	}
	if (!gpio_is_ready_dt(&input)) {
		LOG_ERR("Event input device is not ready");
		return -ENODEV;
	}
	ret = gpio_pin_configure_dt(&input, GPIO_INPUT);
	if (ret != 0) {
		LOG_ERR("Cannot configure event input: %d", ret);
		return ret;
	}
	gpio_init_callback(&input_callback, input_changed, BIT(input.pin));
	ret = gpio_add_callback(input.port, &input_callback);
	if (ret != 0) {
		LOG_ERR("Cannot add GPIO callback: %d", ret);
		return ret;
	}
	ret = stimulus_init(&input);
	if (ret != 0) {
		LOG_ERR("Cannot initialize stimulus: %d", ret);
		return ret;
	}
	ret = gpio_pin_interrupt_configure_dt(&input, GPIO_INT_EDGE_BOTH);
	if (ret != 0) {
		LOG_ERR("Cannot enable GPIO edges: %d", ret);
		return ret;
	}
	LOG_INF("Try: event status, event stats, event input 1, event pulse, event burst 3 200");
	return 0;
}
