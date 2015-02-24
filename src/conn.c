/* socket connection functions */
/* implements connecting to synapse and api communications */
/* Copyright(c) 1998-99 Objective Reality Computer. */


#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include "conn.h"
#include "event.h"
#include "tree.h"
#include "nodetypes.h"
#include "api.h"

static int mastersocket;

int event_app(struct treenode *target, struct treenode *node, int event, int modifier, point *where, unsigned long usec)
{
  int msg[9];

  msg[0] = sizeof(int)*9;
  msg[1] = 0x2000;
  msg[2] = (int) node;
  msg[3] = event;
  msg[4] = modifier;
  if(where)
  {
    msg[5] = floattoint(where->x);
    msg[6] = floattoint(where->y);
    msg[7] = floattoint(where->z);
  }
  else
    msg[5] = msg[6] = msg[7] = floattoint(0.0);
  msg[8] = usec;

  sendmessage((connection *)target, msg, msg[0]);
  return 0;
}

int init_sockets(void)
{
  char hostname[128];
  struct hostent *hp;
  struct sockaddr_in sin;
  int one = 1;

  gethostname(hostname, sizeof(hostname));
  if((hp = gethostbyname(hostname)) == NULL)
    return -1;

  if((mastersocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    return -2;

  if(setsockopt(mastersocket, SOL_SOCKET, SO_REUSEADDR, (char *) &one, sizeof(one)) == -1)
    return -3;

  sin.sin_family = AF_INET;
  sin.sin_port = htons(STD_PORT);
  sin.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(mastersocket, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    return -4;

  listen(mastersocket, SOMAXCONN);

  return 0;
}

void close_sockets(void)
{
  close(mastersocket);
}

int conn_waiting(void)
{
  struct timeval tv;
  fd_set fdset;

  tv.tv_sec = tv.tv_usec = 0;
  FD_ZERO(&fdset);
  FD_SET(mastersocket, &fdset);
  return select(mastersocket + 1, &fdset, NULL, NULL, &tv);
}

connection * new_conn(void)
{
  connection *theconn;
  struct sockaddr_in sin;
  unsigned int fromlen = sizeof(struct sockaddr_in);
  GLfloat colorz[3][9];
  static int whichone = 0;

  colorz[0][0] = 0.6156; colorz[1][0] = 0.688; colorz[2][0] = 0.735; // grey

  colorz[0][1] = 0.8784; colorz[1][1] = 0.721; colorz[2][1] = 0.011; // yeller
  colorz[0][2] = 0.5058; colorz[1][2] = 0.050; colorz[2][2] = 0.019; // red
  colorz[0][3] = 0.1156; colorz[1][3] = 0.390; colorz[2][3] = 0.586; // blue-ish green
  colorz[0][4] = 0.2156; colorz[1][4] = 0.288; colorz[2][4] = 0.435; // grape
 
  colorz[0][5] = 0.7784; colorz[1][5] = 0.521; colorz[2][5] = 0.011; // dark yeller
  colorz[0][6] = 0.4058; colorz[1][6] = 0.050; colorz[2][6] = 0.019; // dark red
  colorz[0][7] = 0.0156; colorz[1][7] = 0.290; colorz[2][7] = 0.486; // dark blue-ish green
  colorz[0][8] = 0.1156; colorz[1][8] = 0.188; colorz[2][8] = 0.335; // evil grape

  theconn = (connection *) create_child(&rootnode, sizeof(connection));
  if(!theconn)
    return (connection *)-1;
  theconn->nodedef.type = APP;
  theconn->nodedef.mask = MASK_COLLIDE;
  theconn->nodedef.event = event_app;

  theconn->nodedef.color[0] = colorz[0][whichone];
  theconn->nodedef.color[1] = colorz[1][whichone];
  theconn->nodedef.color[2] = colorz[2][whichone];
  theconn->nodedef.color[3] = 1.0;
  whichone++;

  if (whichone > 8)
     whichone = 1;

  if ((theconn->fd = accept(mastersocket, (struct sockaddr *)&sin, &fromlen)) < 0)
  {
    destroy_node((treenode *)theconn);
    return (connection *)-1;
  }
  fcntl(theconn->fd, F_SETFL, O_NONBLOCK);

  theconn->addr = ntohl(sin.sin_addr.s_addr);
  if(!(theconn->buffer = malloc(BUFFLEN)))
  {
    destroy_node((treenode *)theconn);
    return (connection *)-1;
  }
  
  theconn->bufflen = BUFFLEN;
  theconn->buffat = 0;
  theconn->kaput = 0;

  set_active_node((treenode *) theconn);

  return theconn;
}

int close_conn(connection *this)
{
  if(!this)
    return -1;

  this->kaput = 1;
  close(this->fd);
  free(this->buffer);
  destroy_node((treenode *)this);
  return 0;
}

int process_connections()
{
  struct timeval tv;
  fd_set fdset;
  int y, maxfd = 0;
  treenode *this;

  tv.tv_sec = tv.tv_usec = 0;
  FD_ZERO(&fdset);

  this = rootnode.child;
  while(this)
  {
    if(this->type == APP)
    {
      FD_SET(((connection *)this)->fd, &fdset);
      if(((connection *)this)->fd > maxfd)
        maxfd = ((connection *)this)->fd;
    }
    this = this->next;
  }
  if((y = select(maxfd + 1, &fdset, NULL, NULL, &tv)) > 0)
  {
    this = rootnode.child;
    while((int)this > 0x1000)
    {
      if(this->type == APP)
        if(FD_ISSET(((connection *)this)->fd, &fdset))
          if((y = process_conn((connection *)this)) != 0)
            return y;
      this = this->next;
    }
  }
  else if(y < 0)
    printf("error in select\n");
  return 0;
}

/* static connection * find_conn_by_fd(int fd)
{
  treenode *this;

  this = rootnode.child;
  while(this)
  {
    if(this->type == APP)
      if(((connection *)this)->fd == fd)
        return (connection *)this;
    this = this->next;
  }
  return (connection *)0;
} */

int process_conn(connection *this)
{
  int y, rd, fd;

  fd = this->fd;

  // here's where this scheme would break:  if there's more to be read than the buffer can handle, the app would hang

  // NOTE: if you modify this here, you need to modify it below
  if((rd = recv(fd, (void *)(((int)this->buffer) + this->buffat), this->bufflen - this->buffat, 0)) == -1)
    return -1;
  if(rd == 0)
  {
    close_conn(this);  // this is a pretty good sign that the connection has been closed
    return -2;
  }

  this->buffat += rd;
  if(this->buffat < sizeof(int))
    return 0;

  while(rd > 0)
  {
    while((this->buffat >= ntohl(this->buffer[0])) && this->buffat > 2)  // buffer[0] == size of message
    {
      if(do_api(this, ntohl(this->buffer[0]) - 4, ntohl(this->buffer[1]), &(this->buffer[2])))
        return -1;
      y = ntohl(this->buffer[0]);
      memmove(this->buffer, (void *)((int)(this->buffer) + y), this->buffat - y);
      this->buffat -= y;
      y = ntohl(this->buffer[0]);
    }
    if((rd = recv(fd, (void *)(((int)this->buffer) + this->buffat), this->bufflen - this->buffat, 0)) == -1)
    {
      if(errno != EWOULDBLOCK)
        return -1;
    }
    else
      this->buffat += rd;
  }
  return 0;
}

int sendmessage(connection *this, void *data, unsigned int len)
{
  int *ip;
  int i;

  if(this->kaput)
    return 0;  // silently fail

  ip = (int *) malloc(len);
  if(!ip)
    return -3;

  memcpy(ip, data, len);
  for(i = 0; i < (len / sizeof(int)); i++)
    ip[i] = htonl(ip[i]);

  if(send(this->fd, ip, len, 0) < 0)
  {
    close_conn(this);
    return -1;
  }

  return 0;
}

