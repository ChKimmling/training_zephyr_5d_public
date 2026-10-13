#include <errno.h>
#include <zephyr/shell/shell.h>

#include "event_service.h"

static int report_service_error(const struct shell *sh, const char *operation, int ret)
{
	if (ret == -EAGAIN) {
		shell_error(sh, "%s unavailable: event service is not initialized", operation);
	} else {
		shell_error(sh, "%s failed: %d", operation, ret);
	}
	return ret;
}

static int cmd_status(const struct shell *sh, size_t argc, char **argv)
{
	struct event_status status;
	int ret = event_service_get_status(&status);

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	if (ret != 0) {
		return report_service_error(sh, "status", ret);
	}
	shell_print(sh, "initialized=%s input_ready=%s input=%s stimulus=%s error=%d",
		    status.initialized ? "yes" : "no",
		    status.input_ready ? "yes" : "no",
		    status.input_active ? "active" : "idle",
		    status.stimulus_state, status.last_error);
	return 0;
}

static int cmd_stats(const struct shell *sh, size_t argc, char **argv)
{
	struct event_stats stats;
	int ret = event_service_get_stats(&stats);

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	if (ret != 0) {
		return report_service_error(sh, "stats", ret);
	}
	shell_print(sh, "raw=%u processed=%u rejected=%u", stats.raw_count,
		    stats.processed_count, stats.rejected_count);
	return 0;
}

static int cmd_reset(const struct shell *sh, size_t argc, char **argv)
{
	int ret = event_service_reset_stats();

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	if (ret != 0) {
		return report_service_error(sh, "reset", ret);
	}
	shell_print(sh, "statistics reset");
	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(event_commands,
	SHELL_CMD_ARG(status, NULL, "Show event service status", cmd_status, 1, 0),
	SHELL_CMD_ARG(stats, NULL, "Show event statistics", cmd_stats, 1, 0),
	SHELL_CMD_ARG(reset, NULL, "Reset event statistics", cmd_reset, 1, 0),
	SHELL_SUBCMD_SET_END
);
SHELL_CMD_REGISTER(event, &event_commands, "Event service commands", NULL);
