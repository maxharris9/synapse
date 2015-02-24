/* event.c
 * event routines for System 1.0. 
 * Copyright(c) 1998 Objective Reality Computer.
 * By Nick McKinney and Max Harris. 
 */

#include <sys/time.h>
#include <unistd.h>
#include "tree.h"
#include "event.h"
#include "window.h" // for event_window()
#include "wmanager.h"  // REMOVE ME
#include "button.h" // for event_button()
#include "menu.h" // for event_menu()
#include "bar.h" // for event_bar()
#include "text.h" // for event_text()
#include "nodetypes.h"  // for error reporting
#include "conn.h" // for sending apps events
#include "plugin.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int send_node_event(treenode *target, treenode *node, int event, int modifier, point *where)
{
  treenode *try = target;
  unsigned long usec;
  struct timeval tv;
  int action;

  gettimeofday(&tv, NULL);
  usec = tv.tv_sec * 100;
  usec += (tv.tv_usec / 10000);

  while(1)
  {
    if(!try)
      return -1;

    action = 1;  // ignore

    if(!(try->mask & (1 << event)))
      action = try->event(try, node, event, modifier, where, usec);

    if(action <= 0 || event == EVENT_COLLIDE)
      return action;

    try = try->parent;
    if(try == &rootnode)
      return 0;
  }
}
