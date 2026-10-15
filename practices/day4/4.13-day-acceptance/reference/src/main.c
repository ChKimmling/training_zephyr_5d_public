#include <errno.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/socket.h>
#include "app_state.h"
#include "connectivity.h"
#include "motion_probe.h"
#include "probe_queue.h"
#include "sensor_source.h"
#include "status_provider.h"
#include "transport.h"

LOG_MODULE_REGISTER(tcp_status, LOG_LEVEL_INF);
#define PORT 4243

K_SEM_DEFINE(server_ready, 0, 1);
K_SEM_DEFINE(client_done, 0, 2);
static atomic_t client_successes;
static atomic_t client_failures;

static int client_wait_for_socket(int fd, short events)
{
	struct zsock_pollfd poll_fd = {.fd = fd, .events = events};
	int rc = zsock_poll(&poll_fd, 1, 2000);

	if (rc == 0) return -ETIMEDOUT;
	if (rc < 0) return -errno;
	return (poll_fd.revents & events) != 0 ? 0 : -ECONNRESET;
}

static int client_exchange(int fd, char *response, size_t size)
{
	const char *parts[] = {"G", "ET\n"};
	size_t used = 0;

	for (size_t part = 0; part < ARRAY_SIZE(parts); ++part) {
		size_t sent = 0;
		size_t length = strlen(parts[part]);

		while (sent < length) {
			int ready = client_wait_for_socket(fd, ZSOCK_POLLOUT);

			if (ready < 0) return ready;
			int rc = zsock_send(fd, parts[part] + sent, length - sent, 0);

			if (rc <= 0) {
				return rc == 0 ? -EPIPE : -errno;
			}
			sent += (size_t)rc;
		}
	}
	while (used < size - 1U) {
		int ready = client_wait_for_socket(fd, ZSOCK_POLLIN);

		if (ready < 0) return ready;
		int rc = zsock_recv(fd, response + used, size - 1U - used, 0);

		if (rc == 0) {
			return -ECONNRESET;
		}
		if (rc < 0) {
			return -errno;
		}
		used += (size_t)rc;
		response[used] = '\0';
		if (strchr(response, '\n') != NULL) {
			return (int)used;
		}
	}
	return -ENOSPC;
}

static void client(void)
{
	for (int request = 0; request < 2; ++request) {
		struct sockaddr_in address = {
			.sin_family = AF_INET,
			.sin_port = htons(PORT),
			.sin_addr = INADDR_LOOPBACK_INIT,
		};
		char response[192];
		int fd = -1;
		int rc = k_sem_take(&server_ready, K_SECONDS(3));

		if (rc == 0) {
			fd = zsock_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			rc = fd < 0 ? -errno :
				zsock_connect(fd, (struct sockaddr *)&address, sizeof(address));
		}
		if (rc == 0) {
			rc = client_exchange(fd, response, sizeof(response));
		}
		if (rc > 0) {
			LOG_INF("client response %d: %s", request + 1, response);
			if (strstr(response, request == 0 ? "seq=1 " : "seq=2 ") != NULL &&
			    strstr(response, "quality=OK ") != NULL &&
			    strstr(response, "connected=1") != NULL) {
				atomic_inc(&client_successes);
			} else {
				rc = -EBADMSG;
			}
		}
		if (rc <= 0) {
			LOG_ERR("client request %d failed: %d", request + 1, rc);
			atomic_inc(&client_failures);
		}
		if (fd >= 0) zsock_close(fd);
		k_sem_give(&client_done);
	}
}

K_THREAD_DEFINE(client_id, 2048, client, NULL, NULL, NULL, 5, 0, 0);

static int capture(uint32_t sequence)
{
	const struct motion_axes stimulus = {(int32_t)sequence * 100, 0, 9807};
	struct motion_axes axes = {0};
	struct motion_probe probe;
	int rc = sensor_source_set_sample(&stimulus);

	rc = rc < 0 ? rc : sensor_source_read(&axes);
	if (rc < 0) app_state_note_sensor_error();
	motion_probe_build(&probe, &axes, rc, sequence, k_uptime_get());
	if (probe_queue_publish_latest(&probe) < 0 ||
	    probe_queue_take(&probe, K_NO_WAIT) < 0) return -EIO;
	app_state_accept_probe(&probe);
	return 0;
}

static int serve_snapshot(int listener)
{
	char response[192];
	int rc = status_format(response, sizeof(response));

	if (rc >= 0) {
		k_sem_give(&server_ready);
		rc = transport_tcp_accept_one(listener, response, sizeof(response));
	}
	app_state_note_transport_result(rc == 0);
	return rc;
}

int main(void)
{
	struct app_state_snapshot state;
	int listener = -1;
	int rc = sensor_source_init();

	app_state_reset();
	probe_queue_reset();
	if (rc == 0) rc = connectivity_init();
	if (rc == 0) rc = capture(1);
	if (rc == 0) {
		char tiny[8];

		if (status_format(tiny, sizeof(tiny)) != -ENOSPC) rc = -EINVAL;
	}
	connectivity_emit_for_test(true);
	connectivity_wait_for_work();
	app_state_get(&state);
	if (rc == 0 && state.connected) {
		listener = transport_tcp_open_listener(PORT);
		rc = listener < 0 ? listener : serve_snapshot(listener);
	}
	connectivity_emit_for_test(false);
	connectivity_wait_for_work();
	if (rc == 0) rc = capture(2);
	connectivity_emit_for_test(true);
	connectivity_wait_for_work();
	if (rc == 0) rc = serve_snapshot(listener);
	transport_tcp_close(&listener);
	for (int request = 0; request < 2; ++request) {
		if (k_sem_take(&client_done, K_SECONDS(3)) < 0 && rc == 0) rc = -ETIMEDOUT;
	}
	app_state_get(&state);
	if (rc < 0 || atomic_get(&client_successes) != 2 ||
	    atomic_get(&client_failures) != 0 ||
	    state.transport_requests != 2U || state.last_probe.sequence != 2U) {
		LOG_ERR("TCP_STATUS FAIL rc=%d clients=%d failures=%d requests=%u seq=%u",
			rc, (int)atomic_get(&client_successes),
			(int)atomic_get(&client_failures),
			state.transport_requests, state.last_probe.sequence);
		return -EINVAL;
	}
	LOG_INF("TCP_STATUS PASS");
	return 0;
}

