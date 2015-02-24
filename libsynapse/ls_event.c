#include "libsynapse_internal.h"
#include "ls_idmap.h"
#include "ls_event.h"
#include <stdio.h>

int call_event(int id, int type, int modifier, Point *where, int usec);

int process_event_message(unsigned int len, int type, void *message)
{
  Point pt;
  int *ip;
  int object, event, modifier, ticks;

  ip = (int *)message;
  object = *ip++;
  event = *ip++;
  modifier = *ip++;
  pt.x = int_to_glfloat(*ip++);
  pt.y = int_to_glfloat(*ip++);
  pt.z = int_to_glfloat(*ip++);
  ticks = *ip++;
  object = (int) find_id_in_map((void *)object);
  if(object == -1)
    return -4;

  call_event(object, event, modifier, &pt, ticks);

  return 0;
}
