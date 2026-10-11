#include <zephyr/devicetree.h>
#include <zephyr/sys/printk.h>

#define DEVICE_NODE DT_ALIAS(training_device)

int main(void)
{
	printk("value=%d\n", DT_PROP(DEVICE_NODE, value));
	return 0;
}
