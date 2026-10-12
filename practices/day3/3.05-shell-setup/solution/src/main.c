#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

int main(void)
{
	printk("Shell ready. Try: help, device list, kernel uptime, kernel thread list, gpio\n");
	return 0;
}
