/* implements a "window manager"-like functionality */

#include <stdio.h>
#include <stdlib.h>
#include "synapse.h"
#include "tree.h"
#include "event.h"
#include "window.h"
#include "wmanager.h"
#include "resources.h"
#include "resource.h"
#include "glyph.h"
#include "button.h"
#include "text.h"
#include "coord.h"

extern unsigned int resources[];

int titlebar_event(struct treenode *target, struct treenode *node, int event, int modifier, point *where, unsigned long usec)
{
  point dar;
  static point twistpoint;

  switch(event)
  {
    case EVENT_MOUSEDRAG:
      dar = *where;
      dar.x += target->parent->origin.x;
      dar.y += target->parent->origin.y;
      dar.z += target->parent->origin.z;
      move_window(target->parent, &dar);
      break;

    case EVENT_SETTWIST:
      twistpoint = *where;
      break;

    case EVENT_TWIST:
      dar = *where;
      dar.x += target->parent->rotation.x;
      dar.y += target->parent->rotation.y;
      dar.z += target->parent->rotation.z;
      twist_window(target->parent, &dar, &twistpoint);
      break;

    case EVENT_MOUSEDOWN:
      break;

    case EVENT_MOUSEUP:
      break;
  }
  return 0;
}

// warning: the following event code works for a single pointer only
int border_event(struct treenode *target, struct treenode *node, int event, int modifier, point *where, unsigned long usec)
{
  static point dar;
  point temp;
  
  static int dx = 0, dy = 0, dz = 0;
  static int mx = 0, my = 0, mz = 0;
  GLfloat ddx, ddy, ddz;
  regwinprop *props;

  props = (regwinprop *)(target->parent->prop);

  switch(event)
  {
    case EVENT_MOUSEDRAG:
      ddx = target->parent->dimentions.x;
      ddy = target->parent->dimentions.y;
      ddz = target->parent->dimentions.z;

      dar.x += (where->x * dx);
      dar.y += (where->y * dy);
      dar.z += (where->z * dz);
      resize_window(target->parent, &dar);
      where->x = ddx - target->parent->dimentions.x;
      where->y = ddy - target->parent->dimentions.y;
      where->z = ddz - target->parent->dimentions.z;

      temp = target->parent->origin;
      temp.x += (where->x * mx);
      temp.y += (where->y * my);
      temp.z += (where->z * mz);
      move_window(target->parent, &temp);
      break;

    case EVENT_MOUSEDOWN:
      dar = target->parent->dimentions;
      dx = dy = dz = mx = my = mz = 0;

      if(target == props->rborder || target == props->brcorner || target == props->trcorner)
        dx = 1;

      if(target == props->tborder || target == props->trcorner || target == props->ltcorner)
        dy = 1;

      if(target == props->lborder || target == props->ltcorner || target == props->blcorner)
      {
        mx = 1;
        dx = -1;
      }

      if(target == props->bborder || target == props->blcorner || target == props->brcorner)
      {
        my = 1;
        dy = -1;
      }
      break;

    case EVENT_MOUSEUP:
      break;

    case EVENT_COLLIDE:
      if(target->subtype == CORNER)
        return collide_corner(target, where);
      else
        return 1;
  }
  return 0;
}

#define CORNERSIZE 0.3375 // MOVE ME!
#define BORDERSIZE 0.0625 // MOVE ME!
#define DBLBORDERSIZE 0.125

