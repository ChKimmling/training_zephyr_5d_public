#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(day3_shell_setup, LOG_LEVEL_INF);

int main(void)
{
	/* Logging remains enabled from exercise 3.02; this app does not yet enable a shell. */
	LOG_INF("Day 3 shell setup application started");
	return 0;
}
