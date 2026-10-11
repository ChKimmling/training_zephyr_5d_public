#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define DEVICE_NODE DT_ALIAS(training_device)
#define DEVICE_INTERVAL_MS DT_PROP(DEVICE_NODE, interval_ms)
#define DEVICE_LIMIT DT_PROP(DEVICE_NODE, limit)

int main(void)
{
	printk("device=%s interval=%d ms limit=%d\n",
	       DT_PROP(DEVICE_NODE, device_name),
	       DEVICE_INTERVAL_MS,
	       DEVICE_LIMIT);
	return 0;
}
