#include <errno.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

#include "event_channels.h"
#include "event_service.h"

static const struct gpio_dt_spec input =
	GPIO_DT_SPEC_GET(DT_ALIAS(event_input), gpios);
static struct gpio_callback input_callback;

static void input_changed(const struct device *port, struct gpio_callback *callback,
			  gpio_port_pins_t pins)
{
	ARG_UNUSED(port);
	ARG_UNUSED(callback);
	if ((pins & BIT(input.pin)) != 0U) {
		int level = gpio_pin_get_dt(&input);

		if (level < 0) {
			event_service_record_publish_failure(level);
			return;
		}
		if (level == 0) {
			return;
		}
		/* TODO: Publish one logical event for the active edge without blocking. */
		struct event_input event = {
			.source = EVENT_SOURCE_EMULATED,
			.sequence = event_service_next_sequence(),
			.active = true,
			.timestamp = k_uptime_get(),
		};
		int ret = zbus_chan_pub(&event_input_chan, &event, K_NO_WAIT);

		if (ret == 0) {
			event_service_record_input_published();
		} else {
			event_service_record_publish_failure(ret);
		}
	}
}

int event_input_init(void)
{
	int ret;

	if (!gpio_is_ready_dt(&input)) {
		event_service_set_input_ready(false);
		return -ENODEV;
	}
	ret = gpio_pin_configure_dt(&input, GPIO_INPUT);
	if (ret == 0) {
		gpio_init_callback(&input_callback, input_changed, BIT(input.pin));
		ret = gpio_add_callback(input.port, &input_callback);
	}
	if (ret != 0) {
		event_service_set_input_ready(false);
	} else {
		event_service_set_input_ready(true);
	}
	return ret;
}

int event_input_enable(void)
{
	return gpio_pin_interrupt_configure_dt(&input, GPIO_INT_EDGE_BOTH);
}

int event_input_get_level(bool *active)
{
	int value;

	if (active == NULL) {
		return -EINVAL;
	}
	value = gpio_pin_get_dt(&input);
	if (value < 0) {
		return value;
	}
	*active = value != 0;
	return 0;
}

const struct gpio_dt_spec *event_input_spec(void)
{
	return &input;
}
