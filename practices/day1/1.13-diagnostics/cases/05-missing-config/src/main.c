#include <zephyr/sys/printk.h>

#if CONFIG_TRAINING_LIMIT < 1
#error "Set CONFIG_TRAINING_LIMIT to a positive value in prj.conf"
#endif

int main(void)
{
	printk("limit=%d\n", CONFIG_TRAINING_LIMIT);
	return 0;
}
