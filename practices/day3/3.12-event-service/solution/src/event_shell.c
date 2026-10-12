#include <errno.h>
#include <stdlib.h>
#include <zephyr/shell/shell.h>

#include "event_service.h"
#include "stimulus.h"

static int parse_u32(const struct shell *sh, const char *text, uint32_t min,
		     uint32_t max, uint32_t *value)
{
	char *end;
	unsigned long parsed;

	if (text == NULL || text[0] == '\0' || text[0] == '-') {
		event_service_reject();
		event_service_set_error(-EINVAL);
		shell_error(sh, "expected an unsigned integer");
		return -EINVAL;
	}
	errno = 0;
	parsed = strtoul(text, &end, 10);
	if (errno != 0 || end == text || *end != '\0' || parsed < min || parsed > max) {
		event_service_reject();
		event_service_set_error(-ERANGE);
		shell_error(sh, "value must be in range %u..%u", min, max);
		return -ERANGE;
	}
	*value = (uint32_t)parsed;
	return 0;
}

static int report_result(const struct shell *sh, const char *name, int ret)
{
	if (ret == 0) {
		shell_print(sh, "%s accepted", name);
		return 0;
	}
	event_service_reject();
	event_service_set_error(ret);
	shell_error(sh, "%s failed: %d", name, ret);
	return ret;
}

static int cmd_status(const struct shell *sh, size_t argc, char **argv)
{
	struct event_status status;
	int ret = event_service_get_status(&status);

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	if (ret != 0) {
		shell_error(sh, "status unavailable: %d", ret);
		return ret;
	}
	stimulus_get_status(&status.input_active, &status.stimulus_state,
			    &status.remaining);
	shell_print(sh, "initialized=%s input_ready=%s input=%s stimulus=%s remaining=%u error=%d",
		    status.initialized ? "yes" : "no", status.input_ready ? "yes" : "no",
		    status.input_active ? "active" : "idle", status.stimulus_state,
		    status.remaining, status.last_error);
	return 0;
}

static int cmd_stats(const struct shell *sh, size_t argc, char **argv)
{
	struct event_stats stats;
	int ret = event_service_get_stats(&stats);

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	if (ret != 0) {
		shell_error(sh, "stats unavailable: %d", ret);
		return ret;
	}
	shell_print(sh, "raw=%u processed=%u rejected=%u lost=%u",
		    stats.raw_count, stats.processed_count,
		    stats.rejected_count, stats.lost_count);
	return 0;
}

static int cmd_reset(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	return report_result(sh, "reset", event_service_reset_stats());
}

static int cmd_input(const struct shell *sh, size_t argc, char **argv)
{
	uint32_t level;
	int ret;

	ARG_UNUSED(argc);
	ret = parse_u32(sh, argv[2], 0U, 1U, &level);
	return ret != 0 ? ret : report_result(sh, "input", stimulus_set_input(level != 0U));
}

static int cmd_pulse(const struct shell *sh, size_t argc, char **argv)
{
	uint32_t duration = 100U;
	int ret = 0;

	if (argc > 2U) {
		ret = parse_u32(sh, argv[2], 20U, 2000U, &duration);
	}
	return ret != 0 ? ret : report_result(sh, "pulse", stimulus_start_pulse(duration));
}

static int cmd_burst(const struct shell *sh, size_t argc, char **argv)
{
	uint32_t count;
	uint32_t gap;
	int ret;

	ARG_UNUSED(argc);
	ret = parse_u32(sh, argv[2], 1U, 20U, &count);
	if (ret == 0) {
		ret = parse_u32(sh, argv[3], 20U, 2000U, &gap);
	}
	return ret != 0 ? ret : report_result(sh, "burst", stimulus_start_burst(count, gap));
}

static int cmd_cancel(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	return report_result(sh, "cancel", stimulus_cancel());
}

SHELL_STATIC_SUBCMD_SET_CREATE(event_commands,
	SHELL_CMD_ARG(status, NULL, "Show service status", cmd_status, 2, 0),
	SHELL_CMD_ARG(stats, NULL, "Show event statistics", cmd_stats, 2, 0),
	SHELL_CMD_ARG(reset, NULL, "Reset statistics", cmd_reset, 2, 0),
	SHELL_CMD_ARG(input, NULL, "Set emulated input: 0 or 1", cmd_input, 3, 0),
	SHELL_CMD_ARG(pulse, NULL, "Start pulse: optional duration in ms", cmd_pulse, 2, 1),
	SHELL_CMD_ARG(burst, NULL, "Start burst: count and gap in ms", cmd_burst, 4, 0),
	SHELL_CMD_ARG(cancel, NULL, "Cancel active stimulus", cmd_cancel, 2, 0),
	SHELL_SUBCMD_SET_END
);
SHELL_CMD_REGISTER(event, &event_commands, "Interactive event service", NULL);
