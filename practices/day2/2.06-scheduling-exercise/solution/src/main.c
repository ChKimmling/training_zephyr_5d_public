#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define WORKER_STACK_SIZE 1024
#define EVENT_COUNT 8
#define WORKER_DELAY_MS 100

struct worker_config {
	const char *name;
	uint32_t work_us;
	bool yield_after_work;
};

static const struct worker_config alpha = {
	.name = "alpha",
	.work_us = 30000,
	.yield_after_work = false,
};

static const struct worker_config bravo = {
	.name = "bravo",
	.work_us = 10000,
	.yield_after_work = true,
};

static const struct worker_config charlie = {
	.name = "charlie",
	.work_us = 5000,
	.yield_after_work = false,
};

static void worker(void *p1, void *p2, void *p3)
{
	const struct worker_config *config = p1;

	ARG_UNUSED(p2);
	ARG_UNUSED(p3);
	k_thread_name_set(k_current_get(), config->name);

	for (int event = 1; event <= EVENT_COUNT; ++event) {
		printk("%s event=%d\n", config->name, event);
		k_busy_wait(config->work_us);

		if (config->yield_after_work) {
			k_yield();
		} else {
			k_msleep(WORKER_DELAY_MS);
		}
	}
}

K_THREAD_DEFINE(worker_alpha, WORKER_STACK_SIZE, worker, (void *)&alpha, NULL, NULL,
		K_PRIO_PREEMPT(1), 0, K_MSEC(100));
K_THREAD_DEFINE(worker_bravo, WORKER_STACK_SIZE, worker, (void *)&bravo, NULL, NULL,
		K_PRIO_PREEMPT(2), 0, K_MSEC(100));
K_THREAD_DEFINE(worker_charlie, WORKER_STACK_SIZE, worker, (void *)&charlie, NULL, NULL,
		K_PRIO_PREEMPT(3), 0, K_MSEC(100));

int main(void)
{
	printk("Scheduling experiment: smaller priority number means higher priority\n");
	return 0;
}
