#include <zephyr/shell/shell.h>

#include "event_service.h"

static int cmd_status(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	shell_print(sh, "input callback registered; complete service status API");
	return 0;
}

static int cmd_stats(const struct shell *sh, size_t argc, char **argv)
{
	struct event_stats stats;

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	event_service_get_stats(&stats);
	shell_print(sh, "raw=%u processed=%u rejected=%u lost=%u",
		    stats.raw_count, stats.processed_count,
		    stats.rejected_count, stats.lost_count);
	return 0;
}

static int cmd_reset(const struct shell *sh, size_t argc, char **argv)
{
	int ret;

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	ret = event_service_reset_stats();
	if (ret != 0) {
		shell_error(sh, "reset failed: %d", ret);
		return ret;
	}
	shell_print(sh, "statistics reset");
	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(event_commands,
	SHELL_CMD_ARG(status, NULL, "Show event service status", cmd_status, 2, 0),
	SHELL_CMD_ARG(stats, NULL, "Show event counters", cmd_stats, 2, 0),
	SHELL_CMD_ARG(reset, NULL, "Reset counters", cmd_reset, 2, 0),
	SHELL_SUBCMD_SET_END
);
SHELL_CMD_REGISTER(event, &event_commands, "Interactive event service", NULL);
