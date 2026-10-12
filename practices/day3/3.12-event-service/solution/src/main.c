#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "event_input.h"
#include "event_service.h"
#include "stimulus.h"

int main(void)
{
	int ret = event_service_init();

	if (ret == 0) {
		ret = event_input_init();
	}
	if (ret == 0) {
		ret = stimulus_init(event_input_spec());
	}
	if (ret != 0) {
		event_service_set_error(ret);
		printk("event service initialization failed: %d\n", ret);
		return ret;
	}
	printk("Interactive event service ready. Try: event status, event stats, event burst 5 200\n");
	return 0;
}
