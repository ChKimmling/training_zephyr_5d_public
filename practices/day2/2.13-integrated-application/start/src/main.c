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

/* Der Timer erzeugt Ereignisse; Nutzdaten laufen über die Workqueue/Queue. */
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
	ARG_UNUSED(work);

	/*
	 * TODO: Sample erzeugen und mit k_msgq_put(..., K_NO_WAIT) einstellen.
	 * Erfolg oder volle Queue zählen und nach Erfolg den Queue-Höchststand
	 * aktualisieren. Hier läuft Workqueue-Arbeit, nicht der Timer-Callback.
	 */
	update_queue_high_water();
}

static void timer_expired(struct k_timer *timer)
{
	ARG_UNUSED(timer);

	/* Timer-Callbacks müssen kurz bleiben und dürfen nicht blockieren. */
	atomic_inc(&timer_events);
	if (k_work_submit(&sample_work) < 0) {
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
			/*
			 * TODO: Sample verarbeiten, Alter ausgeben und
			 * processed_count erhöhen.
			 */
			k_msleep(CONSUMER_WORK_MS);
		} else if (ret == -EAGAIN) {
			/* Der Timeout ist bei leerer Queue erwartbar. */
			atomic_inc(&timeout_count);
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

	/* Initialisierung muss vor dem Timer-Start erfolgen. */
	k_work_init(&sample_work, sample_work_handler);
	k_timer_start(&sample_timer, K_MSEC(750), K_MSEC(SAMPLE_PERIOD_MS));
	printk("Integrated application scaffold started\n");

	while (true) {
		k_msleep(2000);
		int64_t now_ms = k_uptime_get();

		/*
		 * TODO: Erfassung alle 30 s für 3 s anhalten und danach neu
		 * starten, damit der Empfangs-Timeout sichtbar wird.
		 */
		if (!capture_paused && now_ms - phase_started_ms >= 30000) {
			k_timer_stop(&sample_timer);
			capture_paused = true;
			pause_started_ms = now_ms;
		} else if (capture_paused && now_ms - pause_started_ms >= 3000) {
			k_timer_start(&sample_timer, K_MSEC(750), K_MSEC(SAMPLE_PERIOD_MS));
			capture_paused = false;
			phase_started_ms = now_ms;
		}

		/*
		 * TODO: Zähler, Queue-Füllstand und Höchststand ausgeben und
		 * die Nachrichtenbilanz prüfen.
		 */
		printk("timer=%d produced=%d processed=%d dropped=%d timeouts=%d "
		       "submit_errors=%d queue=%u/%d high_water=%d/%d\n",
		       atomic_get(&timer_events), atomic_get(&produced_count),
		       atomic_get(&processed_count), atomic_get(&dropped_count),
		       atomic_get(&timeout_count), atomic_get(&work_submit_errors),
		       k_msgq_num_used_get(&sample_queue), QUEUE_CAPACITY,
		       atomic_get(&queue_high_water), QUEUE_CAPACITY);
	}
}
