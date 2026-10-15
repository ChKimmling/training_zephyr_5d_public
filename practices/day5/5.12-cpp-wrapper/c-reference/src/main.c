#include <errno.h>
#include <stddef.h>

#include <zephyr/sys/printk.h>

#include "sensor_api.h"

static const char *read_status(int rc)
{
	if (rc == 0) {
		return "ok";
	}
	if (rc == -EBUSY) {
		return "busy";
	}
	if (rc == -EACCES) {
		return "not_ready";
	}
	return "io_error";
}

int main(void)
{
	struct sensor_api_handle handle = {0};
	const int init_rc = sensor_api_open(&handle, SENSOR_API_CHANNEL_ACCEL);

	printk("C_SENSOR init=%d ready=%d attempts=5\n", init_rc, handle.opened);
	if (init_rc == 0) {
		for (size_t attempt = 0; attempt < 5U; ++attempt) {
			struct sensor_api_sample sample = {0};
			const int rc = sensor_api_read(&handle, &sample);

			printk("C_SENSOR read=%s sequence=%u value_milli=%d\n", read_status(rc),
			       sample.sequence, sample.value_milli);
		}
	}

	sensor_api_close(&handle);

	struct sensor_api_handle reopened = {0};
	const int reopen_rc = sensor_api_open(&reopened, SENSOR_API_CHANNEL_ACCEL);
	printk("C_SENSOR reopen=%d ready=%d\n", reopen_rc, reopened.opened);
	sensor_api_close(&reopened);
	return 0;
}
