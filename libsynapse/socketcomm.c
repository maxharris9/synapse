/* socketcomm.c
 *
 * for starting/stopping socket-based ipc, sending, receiving, etc.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>
#include <termios.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>

#include "libsynapse_internal.h"
#include "socketcomm.h"
#include "ls_event.h"
#include "ls_wmapi.h"
#include "ls_ctrlapi.h"
#include "ls_idmap.h"
#include "../src/api_defs.h"

static unsigned int buffer[BUFLEN];
static unsigned int buffat = 0;

void syn_quit(void);

// connect to the named synapse server.  ipaddr and port are both
// in host byte order (i.e., no htonl() necessary)
int connect_to_server(unsigned long ipaddr, unsigned short port)
{
  int s;
  struct sockaddr_in sin;

  if(LS_socket != 0)
    return -4;

  if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    return -1;

  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  sin.sin_addr.s_addr = htonl(ipaddr); 

  if (connect (s, (struct sockaddr *) &sin, sizeof (sin)) < 0)
    return -1;

  LS_socket = s;
  return 0;
}

int close_connection(void)
{
  if(LS_socket == 0)
    return -4;

  close(LS_socket);
  LS_socket = 0;
  return 0;
}

int wait_for_data(struct timeval *tv)
{
  fd_set read_set;
  FD_ZERO (&read_set);
  FD_SET (LS_socket, &read_set);

  if (select (LS_socket + 1, &read_set, NULL, NULL, tv) > 0)
    return 0;
  return -2;
}

int sendmessage(header *head, void *data)
{
  int *ip;
  int i;

  ip = (int *) malloc(head->len);
  if(!ip)
    return -3;

  memcpy(&ip[2], data, head->len - 8);
  for(i = 2; i < (head->len / sizeof(int)); i++)
    ip[i] = htonl(ip[i]);
  ip[0] = htonl(head->len);
  ip[1] = htonl(head->type);

  if(send(LS_socket, ip, head->len, 0) < 0)
    return -1;

  return 0;
}

int process_protocol_message(unsigned int len, int type, void *message)
{
  switch(type)
  {
    case API_DESTROY_OBJECT:  // not an api call
      remove_ptr_from_map((void *)(*((int *)message)));
      return 0;

    default:
      return 0;
  }
}

int process_message(unsigned int len, int type, void *message)
{
  int *ip;
  int i;

  ip = malloc(len);
  memcpy(ip, message, len);
  for(i = 0; i < (len / sizeof(int)); i++)
    ip[i] = ntohl(ip[i]);

  if(type < 0x1000)
  {
    i =  process_protocol_message(len, type, (void *)ip);
    free(ip);
    return i;
  }
  if(type < 0x2000)
  {
    i =  process_wmanager_message(len, type, (void *)ip);
    free(ip);
    return i;
  }
  if(type < 0x3000)
  {
    i =  process_event_message(len, type, (void *)ip);
    free(ip);
    return i;
  }
  if(type < 0x4000)
  {
    i =  process_control_message(len, type, (void *)ip);
    free(ip);
    return i;
  }
  free(ip);
  return -4;
}

int process_incoming(void)
{
  int y, rd;
  int *message_in_process;

  // here's where this scheme would break:  if there's more to be read than the buffer can handle, the app would hang

  if((rd = recv(LS_socket, (void *)(((int)buffer) + buffat), BUFLEN - buffat, 0)) == -1)
    return -1;
  if(rd == 0)   // ut oh...broken connection
    syn_quit();

  buffat += rd;
  if(buffat < sizeof(int))
    return 0;

  while((buffat >= ntohl(buffer[0])) && buffat > 2)  // buffer[0] == size of message
  {
    message_in_process = (int *) malloc(ntohl(buffer[0]));
    memcpy(message_in_process, buffer, ntohl(buffer[0]));
    y = ntohl(buffer[0]);
    memmove(buffer, (void *)((int)buffer + y), buffat - y);
    buffat -= y;
    y = ntohl(buffer[0]);
    process_message(ntohl(message_in_process[0]) - 4, ntohl(message_in_process[1]), &(message_in_process[2]));
    free(message_in_process);
  }

  return 0;
}

int process_connection(int block)
{
  struct timeval tv;

  if(block)
  {
    wait_for_data(NULL);
    process_incoming();
  }
  else
  {
    tv.tv_sec = 0;
    tv.tv_usec = 10;
    if(wait_for_data(&tv) == 0)
      process_incoming();
  }
  return 0;
}

