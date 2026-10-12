#include <zephyr/shell/shell.h>

#include "event_service.h"

static int cmd_status(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	shell_print(sh, "GPIO stimulus scaffold; implement device and sequence status");
	return 0;
}

static int cmd_stats(const struct shell *sh, size_t argc, char **argv)
{
	struct event_stats stats;

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	event_service_get_stats(&stats);
	shell_print(sh, "raw=%u processed=%u", stats.raw_count, stats.processed_count);
	return 0;
}

static int cmd_reset(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	event_service_reset_stats();
	shell_print(sh, "statistics reset");
	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(event_commands,
	SHELL_CMD_ARG(status, NULL, "Show stimulus status", cmd_status, 2, 0),
	SHELL_CMD_ARG(stats, NULL, "Show event counters", cmd_stats, 2, 0),
	SHELL_CMD_ARG(reset, NULL, "Reset event counters", cmd_reset, 2, 0),
	SHELL_SUBCMD_SET_END
);
SHELL_CMD_REGISTER(event, &event_commands, "GPIO stimulus exercise", NULL);
