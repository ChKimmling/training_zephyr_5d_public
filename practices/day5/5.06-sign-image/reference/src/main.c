#include <zephyr/app_version.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

int main(void)
{
	printk("IMAGE_SIGNING payload version=%s target=native_sim\n",
	       APP_VERSION_EXTENDED_STRING);
	return 0;
}
