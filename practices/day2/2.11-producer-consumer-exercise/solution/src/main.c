#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>
#include <zephyr/sys/printk.h>

#define WORKER_STACK_SIZE 1024
#define SAMPLE_COUNT 40
#define QUEUE_CAPACITY 4
#define PRODUCER_INTERVAL_MS 40
#define CONSUMER_WORK_MS 150

struct sample {
	uint32_t sequence;
	int32_t value;
	int64_t produced_at_ms;
};

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
		int ret;

		atomic_inc(&produced_count);
		ret = k_msgq_put(&sample_queue, &item, K_NO_WAIT);
		if (ret == 0) {
			atomic_inc(&accepted_count);
			update_queue_high_water();
		} else if (ret == -ENOMSG) {
			atomic_inc(&dropped_count);
			printk("queue full: dropped sample %u\n", sequence);
		} else {
			printk("queue put failed for sample %u: %d\n", sequence, ret);
			atomic_inc(&dropped_count);
		}

		k_msleep(PRODUCER_INTERVAL_MS);
	}

	atomic_set(&producer_finished, 1);
}

static void consumer(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	while (true) {
		struct sample item;
		int ret = k_msgq_get(&sample_queue, &item, K_MSEC(250));

		if (ret == 0) {
			printk("consume seq=%u value=%d age=%lld ms\n",
			       item.sequence, item.value,
			       (long long)(k_uptime_get() - item.produced_at_ms));
			k_msleep(CONSUMER_WORK_MS);
			atomic_inc(&processed_count);
		} else if (ret == -EAGAIN) {
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

	printk("produced=%d accepted=%d processed=%d dropped=%d timeouts=%d high_water=%d/%d\n",
	       atomic_get(&produced_count), atomic_get(&accepted_count),
	       atomic_get(&processed_count), atomic_get(&dropped_count),
	       atomic_get(&timeout_count), atomic_get(&queue_high_water), QUEUE_CAPACITY);
	return 0;
}
