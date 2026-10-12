#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define ITERATIONS 1000
#define WORKER_STACK_SIZE 1024
#define SIGNAL_COMPLETION 1

static volatile uint32_t shared_counter;
/* TODO: Im zweiten Durchlauf den Read-Modify-Write-Abschnitt damit schützen. */
K_MUTEX_DEFINE(counter_mutex);
K_SEM_DEFINE(worker_finished, 0, 2);

static void increment_shared_counter(void)
{
	/*
	 * TODO: Mutex mit endlichem Timeout sperren und den Rückgabewert prüfen.
	 * Der Yield zwischen Lesen und Schreiben macht das Race sichtbar.
	 * Schütze beide Zugriffe gemeinsam und gib den Mutex danach wieder frei.
	 */
	uint32_t current = shared_counter;

	k_yield();
	shared_counter = current + 1;
}

static void increment_worker(void *p1, void *p2, void *p3)
{
	const char *name = p1;

	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	for (int i = 0; i < ITERATIONS; ++i) {
		increment_shared_counter();
	}

	printk("%s finished\n", name);
	/* TODO: Auch nach einem Fehlerpfad signalisieren, falls SIGNAL_COMPLETION aktiv ist. */
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
	for (int i = 0; i < 2; ++i) {
		/* TODO: Timeout explizit auswerten; SIGNAL_COMPLETION=0 testet diesen Pfad. */
		int ret = k_sem_take(&worker_finished, K_SECONDS(3));

		if (ret != 0) {
			printk("completion timeout: %d\n", ret);
		}
	}

	printk("expected=%u actual=%u\n", 2U * ITERATIONS, shared_counter);
	return 0;
}
