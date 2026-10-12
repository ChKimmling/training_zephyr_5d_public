#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "event_service.h"

int main(void)
{
	int ret = event_service_init();

	if (ret != 0) {
		printk("event service init failed: %d\n", ret);
		return ret;
	}
	printk("Event shell ready. Try: event status, event stats, event reset\n");
	return 0;
}