// magic numbers in the next function:
// BORDERSCALE:  thickness of border pieces
// 0.25: size (width and height) of border pieces
// DBLBORDERSCALE:  double BORDERSCALE; thickness in z of border and corner pieces
int create_regular(treenode *temp, char *title)
{
   regwinprop *props;

   temp->mask = MASK_ALL & ~(MASK_MOUSEDOWN | MASK_MOUSEUP | MASK_RESIZE | MASK_CLICK);
   temp->event = event_window;

   temp->prop = malloc(sizeof(regwinprop));
   props = (regwinprop *)temp->prop;

   props->destroying = 0;

   props->closebox = create_button(temp, 0, temp->dimentions.y - 0.25, 0.0, .25, .25, 0.0, 0.0, 0.0, 0, BUTTON, PUSH, NULL, 1);
   disallow_focus(props->closebox);

// printf("%Xthis is what the props->closebox guy says.\n", (int)props->closebox);

   create_glyph(props->closebox, 0.07, 0.07, 0.2, 0.2, 0.2, 0, 0, 0, 0, props->closebox->color, resources[RES_CLOSEGRIP], 1);

   props->titlebar = create_button(temp, .25, temp->dimentions.y - .25, 0.0, temp->dimentions.x - .5, .25, 0.0, 0.0, 0.0, 0, BUTTON, PUSH, title, 1);
   disallow_focus(props->titlebar);
   props->titlebar->mask = MASK_ALL & ~(MASK_MOUSEDOWN | MASK_MOUSEUP | MASK_MOUSEDRAG | MASK_TWIST);
   props->titlebar->event = titlebar_event;

   props->zoombox = create_button(temp, temp->dimentions.x - .25, temp->dimentions.y - .25, 0.0, .25, .25, 0.0, 0.0, 0.0, 0, BUTTON, PUSH, NULL, 1);
   disallow_focus(props->zoombox);
   create_glyph(props->zoombox, 0.07, 0.07, 0.2, 0.2, 0.2, 0, 0, 0, 0, props->zoombox->color, resources[RES_SQUAREGRIP], 1);

   props->tborder = create_button(temp, CORNERSIZE - BORDERSIZE, temp->dimentions.y, 0, temp->dimentions.x - (CORNERSIZE + CORNERSIZE) + DBLBORDERSIZE, BORDERSIZE, 0.0, 0.0, 0.0, 0, BUTTON, BORDER, "_", 1);
   disallow_focus(props->tborder);
   props->tborder->mask = MASK_ALL & ~(MASK_MOUSEDOWN | MASK_MOUSEUP | MASK_MOUSEDRAG);
   props->tborder->event = border_event;

   props->bborder = create_button(temp, CORNERSIZE - BORDERSIZE, -BORDERSIZE, 0, temp->dimentions.x - (CORNERSIZE + CORNERSIZE) + DBLBORDERSIZE, BORDERSIZE, 0.0, 0.0, 0.0, 0, BUTTON, BORDER, "_", 1);
   disallow_focus(props->bborder);
   props->bborder->mask = MASK_ALL & ~(MASK_MOUSEDOWN | MASK_MOUSEUP | MASK_MOUSEDRAG);
   props->bborder->event = border_event;

   props->lborder = create_button(temp, -BORDERSIZE, CORNERSIZE - BORDERSIZE, 0, BORDERSIZE, temp->dimentions.y - (CORNERSIZE + CORNERSIZE) + DBLBORDERSIZE, 0.0, 0.0, 0.0, 0, BUTTON, BORDER, "_", 1);
   disallow_focus(props->lborder);
   props->lborder->mask = MASK_ALL & ~(MASK_MOUSEDOWN | MASK_MOUSEUP | MASK_MOUSEDRAG);
   props->lborder->event = border_event;

   props->rborder = create_button(temp, temp->dimentions.x, CORNERSIZE - BORDERSIZE, 0, BORDERSIZE, temp->dimentions.y - (CORNERSIZE + CORNERSIZE) + DBLBORDERSIZE, 0.0, 0.0, 0.0, 0, BUTTON, BORDER, "_", 1);
   disallow_focus(props->rborder);
   props->rborder->mask = MASK_ALL & ~(MASK_MOUSEDOWN | MASK_MOUSEUP | MASK_MOUSEDRAG);
   props->rborder->event = border_event;

   props->ltcorner = create_button(temp, -BORDERSIZE, temp->dimentions.y - CORNERSIZE + BORDERSIZE, 0.0, CORNERSIZE, CORNERSIZE, 0.0, 0.0, 0.0, 0, BUTTON, CORNER, "", 1);
   disallow_focus(props->ltcorner);
   props->ltcorner->mask = MASK_ALL & ~(MASK_MOUSEDOWN | MASK_MOUSEUP | MASK_MOUSEDRAG | MASK_COLLIDE);
   props->ltcorner->event = border_event;

   props->blcorner = create_button(temp, -BORDERSIZE, -BORDERSIZE, 0.0, CORNERSIZE, CORNERSIZE, 0.0, 0.0, 0.0, 0, BUTTON, CORNER, "", 1);
   disallow_focus(props->blcorner);
   props->blcorner->mask = MASK_ALL & ~(MASK_MOUSEDOWN | MASK_MOUSEUP | MASK_MOUSEDRAG | MASK_COLLIDE);
   props->blcorner->event = border_event;

   props->brcorner = create_button(temp, temp->dimentions.x - CORNERSIZE + BORDERSIZE, -BORDERSIZE, 0.0, CORNERSIZE, CORNERSIZE, 0.0, 0.0, 0.0, 0, BUTTON, CORNER, "", 1);
   disallow_focus(props->brcorner);
   props->brcorner->mask = MASK_ALL & ~(MASK_MOUSEDOWN | MASK_MOUSEUP | MASK_MOUSEDRAG | MASK_COLLIDE);
   props->brcorner->event = border_event;

   props->trcorner = create_button(temp, temp->dimentions.x - CORNERSIZE + BORDERSIZE, temp->dimentions.y - CORNERSIZE + BORDERSIZE, 0.0, CORNERSIZE, CORNERSIZE, 0.0, 0.0, 0.0, 0, BUTTON, CORNER, "", 1);
   disallow_focus(props->trcorner);
   props->trcorner->mask = MASK_ALL & ~(MASK_MOUSEDOWN | MASK_MOUSEUP | MASK_MOUSEDRAG | MASK_COLLIDE);
   props->trcorner->event = border_event;

   return 0;
}

