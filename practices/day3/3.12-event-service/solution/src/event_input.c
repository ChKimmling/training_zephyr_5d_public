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
		event_service_set_input_ready(false);
		return -ENODEV;
	}
	ret = gpio_pin_configure_dt(&input, GPIO_INPUT);
	if (ret == 0) {
		gpio_init_callback(&input_callback, input_changed, BIT(input.pin));
		ret = gpio_add_callback(input.port, &input_callback);
	}
	if (ret == 0) {
		event_service_set_input_ready(true);
	} else {
		event_service_set_input_ready(false);
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
