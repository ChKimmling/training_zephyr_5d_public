#include <errno.h>
#include <zephyr/logging/log.h>

#include "event_input.h"
#include "event_service.h"
#include "stimulus.h"

LOG_MODULE_REGISTER(event_service_app, LOG_LEVEL_INF);

int main(void)
{
	int ret = event_service_init();

	if (ret == 0) {
		ret = event_input_init();
	}
	if (ret == 0) {
		ret = stimulus_init(event_input_spec());
	}
	if (ret == 0) {
		ret = event_input_enable();
	}
	if (ret != 0) {
		event_service_set_error(ret);
		LOG_ERR("Event service initialization failed: %d", ret);
		return ret;
	}
	LOG_INF("Interactive event service ready. Try: event status, event stats, event pulse 100");
	return 0;
}
