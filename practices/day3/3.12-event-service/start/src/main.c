#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

int event_service_init(void);
int event_input_init(void);

int main(void)
{
	int ret = event_service_init();

	if (ret == 0) {
		ret = event_input_init();
	}
	if (ret != 0) {
		printk("event service initialization failed: %d\n", ret);
		return ret;
	}
	printk("Integrated event service scaffold; finish queue and stimulus APIs\n");
	return 0;
}
