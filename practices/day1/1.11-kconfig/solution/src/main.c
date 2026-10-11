#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

enum demo_state {
	STARTUP,
	ACTIVE,
	LIMIT_REACHED,
};

static const char *state_name(enum demo_state state)
{
	switch (state) {
	case STARTUP:
		return "STARTUP";
	case ACTIVE:
		return "ACTIVE";
	case LIMIT_REACHED:
		return "LIMIT_REACHED";
	default:
		return "UNKNOWN";
	}
}

int main(void)
{
	enum demo_state state = IS_ENABLED(CONFIG_TRAINING_START_ACTIVE) ? ACTIVE : STARTUP;
	int count = 0;

	printk("Kconfig solution limit=%d interval=%d ms initial-state=%s\n",
	       CONFIG_TRAINING_LIMIT, CONFIG_TRAINING_INTERVAL_MS, state_name(state));
	while (1) {
		++count;
		if (count >= CONFIG_TRAINING_LIMIT) {
			state = LIMIT_REACHED;
		} else if (state == STARTUP) {
			state = ACTIVE;
		}
		printk("sample %d state=%s\n", count, state_name(state));
		if (IS_ENABLED(CONFIG_TRAINING_VERBOSE_OUTPUT)) {
			printk("diagnostic: count=%d\n", count);
		}
		k_msleep(CONFIG_TRAINING_INTERVAL_MS);
	}
	return 0;
}
