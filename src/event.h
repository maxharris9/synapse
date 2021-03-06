#ifndef _EVENT_H_
#define _EVENT_H_

#include "synapse.h"
#include "tree.h"
#include "container.h"

enum event_type
{
   EVENT_MOUSEDOWN = 0,
   EVENT_MOUSEUP = 1,
   EVENT_MOUSEDRAG = 2,
   EVENT_KEYDOWN = 3,
   EVENT_KEYUP = 4,
   EVENT_COLLIDE = 5,
   EVENT_CLICK = 8,
   EVENT_KEYPRESS = 9,
   EVENT_FOCUSON = 10,
   EVENT_FOCUSOFF = 11,
   EVENT_DRAG = 12,
   EVENT_TWIST = 13,
   EVENT_SETTWIST = 14,
   EVENT_RESIZE = 16,
   EVENT_CHANGE = 17,
   EVENT_DESTROY = 18
};

enum event_masks
{
   MASK_MOUSEDOWN = 0x1,
   MASK_MOUSEUP = 0x2,
   MASK_MOUSEDRAG = 0x4,
   MASK_KEYDOWN = 0x8,
   MASK_KEYUP = 0x10,
   MASK_COLLIDE = 0x20,
   MASK_CLICK = 0x100,
   MASK_KEYPRESS = 0x200,
   MASK_FOCUSON = 0x400,
   MASK_FOCUSOFF = 0x800,
   MASK_DRAG = 0x1000,
   MASK_TWIST = 0x2000,
   MASK_SETTWIST = 0x4000,
   MASK_RESIZE = 0x10000,
   MASK_CHANGE = 0x20000,
   MASK_DESTROY = 0x40000,
   MASK_ALL = 0xFFFFFFFF
};

enum special_keys
{
   SYN_KEY_UP = 0x00000100,
   SYN_KEY_DOWN = 0x00000200,
   SYN_KEY_LEFT = 0x00000400,
   SYN_KEY_RIGHT = 0x00000800
};

int send_node_event(treenode *target, treenode *node, int event, int modifier, point *where);

// int send_event(treenode *object, int event, int dsize, void *data);
// int send_app_event(treenode *app, treenode *object, int event, int dsize, void *data);
// int stdcontrol_event(treenode *this, int event, int dsize, int *data);
// int send_manager_event(treenode *owner, treenode *object, int event, int dsize, void *data);

#endif
