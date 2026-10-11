#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>
#include <zephyr/sys/printk.h>

#define QUEUE_CAPACITY 8
#define CONSUMER_STACK_SIZE 1024
#define SAMPLE_PERIOD_MS 100
#define CONSUMER_WORK_MS 250

struct sample {
	uint32_t sequence;
	int32_t value;
	int64_t timestamp_ms;
};

K_MSGQ_DEFINE(sample_queue, sizeof(struct sample), QUEUE_CAPACITY, 4);

static atomic_t timer_events;
static atomic_t work_submit_errors;
static atomic_t produced_count;
static atomic_t processed_count;
static atomic_t dropped_count;
static atomic_t timeout_count;
static atomic_t queue_high_water;
static struct k_work sample_work;

static void update_queue_high_water(void)
{
	int used = (int)k_msgq_num_used_get(&sample_queue);
	int observed = atomic_get(&queue_high_water);

	while (used > observed && !atomic_cas(&queue_high_water, observed, used)) {
		observed = atomic_get(&queue_high_water);
	}
}

static void sample_work_handler(struct k_work *work)
{
	struct sample item;
	int ret;

	ARG_UNUSED(work);
	item.sequence = (uint32_t)atomic_inc(&produced_count) + 1U;
	item.value = (int32_t)((item.sequence * 53U) % 1024U);
	item.timestamp_ms = k_uptime_get();

	ret = k_msgq_put(&sample_queue, &item, K_NO_WAIT);
	if (ret == 0) {
		update_queue_high_water();
	} else if (ret == -ENOMSG) {
		atomic_inc(&dropped_count);
	} else {
		atomic_inc(&dropped_count);
		printk("sample queue put failed: %d\n", ret);
	}
}

static void timer_expired(struct k_timer *timer)
{
	int ret;

	ARG_UNUSED(timer);
	atomic_inc(&timer_events);
	ret = k_work_submit(&sample_work);
	if (ret < 0) {
		atomic_inc(&work_submit_errors);
	}
}

K_TIMER_DEFINE(sample_timer, timer_expired, NULL);

static void consumer(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	while (true) {
		struct sample item;
		int ret = k_msgq_get(&sample_queue, &item, K_MSEC(500));

		if (ret == 0) {
			printk("processed seq=%u value=%d age=%lld ms\n",
			       item.sequence, item.value,
			       (long long)(k_uptime_get() - item.timestamp_ms));
			k_msleep(CONSUMER_WORK_MS);
			atomic_inc(&processed_count);
		} else if (ret == -EAGAIN) {
			atomic_inc(&timeout_count);
			printk("consumer timeout\n");
		} else {
			printk("sample queue get failed: %d\n", ret);
		}
	}
}

K_THREAD_DEFINE(consumer_thread, CONSUMER_STACK_SIZE, consumer, NULL, NULL, NULL,
		K_PRIO_PREEMPT(2), 0, 0);

int main(void)
{
	bool capture_paused = false;
	int64_t phase_started_ms = k_uptime_get();
	int64_t pause_started_ms = 0;

	k_work_init(&sample_work, sample_work_handler);
	k_timer_start(&sample_timer, K_MSEC(750), K_MSEC(SAMPLE_PERIOD_MS));
	printk("Integrated sample pipeline started\n");

	while (true) {
		k_msleep(2000);
		int64_t now_ms = k_uptime_get();

		if (!capture_paused && now_ms - phase_started_ms >= 30000) {
			k_timer_stop(&sample_timer);
			capture_paused = true;
			pause_started_ms = now_ms;
			printk("capture paused to exercise the receive timeout\n");
		} else if (capture_paused && now_ms - pause_started_ms >= 3000) {
			k_timer_start(&sample_timer, K_MSEC(750), K_MSEC(SAMPLE_PERIOD_MS));
			capture_paused = false;
			phase_started_ms = now_ms;
			printk("capture resumed\n");
		}

		printk("timer=%d produced=%d processed=%d dropped=%d timeouts=%d "
		       "submit_errors=%d queue=%u/%d high_water=%d/%d\n",
		       atomic_get(&timer_events), atomic_get(&produced_count),
		       atomic_get(&processed_count), atomic_get(&dropped_count),
		       atomic_get(&timeout_count), atomic_get(&work_submit_errors),
		       k_msgq_num_used_get(&sample_queue), QUEUE_CAPACITY,
		       atomic_get(&queue_high_water), QUEUE_CAPACITY);
	}
}