int create_dialog(treenode *temp, char *title)
{
   dialogprop *props;

   temp->prop = malloc(sizeof(dialogprop));
   props = (dialogprop *)temp->prop;

   temp->mask = MASK_ALL & ~(MASK_MOUSEDOWN | MASK_MOUSEUP);
   temp->event = event_window;

   props->titlebar = create_button(temp, 0, temp->dimentions.y - .25, 0.0, temp->dimentions.x, .25, 0.0, 0.0, 0.0, 0, BUTTON, PUSH, title, 1);
   disallow_focus(props->titlebar);
   props->titlebar->mask = MASK_ALL & ~(MASK_MOUSEDOWN | MASK_MOUSEUP | MASK_MOUSEDRAG);
   props->titlebar->event = titlebar_event;
  
   return 0;
}

#undef CORNERSIZE
#undef BORDERSIZE
#undef DBLBORDERSIZE
#define CORNERSIZE 0.16875 // MOVE ME!
#define BORDERSIZE 0.03125 // MOVE ME!
#define DBLBORDERSIZE 0.0625

int create_mini(treenode *temp, char *title)
{
   miniwinprop *props;

   temp->mask = MASK_ALL & ~(MASK_MOUSEDOWN | MASK_MOUSEUP | MASK_RESIZE | MASK_CLICK);
   temp->event = event_window;

   temp->prop = malloc(sizeof(miniwinprop));
   props = (miniwinprop *)temp->prop;

   props->destroying = 0;

// close box
   props->closebox = create_button(temp, 0, temp->dimentions.y - 0.125, 0.0, .125, .125, 0.0, 0.0, 0.0, 0, BUTTON, PUSH, "x", 1);
   disallow_focus(props->closebox);

// title bar
   props->titlebar = create_button(temp, .125, temp->dimentions.y - .125, 0.0, temp->dimentions.x - .25, .125, 0.0, 0.0, 0.0, 0, BUTTON, PUSH, title, 1);
   disallow_focus(props->titlebar);
   props->titlebar->mask = MASK_ALL & ~(MASK_MOUSEDOWN | MASK_MOUSEUP | MASK_MOUSEDRAG);
   props->titlebar->event = titlebar_event;

// minimize button
   props->zoombox = create_button(temp, temp->dimentions.x - .125, temp->dimentions.y - .125, 0.0, .125, .125, 0.0, 0.0, 0.0, 0, BUTTON, PUSH, "_", 1);
   disallow_focus(props->titlebar);

// top border
   props->tborder = create_button(temp, CORNERSIZE - BORDERSIZE, temp->dimentions.y, 0, temp->dimentions.x - (CORNERSIZE + CORNERSIZE) + DBLBORDERSIZE, BORDERSIZE, 0.0, 0.0, 0.0, 0, BUTTON, BORDER, "_", 1);
   disallow_focus(props->tborder);
   props->tborder->mask = MASK_ALL & ~(MASK_MOUSEDOWN | MASK_MOUSEUP | MASK_MOUSEDRAG);
   props->tborder->event = border_event;

// bottom border
   props->bborder = create_button(temp, CORNERSIZE - BORDERSIZE, -BORDERSIZE, 0, temp->dimentions.x - (CORNERSIZE + CORNERSIZE) + DBLBORDERSIZE, BORDERSIZE, 0.0, 0.0, 0.0, 0, BUTTON, BORDER, "_", 1);
   disallow_focus( props->bborder);
   props->bborder->mask = MASK_ALL & ~(MASK_MOUSEDOWN | MASK_MOUSEUP | MASK_MOUSEDRAG);
   props->bborder->event = border_event;

// left border
   props->lborder = create_button(temp, -BORDERSIZE, CORNERSIZE - BORDERSIZE, 0, BORDERSIZE, temp->dimentions.y - (CORNERSIZE + CORNERSIZE) + DBLBORDERSIZE, 0.0, 0.0, 0.0, 0, BUTTON, BORDER, "_", 1);
   disallow_focus(props->lborder);
   props->lborder->mask = MASK_ALL & ~(MASK_MOUSEDOWN | MASK_MOUSEUP | MASK_MOUSEDRAG);
   props->lborder->event = border_event;

// right border
   props->rborder = create_button(temp, temp->dimentions.x, CORNERSIZE - BORDERSIZE, 0, BORDERSIZE, temp->dimentions.y - (CORNERSIZE + CORNERSIZE) + DBLBORDERSIZE, 0.0, 0.0, 0.0, 0, BUTTON, BORDER, "_", 1);
   disallow_focus(props->rborder);
   props->rborder->mask = MASK_ALL & ~(MASK_MOUSEDOWN | MASK_MOUSEUP | MASK_MOUSEDRAG);
   props->rborder->event = border_event;

   props->ltcorner = create_button(temp, -BORDERSIZE, temp->dimentions.y - CORNERSIZE + BORDERSIZE, 0.0, CORNERSIZE, CORNERSIZE, 0.0, 0.0, 0.0, 0, BUTTON, CORNER, "", 1);
   disallow_focus(props->ltcorner);
   props->ltcorner->mask = MASK_ALL & ~(MASK_MOUSEDOWN | MASK_MOUSEUP | MASK_MOUSEDRAG | MASK_COLLIDE);
   props->ltcorner->event = border_event;

// bottom-left corner
   props->blcorner = create_button(temp, -BORDERSIZE, -BORDERSIZE, 0.0, CORNERSIZE, CORNERSIZE, 0.0, 0.0, 0.0, 0, BUTTON, CORNER, "", 1);
   disallow_focus(props->blcorner);
   props->blcorner->mask = MASK_ALL & ~(MASK_MOUSEDOWN | MASK_MOUSEUP | MASK_MOUSEDRAG | MASK_COLLIDE);
   props->blcorner->event = border_event;

// bottom-right corner
   props->brcorner = create_button(temp, temp->dimentions.x - CORNERSIZE + BORDERSIZE, -BORDERSIZE, 0.0, CORNERSIZE, CORNERSIZE, 0.0, 0.0, 0.0, 0, BUTTON, CORNER, "", 1);
   disallow_focus(props->brcorner);
   props->brcorner->mask = MASK_ALL & ~(MASK_MOUSEDOWN | MASK_MOUSEUP | MASK_MOUSEDRAG | MASK_COLLIDE);
   props->brcorner->event = border_event;

// top-right corner
   props->rtcorner = create_button(temp, temp->dimentions.x - CORNERSIZE + BORDERSIZE, temp->dimentions.y - CORNERSIZE + BORDERSIZE, 0.0, CORNERSIZE, CORNERSIZE, 0.0, 0.0, 0.0, 0, BUTTON, CORNER, "", 1);
   disallow_focus(props->rtcorner);
   props->rtcorner->mask = MASK_ALL & ~(MASK_MOUSEDOWN | MASK_MOUSEUP | MASK_MOUSEDRAG | MASK_COLLIDE);
   props->rtcorner->event = border_event;

   return 0;
}

