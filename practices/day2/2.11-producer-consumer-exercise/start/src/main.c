#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>
#include <zephyr/sys/printk.h>

#define WORKER_STACK_SIZE 1024
#define SAMPLE_COUNT 40
#define QUEUE_CAPACITY 4
#define PRODUCER_INTERVAL_MS 40
#define CONSUMER_WORK_MS 150
#define RECEIVE_TIMEOUT_MS 250

struct sample {
	uint32_t sequence;
	int32_t value;
	int64_t produced_at_ms;
};

/* TODO: Begründe die Kapazität und wähle eine Überlaststrategie. */
K_MSGQ_DEFINE(sample_queue, sizeof(struct sample), QUEUE_CAPACITY, 4);
K_SEM_DEFINE(consumer_finished, 0, 1);

static atomic_t producer_finished;
static atomic_t produced_count;
static atomic_t accepted_count;
static atomic_t processed_count;
static atomic_t dropped_count;
static atomic_t timeout_count;
static atomic_t queue_high_water;

static void update_queue_high_water(void)
{
	int used = (int)k_msgq_num_used_get(&sample_queue);
	int observed = atomic_get(&queue_high_water);

	while (used > observed && !atomic_cas(&queue_high_water, observed, used)) {
		observed = atomic_get(&queue_high_water);
	}
}

static void producer(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	for (uint32_t sequence = 1; sequence <= SAMPLE_COUNT; ++sequence) {
		struct sample item = {
			.sequence = sequence,
			.value = (int32_t)((sequence * 37U) % 1000U),
			.produced_at_ms = k_uptime_get(),
		};

		atomic_inc(&produced_count);
		/*
		 * TODO: k_msgq_put() mit K_NO_WAIT aufrufen. Erfolg, volle Queue
		 * und sonstige Fehler getrennt bilanzieren. Bei Erfolg
		 * update_queue_high_water() aufrufen.
		 */
		ARG_UNUSED(item);
		k_msleep(PRODUCER_INTERVAL_MS);
	}

	/* Der Consumer kann erst nach Producer-Ende und leerer Queue beenden. */
	atomic_set(&producer_finished, 1);
}

static void consumer(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	while (true) {
		struct sample item;
		int ret = k_msgq_get(&sample_queue, &item, K_MSEC(RECEIVE_TIMEOUT_MS));

		if (ret == 0) {
			/*
			 * TODO: Alter ausgeben, Verarbeitung simulieren und
			 * processed_count erhöhen.
			 */
			k_msleep(CONSUMER_WORK_MS);
		} else if (ret == -EAGAIN) {
			/* TODO: Timeout zählen und explizit melden. */
			atomic_inc(&timeout_count);
			printk("consumer receive timeout\n");
		} else {
			printk("queue get failed: %d\n", ret);
			break;
		}

		if (atomic_get(&producer_finished) != 0 &&
		    k_msgq_num_used_get(&sample_queue) == 0U) {
			break;
		}
	}

	k_sem_give(&consumer_finished);
}

K_THREAD_DEFINE(producer_thread, WORKER_STACK_SIZE, producer, NULL, NULL, NULL,
		K_PRIO_PREEMPT(2), 0, 0);
K_THREAD_DEFINE(consumer_thread, WORKER_STACK_SIZE, consumer, NULL, NULL, NULL,
		K_PRIO_PREEMPT(3), 0, 0);

int main(void)
{
	int ret = k_sem_take(&consumer_finished, K_SECONDS(15));

	if (ret != 0) {
		printk("consumer completion timeout: %d\n", ret);
		return ret;
	}

	/*
	 * TODO: Bilanz ausgeben und prüfen: erzeugt = angenommen + verworfen;
	 * angenommen = verarbeitet.
	 */
	update_queue_high_water();
	printk("produced=%d accepted=%d processed=%d dropped=%d timeouts=%d "
	       "high_water=%d/%d\n",
	       atomic_get(&produced_count), atomic_get(&accepted_count),
	       atomic_get(&processed_count), atomic_get(&dropped_count),
	       atomic_get(&timeout_count), atomic_get(&queue_high_water), QUEUE_CAPACITY);
	return 0;
}
