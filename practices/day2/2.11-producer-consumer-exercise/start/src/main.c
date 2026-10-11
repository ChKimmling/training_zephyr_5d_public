#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define WORKER_STACK_SIZE 1024

struct sample {
	uint32_t sequence;
	int32_t value;
	int64_t produced_at_ms;
};

static void producer(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	/* TODO: erzeugen und in die Message Queue einstellen. */
}

static void consumer(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	/* TODO: Nachrichten mit endlichem Timeout empfangen und verarbeiten. */
}

K_THREAD_DEFINE(producer_thread, WORKER_STACK_SIZE, producer, NULL, NULL, NULL,
		K_PRIO_PREEMPT(2), 0, 0);
K_THREAD_DEFINE(consumer_thread, WORKER_STACK_SIZE, consumer, NULL, NULL, NULL,
		K_PRIO_PREEMPT(3), 0, 0);

int main(void)
{
	printk("Producer-consumer scaffold: implement the queue pipeline\n");
	return 0;
}