#undef CORNERSIZE
#undef BORDERSIZE
#undef DBLBORDERSIZE
#define CORNERSIZE 0.3375 // MOVE ME!
#define BORDERSIZE 0.0625 // MOVE ME!
#define DBLBORDERSIZE 0.125

void resize_window_regular(treenode *dis)
{
   regwinprop *props;

   props = (regwinprop *)dis->prop;

   props->rborder->origin.x = dis->dimentions.x;
   props->rborder->dimentions.y = dis->dimentions.y - (CORNERSIZE + CORNERSIZE) + DBLBORDERSIZE;
   make_bbox(props->rborder);

   props->lborder->dimentions.y = dis->dimentions.y - (CORNERSIZE + CORNERSIZE) + DBLBORDERSIZE;
   make_bbox(props->lborder);

   props->trcorner->origin.x = dis->dimentions.x - CORNERSIZE + BORDERSIZE;
   props->trcorner->origin.y = dis->dimentions.y - CORNERSIZE + BORDERSIZE;
   make_bbox(props->trcorner);

   props->ltcorner->origin.y = dis->dimentions.y - CORNERSIZE + BORDERSIZE;
   make_bbox(props->ltcorner);

   props->brcorner->origin.x = dis->dimentions.x - CORNERSIZE + BORDERSIZE;
   make_bbox(props->brcorner);

   props->zoombox->origin.x = dis->dimentions.x - 0.25;
   make_bbox(props->zoombox);

   props->tborder->origin.y = dis->dimentions.y;
   props->tborder->dimentions.x = dis->dimentions.x - (CORNERSIZE + CORNERSIZE) + DBLBORDERSIZE;
   make_bbox(props->tborder);

   props->bborder->dimentions.x = dis->dimentions.x - (CORNERSIZE + CORNERSIZE) + DBLBORDERSIZE;
   make_bbox(props->bborder);

   reshape_button(props->titlebar, 3, props->titlebar->origin.x, dis->dimentions.y - 0.25, props->titlebar->origin.z,
                                      dis->dimentions.x - 0.5, 0.25,
                                      0, 0, 0);
//   props->titlebar->dimentions.x = dis->dimentions.x - 0.5;
//   props->titlebar->origin.y = dis->dimentions.y - 0.25;
//   make_bbox(props->titlebar);

   props->closebox->origin.y = dis->dimentions.y - 0.25;
   make_bbox(props->closebox);

   props->zoombox->origin.x = dis->dimentions.x - 0.25;
   props->zoombox->origin.y = dis->dimentions.y - 0.25;
   make_bbox(props->zoombox);
}

