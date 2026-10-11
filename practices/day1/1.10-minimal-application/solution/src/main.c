#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define APP_LIMIT 5
#define APP_INTERVAL_MS 250

enum application_state {
	STARTUP,
	ACTIVE,
	LIMIT_REACHED,
};

static const char *state_name(enum application_state state)
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
	enum application_state state = STARTUP;
	int count = 0;

	printk("Device starting\n");
	state = ACTIVE;
	while (1) {
		++count;
		if (count >= APP_LIMIT) {
			state = LIMIT_REACHED;
		}
		printk("count=%d state=%s\n", count, state_name(state));
		k_msleep(APP_INTERVAL_MS);
	}

	return 0;
}
