#include <zephyr/sys/printk.h>

#include "counter.h"

void counter_report(void)
{
	printk("Counter module: 1\n");
}
