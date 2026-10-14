#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/conn_mgr_monitor.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_mgmt.h>

LOG_MODULE_REGISTER(net_mgmt_demo, LOG_LEVEL_INF);

#define L4_EVENTS (NET_EVENT_L4_CONNECTED | NET_EVENT_L4_DISCONNECTED)

static struct net_mgmt_event_callback l4_callback;
static struct k_work event_work;
static atomic_t pending_connected;

static void event_worker(struct k_work *work)
{
	ARG_UNUSED(work);
	LOG_INF("worker context: connected=%d", atomic_get(&pending_connected));
}

static void l4_event(struct net_mgmt_event_callback *callback, uint64_t event,
		     struct net_if *iface)
{
	ARG_UNUSED(callback);
	LOG_INF("callback context: event=%s iface=%p",
		event == NET_EVENT_L4_CONNECTED ? "connected" : "disconnected", iface);
	atomic_set(&pending_connected, event == NET_EVENT_L4_CONNECTED);
	k_work_submit(&event_work);
}

int main(void)
{
	struct net_if *iface = net_if_get_default();

	k_work_init(&event_work, event_worker);
	net_mgmt_init_event_callback(&l4_callback, l4_event, L4_EVENTS);
	net_mgmt_add_event_callback(&l4_callback);
	conn_mgr_mon_resend_status();

	net_mgmt_event_notify(NET_EVENT_L4_CONNECTED, iface);
	k_work_flush(&event_work, &(struct k_work_sync){});
	net_mgmt_event_notify(NET_EVENT_L4_DISCONNECTED, iface);
	k_work_flush(&event_work, &(struct k_work_sync){});
	LOG_INF("NET_MGMT_DEMO PASS");
	return 0;
}