#undef CORNERSIZE
#undef BORDERSIZE
#undef DBLBORDERSIZE
#define CORNERSIZE 0.16875 // MOVE ME!
#define BORDERSIZE 0.03125 // MOVE ME!
#define DBLBORDERSIZE 0.0625

void resize_window_mini(treenode *dis)
{
   miniwinprop *props;

   props = (miniwinprop *)dis->prop;

   props->rborder->origin.x = dis->dimentions.x;
   props->rborder->dimentions.y = dis->dimentions.y - (CORNERSIZE + CORNERSIZE) + DBLBORDERSIZE;
   make_bbox(props->rborder);

   props->lborder->dimentions.y = dis->dimentions.y - (CORNERSIZE + CORNERSIZE) + DBLBORDERSIZE;
   make_bbox(props->lborder);

   props->rtcorner->origin.x = dis->dimentions.x - CORNERSIZE + BORDERSIZE;
   props->rtcorner->origin.y = dis->dimentions.y - CORNERSIZE + BORDERSIZE;
   make_bbox(props->rtcorner);

   props->ltcorner->origin.y = dis->dimentions.y - CORNERSIZE + BORDERSIZE;
   make_bbox(props->ltcorner);

   props->brcorner->origin.x = dis->dimentions.x - CORNERSIZE + BORDERSIZE;
   make_bbox(props->brcorner);

   props->zoombox->origin.x = dis->dimentions.x - 0.125;
   make_bbox(props->zoombox);

   props->tborder->origin.y = dis->dimentions.y;
   props->tborder->dimentions.x = dis->dimentions.x - (CORNERSIZE + CORNERSIZE) + DBLBORDERSIZE;
   make_bbox(props->tborder);

   props->bborder->dimentions.x = dis->dimentions.x - (CORNERSIZE + CORNERSIZE) + DBLBORDERSIZE;
   make_bbox(props->bborder);

   reshape_button(props->titlebar, 3, props->titlebar->origin.x, dis->dimentions.y - 0.125, props->titlebar->origin.z,
                                      dis->dimentions.x - 0.25, 0.125,
                                      0, 0, 0);
//   props->titlebar->dimentions.x = dis->dimentions.x - 0.25;
//   props->titlebar->origin.y = dis->dimentions.y - 0.125;
//   make_bbox(props->titlebar);

   props->closebox->origin.y = dis->dimentions.y - 0.125;
   make_bbox(props->closebox);

   props->zoombox->origin.x = dis->dimentions.x - 0.125;
   props->zoombox->origin.y = dis->dimentions.y - 0.125;
   make_bbox(props->zoombox);
}

void resize_window_dialog(treenode *dis)
{
}
