#include <errno.h>
#include <stdlib.h>
#include <zephyr/shell/shell.h>

#include "event_channels.h"
#include "event_service.h"
#include "stimulus.h"

static uint32_t command_sequence;

static int parse_u32(const struct shell *sh, const char *text, uint32_t min,
		     uint32_t max, uint32_t *value)
{
	char *end;
	unsigned long parsed;

	if (text == NULL || text[0] == '\0') {
		event_service_reject(-EINVAL);
		shell_error(sh, "expected an unsigned integer");
		return -EINVAL;
	}
	for (const char *digit = text; *digit != '\0'; digit++) {
		if (*digit < '0' || *digit > '9') {
			event_service_reject(-EINVAL);
			shell_error(sh, "expected an unsigned integer");
			return -EINVAL;
		}
	}
	errno = 0;
	parsed = strtoul(text, &end, 10);
	if (errno != 0 || end == text || *end != '\0' || parsed < min || parsed > max) {
		event_service_reject(-ERANGE);
		shell_error(sh, "value must be in range %u..%u", min, max);
		return -ERANGE;
	}
	*value = (uint32_t)parsed;
	return 0;
}

static int publish_command(const struct shell *sh, struct event_command *command)
{
	command->sequence = ++command_sequence;
	int ret = zbus_chan_pub(&event_cmd_chan, command, K_NO_WAIT);

	if (ret != 0) {
		event_service_record_publish_failure(ret);
		shell_error(sh, "command publish failed: %d", ret);
		return ret;
	}
	event_service_record_command_published();
	shell_print(sh, "command sequence=%u published", command->sequence);
	return 0;
}

static int cmd_status(const struct shell *sh, size_t argc, char **argv)
{
	struct stimulus_status status;

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	stimulus_get_status(&status);
	shell_print(sh, "input=%s stimulus=%s remaining=%u last_error=%d",
		    status.input_active ? "active" : "idle", status.mode,
		    status.remaining, status.last_error);
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

static int cmd_reset(const struct shell *sh, size_t argc, char **argv)
{
	struct event_command command = {.type = EVENT_CMD_RESET};

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	return publish_command(sh, &command);
}

static int cmd_input(const struct shell *sh, size_t argc, char **argv)
{
	struct event_command command = {.type = EVENT_CMD_INPUT};
	uint32_t value;
	int ret;

	ARG_UNUSED(argc);
	ret = parse_u32(sh, argv[1], 0U, 1U, &value);
	if (ret != 0) {
		return ret;
	}
	command.value = (int32_t)value;
	return publish_command(sh, &command);
}

static int cmd_pulse(const struct shell *sh, size_t argc, char **argv)
{
	struct event_command command = {
		.type = EVENT_CMD_PULSE,
		.duration_ms = 100U,
	};
	int ret = 0;

	if (argc > 1U) {
		ret = parse_u32(sh, argv[1], 20U, 2000U, &command.duration_ms);
	}
	return ret != 0 ? ret : publish_command(sh, &command);
}

static int cmd_burst(const struct shell *sh, size_t argc, char **argv)
{
	struct event_command command = {.type = EVENT_CMD_BURST};
	int ret;

	ARG_UNUSED(argc);
	ret = parse_u32(sh, argv[1], 1U, 20U, &command.count);
	if (ret == 0) {
		ret = parse_u32(sh, argv[2], 20U, 2000U, &command.interval_ms);
	}
	return ret != 0 ? ret : publish_command(sh, &command);
}

static int cmd_cancel(const struct shell *sh, size_t argc, char **argv)
{
	struct event_command command = {.type = EVENT_CMD_CANCEL};

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	return publish_command(sh, &command);
}

SHELL_STATIC_SUBCMD_SET_CREATE(event_commands,
	SHELL_CMD_ARG(status, NULL, "Show event and stimulus status", cmd_status, 1, 0),
	SHELL_CMD_ARG(stats, NULL, "Show input event counters", cmd_stats, 1, 0),
	SHELL_CMD_ARG(reset, NULL, "Publish statistics reset", cmd_reset, 1, 0),
	SHELL_CMD_ARG(input, NULL, "Set emulated input: 0 or 1", cmd_input, 2, 0),
	SHELL_CMD_ARG(pulse, NULL, "Generate pulse: optional duration_ms", cmd_pulse, 1, 2),
	SHELL_CMD_ARG(burst, NULL, "Generate pulses: count and gap_ms", cmd_burst, 3, 0),
	SHELL_CMD_ARG(cancel, NULL, "Cancel active stimulus", cmd_cancel, 1, 0),
	SHELL_SUBCMD_SET_END
);
SHELL_CMD_REGISTER(event, &event_commands, "Event stimulus command publisher", NULL);
