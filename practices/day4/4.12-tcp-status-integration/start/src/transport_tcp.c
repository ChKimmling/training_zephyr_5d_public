#include "transport.h"

#include <errno.h>
#include <string.h>

#include <zephyr/net/socket.h>

static int wait_for_socket(int descriptor, short events)
{
	struct zsock_pollfd poll_fd = {.fd = descriptor, .events = events};
	int rc = zsock_poll(&poll_fd, 1, 2000);

	if (rc == 0) {
		return -ETIMEDOUT;
	}
	if (rc < 0) {
		return -errno;
	}
	if ((poll_fd.revents & events) != 0) {
		return 0;
	}
	return -ECONNRESET;
}

static int send_all(int descriptor, const char *data, size_t size)
{
	size_t sent = 0;

	while (sent < size) {
		int ready = wait_for_socket(descriptor, ZSOCK_POLLOUT);

		if (ready < 0) {
			return ready;
		}
		int rc = zsock_send(descriptor, data + sent, size - sent, 0);
		if (rc <= 0) {
			return rc == 0 ? -EPIPE : -errno;
		}
		sent += (size_t)rc;
	}
	return 0;
}

static int receive_request(int descriptor, char *request, size_t size)
{
	size_t used = 0;

	if (size < 2U) {
		return -ENOSPC;
	}
	while (used < size - 1U) {
		int ready = wait_for_socket(descriptor, ZSOCK_POLLIN);

		if (ready < 0) {
			return ready;
		}
		int rc = zsock_recv(descriptor, &request[used], 1, 0);

		if (rc == 0) {
			return -ECONNRESET;
		}
		if (rc < 0) {
			return -errno;
		}
		if (request[used++] == '\n') {
			request[used] = '\0';
			return 0;
		}
	}
	request[size - 1U] = '\0';
	return -EMSGSIZE;
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
	size_t response_length;
	int session = zsock_accept(listener, NULL, NULL);
	int rc;

	if (session < 0) {
		return -errno;
	}
	if (response == NULL || response_size == 0U) {
		rc = -EINVAL;
	} else {
		const char *response_end;

		rc = receive_request(session, request, sizeof(request));
		response_end = memchr(response, '\0', response_size);
		if (rc == 0 && response_end == NULL) {
			rc = -ENOSPC;
		}
		if (rc == 0) {
			response_length = (size_t)(response_end - response);
			rc = strcmp(request, "GET\n") == 0 ?
				send_all(session, response, response_length) : -EPROTO;
		}
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

