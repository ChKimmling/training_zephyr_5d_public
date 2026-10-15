#include <errno.h>
#include <stdint.h>

#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

enum boot_trace_bit {
	BOOT_TRACE_EARLY = BIT(0),
	BOOT_TRACE_PRE_KERNEL_1 = BIT(1),
	BOOT_TRACE_PRE_KERNEL_2 = BIT(2),
	BOOT_TRACE_POST_KERNEL = BIT(3),
	BOOT_TRACE_APPLICATION = BIT(4),
};

enum boot_trace_event {
	BOOT_EVENT_EARLY,
	BOOT_EVENT_PRE_KERNEL_1,
	BOOT_EVENT_PRE_KERNEL_2,
	BOOT_EVENT_POST_KERNEL,
	BOOT_EVENT_APPLICATION,
};

static volatile uint32_t boot_trace_mask;
static volatile enum boot_trace_event boot_trace_order[5];
static volatile uint8_t boot_trace_count;

static void boot_trace_record(enum boot_trace_event event)
{
	if (boot_trace_count < ARRAY_SIZE(boot_trace_order)) {
		boot_trace_order[boot_trace_count] = event;
		boot_trace_count++;
	}
}

static const char *boot_trace_event_name(enum boot_trace_event event)
{
	switch (event) {
	case BOOT_EVENT_EARLY:
		return "EARLY";
	case BOOT_EVENT_PRE_KERNEL_1:
		return "PRE_KERNEL_1";
	case BOOT_EVENT_PRE_KERNEL_2:
		return "PRE_KERNEL_2";
	case BOOT_EVENT_POST_KERNEL:
		return "POST_KERNEL";
	case BOOT_EVENT_APPLICATION:
		return "APPLICATION";
	default:
		return "UNKNOWN";
	}
}

int boot_trace_early(void)
{
	boot_trace_mask |= BOOT_TRACE_EARLY;
	boot_trace_record(BOOT_EVENT_EARLY);
	return 0;
}

SYS_INIT(boot_trace_early, EARLY, 50);

int boot_trace_pre_kernel_1(void)
{
	boot_trace_mask |= BOOT_TRACE_PRE_KERNEL_1;
	boot_trace_record(BOOT_EVENT_PRE_KERNEL_1);
	return 0;
}

SYS_INIT(boot_trace_pre_kernel_1, PRE_KERNEL_1, 50);

int boot_trace_pre_kernel_2(void)
{
	boot_trace_mask |= BOOT_TRACE_PRE_KERNEL_2;
	boot_trace_record(BOOT_EVENT_PRE_KERNEL_2);
	return 0;
}

SYS_INIT(boot_trace_pre_kernel_2, PRE_KERNEL_2, 50);

int boot_trace_post_kernel(void)
{
	boot_trace_mask |= BOOT_TRACE_POST_KERNEL;
	boot_trace_record(BOOT_EVENT_POST_KERNEL);
	return 0;
}

SYS_INIT(boot_trace_post_kernel, POST_KERNEL, 50);

int boot_trace_application(void)
{
	boot_trace_mask |= BOOT_TRACE_APPLICATION;
	boot_trace_record(BOOT_EVENT_APPLICATION);
	return 0;
}

SYS_INIT(boot_trace_application, APPLICATION, 50);

int main(void)
{
	const uint32_t expected = BOOT_TRACE_EARLY | BOOT_TRACE_PRE_KERNEL_1 |
				  BOOT_TRACE_PRE_KERNEL_2 | BOOT_TRACE_POST_KERNEL |
				  BOOT_TRACE_APPLICATION;
	const enum boot_trace_event expected_order[] = {
		BOOT_EVENT_EARLY,       BOOT_EVENT_PRE_KERNEL_1, BOOT_EVENT_PRE_KERNEL_2,
		BOOT_EVENT_POST_KERNEL, BOOT_EVENT_APPLICATION,
	};
	bool order_matches = boot_trace_count == ARRAY_SIZE(expected_order);

	printk("BOOT_TRACE mask=0x%02x expected=0x%02x\n", (unsigned int)boot_trace_mask,
	       (unsigned int)expected);

	for (uint8_t position = 0U; position < boot_trace_count; position++) {
		const enum boot_trace_event observed = boot_trace_order[position];

		printk("BOOT_TRACE position=%u level=%s\n", position,
		       boot_trace_event_name(observed));
		if (position >= ARRAY_SIZE(expected_order) ||
		    observed != expected_order[position]) {
			order_matches = false;
		}
	}

	printk("BOOT_TRACE main reached after APPLICATION initialization\n");

	if (boot_trace_mask != expected || !order_matches) {
		printk("BOOT_TRACE order=FAIL\n");
		return -EIO;
	}

	printk("BOOT_TRACE order=PASS\n");

	return 0;
}
