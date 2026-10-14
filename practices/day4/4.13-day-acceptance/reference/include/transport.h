#ifndef DAY4_TRANSPORT_H_
#define DAY4_TRANSPORT_H_

#include <stddef.h>

int transport_tcp_open_listener(unsigned short port);
int transport_tcp_accept_one(int listener, char *response, size_t response_size);
void transport_tcp_close(int *descriptor);

#endif

