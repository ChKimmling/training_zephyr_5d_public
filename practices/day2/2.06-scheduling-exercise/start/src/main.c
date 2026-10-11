#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define WORKER_STACK_SIZE 1024
#define EVENT_COUNT 8

static void worker(void *p1, void *p2, void *p3)
{
	const char *name = p1;

	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	for (int event = 1; event <= EVENT_COUNT; ++event) {
		printk("%s event=%d\n", name, event);
		k_msleep(100);
	}
}

K_THREAD_DEFINE(worker_alpha, WORKER_STACK_SIZE, worker, "alpha", NULL, NULL,
		K_PRIO_PREEMPT(1), 0, K_MSEC(100));
K_THREAD_DEFINE(worker_bravo, WORKER_STACK_SIZE, worker, "bravo", NULL, NULL,
		K_PRIO_PREEMPT(2), 0, K_MSEC(100));
K_THREAD_DEFINE(worker_charlie, WORKER_STACK_SIZE, worker, "charlie", NULL, NULL,
		K_PRIO_PREEMPT(3), 0, K_MSEC(100));

int main(void)
{
	printk("Scheduling experiment started\n");
	return 0;
}
