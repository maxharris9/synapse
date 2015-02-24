#include <sys/time.h>
#include "libsynapse_internal.h"

#ifndef _SOCKETCOMM_H_
#define _SOCKETCOMM_H_

#define BUFLEN 131072  // 128K

int connect_to_server(unsigned long ipaddr, unsigned short port);
int close_connection(void);
int wait_for_data(struct timeval *tv);
int sendmessage(header *head, void *data);
int process_connection(int block);

#endif
