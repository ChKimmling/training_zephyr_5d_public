#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

int main(void)
{
	int count = 0;

	printk("Kconfig starter limit=%d interval=%d ms\n",
	       CONFIG_TRAINING_LIMIT, CONFIG_TRAINING_INTERVAL_MS);
	while (1) {
		printk("sample %d\n", ++count);
		if (IS_ENABLED(CONFIG_TRAINING_VERBOSE_OUTPUT)) {
			printk("diagnostic: start-active=%d\n",
			       IS_ENABLED(CONFIG_TRAINING_START_ACTIVE));
		}
		k_msleep(CONFIG_TRAINING_INTERVAL_MS);
	}
	return 0;
}
