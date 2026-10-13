#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(event_app, CONFIG_EVENT_APP_LOG_LEVEL);

int main(void)
{
	LOG_INF("Event Service started");
	LOG_DBG("Input state initialized: active=%d", 0);
	LOG_WRN("Optional status output is not configured; continuing");
	LOG_ERR("Example error path: invalid event request rejected (%d)", -EINVAL);

	return 0;
}
