#include <errno.h>
#include <zephyr/sys/printk.h>

int main(void)
{
	/* Info: Der Event Service wurde gestartet. */
	printk("Event Service started\n");

	/* Debug: interner Zustand beim Start. */
	printk("Input state initialized: active=%d\n", 0);

	/* Warnung: Die optionale Ausgabe fehlt, der Start kann aber fortgesetzt werden. */
	printk("Optional status output is not configured; continuing\n");

	/* Fehler: Beispiel für eine abgewiesene Anforderung, kein Fehler beim Anwendungsstart. */
	printk("Example error path: invalid event request rejected (%d)\n", -EINVAL);

	return 0;
}
