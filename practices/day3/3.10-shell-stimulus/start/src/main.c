#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "event_service.h"

int main(void)
{
	/*
	 * Shell/Status aus 3.07 sind vorhanden. Ergänze das GPIO-Overlay als
	 * GPIO_DT_SPEC_GET(), konfiguriere den Eingang und registriere einen
	 * Callback. Danach ergänze stimulus-API und asynchrone Sequenzen.
	 */
	int ret = event_service_init();

	printk("GPIO stimulus scaffold ready (init=%d)\n", ret);
	return ret;
}
