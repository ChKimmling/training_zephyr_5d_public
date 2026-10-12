#include <errno.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "event_service.h"
#include "stimulus.h"

static const struct gpio_dt_spec input =
	GPIO_DT_SPEC_GET(DT_ALIAS(event_input), gpios);
static struct gpio_callback input_callback;

static void input_changed(const struct device *port, struct gpio_callback *callback,
			  gpio_port_pins_t pins)
{
	ARG_UNUSED(port);
	ARG_UNUSED(callback);
	if ((pins & BIT(input.pin)) != 0U) {
		event_service_record_raw();
	}
}

int main(void)
{
	int ret;

	if (!gpio_is_ready_dt(&input)) {
		printk("event input device is not ready\n");
		return -ENODEV;
	}
	ret = gpio_pin_configure_dt(&input, GPIO_INPUT);
	if (ret != 0) {
		printk("cannot configure event input: %d\n", ret);
		return ret;
	}
	gpio_init_callback(&input_callback, input_changed, BIT(input.pin));
	ret = gpio_add_callback(input.port, &input_callback);
	if (ret != 0) {
		printk("cannot add GPIO callback: %d\n", ret);
		return ret;
	}
	ret = gpio_pin_interrupt_configure_dt(&input, GPIO_INT_EDGE_BOTH);
	if (ret != 0) {
		printk("cannot enable GPIO edges: %d\n", ret);
		return ret;
	}
	event_service_init();
	ret = stimulus_init(&input);
	if (ret != 0) {
		printk("cannot initialize stimulus: %d\n", ret);
		return ret;
	}
	printk("Try: event status, event stats, event input 1, event pulse, event burst 3 200\n");
	return 0;
}
