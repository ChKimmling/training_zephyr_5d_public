#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define DEVICE_NODE DT_ALIAS(training_device)

int main(void)
{
	printk("device=%s interval=%d ms limit=%d\n",
	       DT_PROP(DEVICE_NODE, device_name),
	       DT_PROP(DEVICE_NODE, interval_ms),
	       DT_PROP(DEVICE_NODE, limit));
	return 0;
}
