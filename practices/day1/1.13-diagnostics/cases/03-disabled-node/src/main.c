#include <zephyr/devicetree.h>
#include <zephyr/sys/printk.h>

#define DEVICE_NODE DT_ALIAS(training_device)

#if !DT_NODE_HAS_STATUS(DEVICE_NODE, okay)
#error "training_device must have status okay"
#endif

int main(void)
{
	printk("diagnostic case recovered\n");
	return 0;
}
