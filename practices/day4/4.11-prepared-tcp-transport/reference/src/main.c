#include <errno.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/socket.h>

#include "transport.h"

LOG_MODULE_REGISTER(tcp_reference, LOG_LEVEL_INF);

#define PORT 4242

K_SEM_DEFINE(server_ready, 0, 1);
K_SEM_DEFINE(client_done, 0, 1);
static char client_response[128];
static int client_result;

static void client(void)
{
	struct sockaddr_in address = {
		.sin_family = AF_INET,
		.sin_port = htons(PORT),
		.sin_addr = INADDR_LOOPBACK_INIT,
	};
	int descriptor;
	int rc;

	k_sem_take(&server_ready, K_FOREVER);
	descriptor = zsock_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (descriptor < 0) {
		client_result = -errno;
		k_sem_give(&client_done);
		return;
	}
	rc = zsock_connect(descriptor, (struct sockaddr *)&address, sizeof(address));
	if (rc == 0) {
		rc = zsock_send(descriptor, "GET\n", 4, 0);
	}
	if (rc > 0) {
		rc = zsock_recv(descriptor, client_response, sizeof(client_response) - 1, 0);
	}
	if (rc > 0) {
		client_response[rc] = '\0';
		client_result = 0;
	} else {
		client_result = rc == 0 ? -ECONNRESET : -errno;
	}
	zsock_close(descriptor);
	k_sem_give(&client_done);
}

K_THREAD_DEFINE(client_id, 2048, client, NULL, NULL, NULL, 5, 0, 0);

int main(void)
{
	char response[] = "seq=7 quality=OK connected=1\n";
	int listener = transport_tcp_open_listener(PORT);
	int rc;

	if (listener < 0) {
		LOG_ERR("listen failed: %d", listener);
		return listener;
	}
	k_sem_give(&server_ready);
	rc = transport_tcp_accept_one(listener, response, sizeof(response));
	transport_tcp_close(&listener);
	k_sem_take(&client_done, K_FOREVER);
	LOG_INF("client received: %s", client_response);
	if (rc < 0 || client_result < 0 || strcmp(client_response, response) != 0) {
		LOG_ERR("TCP_REFERENCE FAIL server=%d client=%d", rc, client_result);
		return -EINVAL;
	}
	LOG_INF("TCP_REFERENCE PASS");
	return 0;
}

