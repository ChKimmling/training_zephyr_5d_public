#include <zephyr/sys/printk.h>

#include "status.h"

void status_report(void)
{
	printk("Status: ACTIVE\n");
}
