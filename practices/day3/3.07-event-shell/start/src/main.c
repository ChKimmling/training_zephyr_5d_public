#include <errno.h>
#include <zephyr/logging/log.h>

#include "event_service.h"

LOG_MODULE_REGISTER(day3_event_shell, LOG_LEVEL_INF);

int main(void)
{
	int ret = event_service_init();

	if (ret != 0) {
		LOG_ERR("Event service initialization failed: %d", ret);
		return ret;
	}
	LOG_INF("Event shell ready. Try: event status, event stats, event reset");
	return 0;
}
