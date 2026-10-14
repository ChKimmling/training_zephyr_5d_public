#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "event_channels.h"
#include "event_service.h"
#include "stimulus.h"

LOG_MODULE_REGISTER(stimulus_subscriber, LOG_LEVEL_INF);

ZBUS_MSG_SUBSCRIBER_DEFINE(event_stimulus_subscriber);

static void reject_command(int error)
{
	event_service_reject();
	event_service_set_error(error);
}

static void process_command(const struct event_command *command)
{
	int ret = 0;

	switch (command->type) {
	case EVENT_CMD_START:
		event_service_set_running(true);
		event_service_set_error(0);
		break;
	case EVENT_CMD_STOP:
		event_service_set_running(false);
		ret = stimulus_cancel();
		break;
	case EVENT_CMD_RESET:
		ret = event_service_reset_stats();
		break;
	case EVENT_CMD_INPUT:
	case EVENT_CMD_PULSE:
	case EVENT_CMD_BURST:
		if (!event_service_is_running()) {
			ret = -ESHUTDOWN;
			break;
		}
		if (command->type == EVENT_CMD_INPUT) {
			ret = stimulus_set_input(command->value != 0);
		} else if (command->type == EVENT_CMD_PULSE) {
			ret = stimulus_start_pulse(command->duration_ms);
		} else {
			ret = stimulus_start_burst(command->count, command->interval_ms);
		}
		break;
	case EVENT_CMD_CANCEL:
		ret = stimulus_cancel();
		break;
	default:
		ret = -EINVAL;
		break;
	}
	if (ret != 0) {
		reject_command(ret);
		LOG_WRN("command type=%d rejected: %d", command->type, ret);
	} else {
		event_service_set_error(0);
	}
}

static void subscriber_thread(void *p1, void *p2, void *p3)
{
	const struct zbus_channel *chan;
	struct event_command command;

	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);
	while (zbus_sub_wait_msg(&event_stimulus_subscriber, &chan, &command, K_FOREVER) == 0) {
		if (chan != &event_cmd_chan) {
			reject_command(-EINVAL);
			continue;
		}
		event_service_record_command_received();
		LOG_DBG("received command type=%d sequence=%u", command.type, command.sequence);
		process_command(&command);
	}
}

K_THREAD_DEFINE(event_stimulus_subscriber_thread, 2048, subscriber_thread,
		NULL, NULL, NULL, K_PRIO_PREEMPT(3), 0, 0);
