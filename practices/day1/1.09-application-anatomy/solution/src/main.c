#include <zephyr/kernel.h>

#include "counter.h"
#include "status.h"

int main(void)
{
	status_report();
	counter_report();
	return 0;
}
