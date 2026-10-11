#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define ITERATIONS 1000
#define WORKER_STACK_SIZE 1024
#define SIGNAL_COMPLETION 1

static volatile uint32_t shared_counter;
K_MUTEX_DEFINE(counter_mutex);
K_SEM_DEFINE(worker_finished, 0, 2);

static void increment_worker(void *p1, void *p2, void *p3)
{
	const char *name = p1;
	int ret;

	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	for (int i = 0; i < ITERATIONS; ++i) {
		ret = k_mutex_lock(&counter_mutex, K_SECONDS(1));
		if (ret != 0) {
			printk("%s: mutex lock failed: %d\n", name, ret);
			goto finished;
		}

		uint32_t current = shared_counter;

		k_yield();
		shared_counter = current + 1;

		ret = k_mutex_unlock(&counter_mutex);
		if (ret != 0) {
			printk("%s: mutex unlock failed: %d\n", name, ret);
			goto finished;
		}
	}

	printk("%s finished\n", name);

finished:
	if (SIGNAL_COMPLETION) {
		k_sem_give(&worker_finished);
	}
}

K_THREAD_DEFINE(worker_one, WORKER_STACK_SIZE, increment_worker, "worker-one", NULL, NULL,
		K_PRIO_PREEMPT(1), 0, 0);
K_THREAD_DEFINE(worker_two, WORKER_STACK_SIZE, increment_worker, "worker-two", NULL, NULL,
		K_PRIO_PREEMPT(1), 0, 0);

int main(void)
{
	int completed = 0;

	for (int i = 0; i < 2; ++i) {
		int ret = k_sem_take(&worker_finished, K_SECONDS(3));

		if (ret != 0) {
			printk("completion timeout: %d\n", ret);
			continue;
		}
		++completed;
	}

	printk("workers_completed=%d expected=%u actual=%u\n", completed,
	       2U * ITERATIONS, shared_counter);
	return 0;
}
