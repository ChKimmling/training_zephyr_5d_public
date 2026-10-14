#include "connectivity.h"

#include <zephyr/kernel.h>
#include <zephyr/net/conn_mgr_monitor.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_mgmt.h>

#include "app_state.h"

#define L4_EVENTS (NET_EVENT_L4_CONNECTED | NET_EVENT_L4_DISCONNECTED)

static struct net_mgmt_event_callback l4_callback;
static struct k_work state_work;
static struct k_work_sync work_sync;
static atomic_t pending_connected;

static void apply_state(struct k_work *work)
{
	ARG_UNUSED(work);
	/* TODO(4.10-03): Den vorgemerkten Zustand erst hier im Workqueue-
	 * Kontext an app_state übergeben. */
}

static void l4_event(struct net_mgmt_event_callback *callback, uint64_t event,
		     struct net_if *iface)
{
	ARG_UNUSED(callback);
	ARG_UNUSED(iface);
	/* TODO(4.10-02): Connected und Disconnected erkennen; andere Events
	 * ignorieren. Den Zustand atomar vormerken und state_work einreichen. */
	ARG_UNUSED(event);
}

int connectivity_init(void)
{
	atomic_set(&pending_connected, 0);
	k_work_init(&state_work, apply_state);
	net_mgmt_init_event_callback(&l4_callback, l4_event, L4_EVENTS);
	net_mgmt_add_event_callback(&l4_callback);
	/* TODO(4.10-01): Den initialen Zustand an app_state übergeben und den
	 * aktuellen Monitorstatus mit conn_mgr_mon_resend_status() anfordern. */
	return 0;
}

void connectivity_emit_for_test(bool connected)
{
	net_mgmt_event_notify(connected ? NET_EVENT_L4_CONNECTED : NET_EVENT_L4_DISCONNECTED,
			      net_if_get_default());
}

void connectivity_wait_for_work(void)
{
	k_work_flush(&state_work, &work_sync);
}
