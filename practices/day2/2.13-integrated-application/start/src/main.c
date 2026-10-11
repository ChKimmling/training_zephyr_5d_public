#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

struct sample {
	uint32_t sequence;
	int32_t value;
	int64_t timestamp_ms;
};

static void timer_expired(struct k_timer *timer)
{
	ARG_UNUSED(timer);
	/* TODO: Nur Arbeit einreichen, nicht im Timer-Kontext verarbeiten. */
}

K_TIMER_DEFINE(sample_timer, timer_expired, NULL);

int main(void)
{
	printk("Integrated application scaffold\n");
	k_timer_start(&sample_timer, K_MSEC(750), K_MSEC(100));
	return 0;
}
