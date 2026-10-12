#include <errno.h>
#include <zephyr/drivers/gpio.h>

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
		event_service_record_input_event();
	}
}

int event_input_init(void)
{
	int ret;

	if (!gpio_is_ready_dt(&input)) {
		return -ENODEV;
	}
	ret = gpio_pin_configure_dt(&input, GPIO_INPUT);
	if (ret != 0) {
		return ret;
	}
	gpio_init_callback(&input_callback, input_changed, BIT(input.pin));
	ret = gpio_add_callback(input.port, &input_callback);
	if (ret != 0) {
		return ret;
	}
	return gpio_pin_interrupt_configure_dt(&input, GPIO_INT_EDGE_BOTH);
}
