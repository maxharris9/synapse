#include <unistd.h>
#include "libsynapse_internal.h"
#include "socketcomm.h"

typedef void (*init_callback) (int, char **);
typedef void (*event_callback) (int, int, int, Point *, int);
typedef void (*idle_callback) (void);

static init_callback callback_init = 0;
static event_callback callback_event = 0;
static idle_callback callback_idle = 0;

int alive = ~0;

void idleloop(void)
{
  while(alive)
  {
    if(callback_idle)
      callback_idle();
    if(process_connection(0) == -1)
      alive = 0;
  }
}

int syn_main(int argc, char **argv)
{
  if(connect_to_server(0x7F000001, 3003) != 0)
    return -1;

  if(callback_init)
    callback_init(argc, argv);

  idleloop();

  close_connection();
  return 0;
}

void syn_quit(void)
{
  alive = 0;
}

int syn_set_callback_init(init_callback callback)
{
  if(!callback)
    return -4;

  callback_init = callback;
  return 0;
}

int syn_set_callback_event(event_callback callback)
{
  if(!callback)
    return -4;

  callback_event = callback;
  return 0;
}

int syn_set_callback_idle(idle_callback callback)
{
  if(!callback)
    return -4;

  callback_idle = callback;
  return 0;
}

int call_event(int id, int type, int modifier, Point *where, int usec)
{
  if(callback_event)
    callback_event(id, type, modifier, where, usec);
  return 0;
}
