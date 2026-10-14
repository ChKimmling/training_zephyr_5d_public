#include "status_provider.h"

#include <errno.h>
#include <zephyr/sys/util.h>
#include "app_state.h"

int status_format(char *buffer, size_t size)
{
	struct app_state_snapshot state;

	/* TODO(4.12-02): app_state_get() verwenden, damit alle Antwortfelder
	 * aus genau einem konsistenten Snapshot stammen. */
	ARG_UNUSED(state);
	/* TODO(4.12-03): Sequenz, Achsen, Qualität, Alter, Drops,
	 * Sensorfehler und Connectivity in stabiler Reihenfolge formatieren;
	 * Überlauf als -ENOSPC melden. */
	ARG_UNUSED(buffer);
	ARG_UNUSED(size);
	return -EAGAIN;
}
