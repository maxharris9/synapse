#include "tree.h"
#include "nodetypes.h"

#ifndef _CONN_H_
#define _CONN_H_

typedef struct
{
  treenode nodedef;
  char *name;
  long addr;
  int fd;
  int *buffer;  // watch for buffer overruns
  int bufflen;
  int buffat;
  int kaput;
} connection;

#define BUFFLEN 65536
#define MAXBUF 65536

#define STD_PORT 3003

int init_sockets(void);
void close_sockets(void);
int conn_waiting(void);
connection * new_conn(void);
int process_connections(void);
int process_conn(connection *this);
int sendmessage(connection *this, void *data, unsigned int len);

#endif
