#include <errno.h>
#include <zephyr/shell/shell.h>

#include "event_channels.h"
#include "event_service.h"

static int cmd_status(const struct shell *sh, size_t argc, char **argv)
{
	struct event_status status;
	int ret;

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	ret = event_service_get_status(&status);
	if (ret != 0) {
		shell_error(sh, "status unavailable: %d", ret);
		return ret;
	}
	shell_print(sh, "initialized=%s running=%s last_error=%d",
		    status.initialized ? "yes" : "no", status.running ? "yes" : "no",
		    status.last_error);
	return 0;
}

static int cmd_stats(const struct shell *sh, size_t argc, char **argv)
{
	struct event_stats stats;
	int ret;

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	ret = event_service_get_stats(&stats);
	if (ret != 0) {
		shell_error(sh, "stats unavailable: %d", ret);
		return ret;
	}
	shell_print(sh, "cmd_published=%u cmd_received=%u cmd_rejected=%u input_published=%u input_received=%u processed=%u publish_failed=%u last_error=%d",
		    stats.cmd_published, stats.cmd_received, stats.cmd_rejected,
		    stats.input_published, stats.input_received, stats.processed,
		    stats.publish_failed, stats.last_error);
	return 0;
}

static int publish_command(const struct shell *sh, enum event_command_type type)
{
	struct event_command command = {
		.type = type,
		.sequence = event_service_next_sequence(),
	};
	int ret = zbus_chan_pub(&event_cmd_chan, &command, K_NO_WAIT);

	if (ret != 0) {
		event_service_record_rejected();
		shell_error(sh, "command publish failed: %d", ret);
		return ret;
	}
	event_service_record_published();
	shell_print(sh, "command type=%d published", type);
	return 0;
}

static int cmd_start(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	return publish_command(sh, EVENT_CMD_START);
}

static int cmd_stop(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	return publish_command(sh, EVENT_CMD_STOP);
}

static int cmd_reset(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	return publish_command(sh, EVENT_CMD_RESET);
}

SHELL_STATIC_SUBCMD_SET_CREATE(event_commands,
	SHELL_CMD_ARG(status, NULL, "Show event service status", cmd_status, 1, 0),
	SHELL_CMD_ARG(stats, NULL, "Show command statistics", cmd_stats, 1, 0),
	SHELL_CMD_ARG(start, NULL, "Publish start command", cmd_start, 1, 0),
	SHELL_CMD_ARG(stop, NULL, "Publish stop command", cmd_stop, 1, 0),
	SHELL_CMD_ARG(reset, NULL, "Publish reset command", cmd_reset, 1, 0),
	SHELL_SUBCMD_SET_END
);
SHELL_CMD_REGISTER(event, &event_commands, "zbus event command publisher", NULL);
