#include <zephyr/app_version.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

int main(void)
{
	printk("UPDATE_ANALYSIS base_payload=%s target=native_sim\n",
	       APP_VERSION_EXTENDED_STRING);
	return 0;
}
