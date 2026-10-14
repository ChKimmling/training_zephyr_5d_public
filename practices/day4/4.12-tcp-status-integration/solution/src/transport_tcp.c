#include "transport.h"

#include <errno.h>
#include <string.h>

#include <zephyr/net/socket.h>

static int send_all(int descriptor, const char *data, size_t size)
{
	size_t sent = 0;

	while (sent < size) {
		int rc = zsock_send(descriptor, data + sent, size - sent, 0);
		if (rc <= 0) {
			return rc == 0 ? -EPIPE : -errno;
		}
		sent += (size_t)rc;
	}
	return 0;
}

int transport_tcp_open_listener(unsigned short port)
{
	struct sockaddr_in address = {
		.sin_family = AF_INET,
		.sin_port = htons(port),
		.sin_addr = INADDR_LOOPBACK_INIT,
	};
	int descriptor = zsock_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (descriptor < 0) {
		return -errno;
	}
	if (zsock_bind(descriptor, (struct sockaddr *)&address, sizeof(address)) < 0 ||
	    zsock_listen(descriptor, 1) < 0) {
		int rc = -errno;
		zsock_close(descriptor);
		return rc;
	}
	return descriptor;
}

int transport_tcp_accept_one(int listener, char *response, size_t response_size)
{
	char request[16];
	int session = zsock_accept(listener, NULL, NULL);
	int rc;
	(void)response_size;

	if (session < 0) {
		return -errno;
	}
	rc = zsock_recv(session, request, sizeof(request) - 1, 0);
	if (rc == 0) {
		rc = -ECONNRESET;
	} else if (rc < 0) {
		rc = -errno;
	} else {
		request[rc] = '\0';
		rc = strcmp(request, "GET\n") == 0 ?
			send_all(session, response, strlen(response)) : -EPROTO;
	}
	transport_tcp_close(&session);
	return rc;
}

void transport_tcp_close(int *descriptor)
{
	if (descriptor != NULL && *descriptor >= 0) {
		zsock_close(*descriptor);
		*descriptor = -1;
	}
}

