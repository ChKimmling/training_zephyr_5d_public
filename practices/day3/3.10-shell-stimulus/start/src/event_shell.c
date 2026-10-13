#include <errno.h>
#include <stdlib.h>
#include <zephyr/shell/shell.h>

#include "event_service.h"
#include "stimulus.h"

static int parse_u32(const struct shell *sh, const char *text, uint32_t min,
		     uint32_t max, uint32_t *value)
{
	const char *digit;
	char *end;
	unsigned long parsed;

	if (text == NULL || text[0] == '\0' || text[0] == '-') {
		shell_error(sh, "expected an unsigned integer");
		return -EINVAL;
	}
	for (digit = text; *digit != '\0'; digit++) {
		if (*digit < '0' || *digit > '9') {
			shell_error(sh, "expected an unsigned integer");
			return -EINVAL;
		}
	}
	errno = 0;
	parsed = strtoul(text, &end, 10);
	if (errno != 0 || end == text || *end != '\0' || parsed < min || parsed > max) {
		shell_error(sh, "value must be in range %u..%u", min, max);
		return -ERANGE;
	}
	*value = (uint32_t)parsed;
	return 0;
}

static int report_ret(const struct shell *sh, const char *action, int ret)
{
	if (ret == 0) {
		shell_print(sh, "%s accepted", action);
		return 0;
	}
	if (ret == -EBUSY) {
		shell_error(sh, "%s rejected: a stimulus sequence is active", action);
	} else if (ret == -ENOSYS) {
		shell_error(sh, "%s not implemented yet; complete the TODOs in stimulus.c", action);
	} else {
		shell_error(sh, "%s failed: %d", action, ret);
	}
	return ret;
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
		return report_ret(sh, "stats", ret);
	}
	shell_print(sh, "raw=%u processed=%u", stats.raw_count, stats.processed_count);
	return 0;
}

static int cmd_reset(const struct shell *sh, size_t argc, char **argv)
{
	int ret;

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	ret = event_service_reset_stats();
	if (ret != 0) {
		return report_ret(sh, "reset", ret);
	}
	shell_print(sh, "statistics reset");
	return 0;
}

static int cmd_input(const struct shell *sh, size_t argc, char **argv)
{
	uint32_t level;
	int ret;

	ARG_UNUSED(argc);
	ret = parse_u32(sh, argv[1], 0U, 1U, &level);
	return ret != 0 ? ret : report_ret(sh, "input", stimulus_set_input(level != 0U));
}

static int cmd_pulse(const struct shell *sh, size_t argc, char **argv)
{
	uint32_t duration = 100U;
	int ret = 0;

	if (argc > 1U) {
		ret = parse_u32(sh, argv[1], 20U, 2000U, &duration);
	}
	return ret != 0 ? ret : report_ret(sh, "pulse", stimulus_start_pulse(duration));
}

static int cmd_burst(const struct shell *sh, size_t argc, char **argv)
{
	uint32_t count;
	uint32_t gap;
	int ret;

	ARG_UNUSED(argc);
	ret = parse_u32(sh, argv[1], 1U, 20U, &count);
	if (ret == 0) {
		ret = parse_u32(sh, argv[2], 20U, 2000U, &gap);
	}
	return ret != 0 ? ret : report_ret(sh, "burst", stimulus_start_burst(count, gap));
}

static int cmd_cancel(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	return report_ret(sh, "cancel", stimulus_cancel());
}

SHELL_STATIC_SUBCMD_SET_CREATE(event_commands,
	SHELL_CMD_ARG(status, NULL, "Show event and stimulus status", cmd_status, 1, 0),
	SHELL_CMD_ARG(stats, NULL, "Show GPIO edge counters", cmd_stats, 1, 0),
	SHELL_CMD_ARG(reset, NULL, "Reset event counters", cmd_reset, 1, 0),
	SHELL_CMD_ARG(input, NULL, "Set emulated input: event input <0|1>", cmd_input, 2, 0),
	SHELL_CMD_ARG(pulse, NULL, "Generate pulse: optional duration_ms", cmd_pulse, 1, 2),
	SHELL_CMD_ARG(burst, NULL, "Generate pulses: count and gap_ms", cmd_burst, 3, 0),
	SHELL_CMD_ARG(cancel, NULL, "Cancel active stimulus", cmd_cancel, 1, 0),
	SHELL_SUBCMD_SET_END
);
SHELL_CMD_REGISTER(event, &event_commands, "GPIO stimulus exercise", NULL);
