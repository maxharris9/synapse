/* bar.c - 
 * bar routines for Synapse
 * Copyright(c) 1998-99 Objective Reality Computer.
 * By Nick McKinney and Max Harris. 
 */

#include <stdio.h> // for printf()
#include <stdlib.h> // for malloc(), free()
#include "draw.h" // for border()
#include "button.h" // for make_button()
#include "bar.h"
#include "barmanager.h" // for create_bar derivitives
#include "tree.h"  // for treenode
#include "GL/glut.h" // for glutSolidCube()
#include "nodetypes.h"  // for all object types
#include "coord.h"

int get_progbar(treenode *thing)
{
  if(!thing)
    return -1;
  if(thing->type != BAR)
    return -1;
  if(thing->subtype != PROGRESS)
    return -1;

  return ((bar *)(thing->prop))->value;
}

int get_scrollbar_value(treenode *thing)
{
  if(!thing)
    return -1;
  if(thing->type != BAR)
    return -1;
  if(thing->subtype != SCROLL)
    return -1;

  return ((bar *)(((scrollbarprop *)(thing->prop))->slider->prop))->value;
}

int change_slider_value(treenode *thing, int value)
{
  if(!thing)
    return -1;
  if(thing->type != BAR)
    return -1;
  if(thing->subtype != SLIDER)
    return -1;

  change_slider_values(thing, ((bar *)(thing->prop))->min, ((bar *)(thing->prop))->min, ((bar *)(thing->prop))->min, value);
  return 0;
}

int get_slider_value(treenode *thing)
{
  if(!thing)
    return -1;
  if(thing->type != BAR)
    return -1;
  if(thing->subtype != SLIDER)
    return -1;

  return ((bar *)(thing->prop))->value;
}

int resize_bar(treenode *thing, GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height, GLfloat rotx, GLfloat roty, GLfloat rotz)
{
  switch(thing->subtype)
  {
    case SCROLL:
      return resize_scrollbar(thing, x, y, z, width, height, rotx, roty, rotz);

    default:
      return -1;
  }
}

int event_bar(struct treenode *target, struct treenode *node, int event, int modifier, point *where, unsigned long usec)
{
  switch(event)
  {
    case EVENT_COLLIDE:
       return 1;
       break;

    default:
       return 1;
       break;
  }
  return 0;
}

void destroy_bar(treenode *dis)
{
  free(dis->prop);
  dis->prop = 0;
}

treenode *create_bar(treenode *parent, GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height,
       GLfloat rotx, GLfloat roty, GLfloat rotz, int min, int max, int value, int window, GLfloat *color, int type, int subtype, int visible)
{
  treenode *temp;

  temp = create_child(parent, sizeof(treenode));
  if(!temp)
    return 0;

  temp->type = BAR;
  temp->origin.x = x;
  temp->origin.y = y;
  temp->origin.z = z;
  temp->dimentions.x = width;
  temp->dimentions.y = height;
  temp->rotation.x = rotx;
  temp->rotation.y = roty;
  temp->rotation.z = rotz;
  temp->visible = visible;

  if(parent->type == TEXT && parent->subtype == MULTIBOX)
  {
    temp->color[0] = 0.9;
    temp->color[1] = 0.9;
    temp->color[2] = 0.9;
    temp->color[3] = 1.0;
  }
  else
  {
    temp->color[0] = parent->color[0] + 0.1;
    temp->color[1] = parent->color[1] + 0.1;
    temp->color[2] = parent->color[2] + 0.1;
    temp->color[3] = parent->color[3];
  }

  temp->mask = MASK_ALL;
  temp->event = event_bar;

  temp->focusable = TRUE;
  temp->subtype = subtype;

  temp->prop = malloc(sizeof(bar));
  ((bar *)(temp->prop))->value = 0; 
  ((bar *)(temp->prop))->max = temp->dimentions.y; 
  ((bar *)(temp->prop))->min = 0;

  switch(subtype)
  {
    case PROGRESS:
      if (create_progress(temp))
      {
         destroy_node(temp);
         return 0;
      }
      break;

    case SLIDER:
    case VSLIDER:
      if (create_slider(temp, min, max, window, value))
      {
         destroy_node(temp);
         return 0;
      }
      break;

    case SCROLL:
    case VSCROLL:
      if (create_scroll(temp, min, max, window, value))
      {
         destroy_node(temp);
         return 0;
      }
      break;

    default:
      destroy_node(temp);
      return 0;
  }

  make_bbox(temp->parent);

  return temp;
}

void move_slider(treenode *dis, point *pnt)
{
   point p;
   point *pt = &p;
   treenode *thumb = dis->child;
   bar *props;

   p = *pnt;
   props = (bar *)(dis->prop);

   switch(dis->subtype)
   {
      case SLIDER:
         thumb->origin.x = pt->x;

         if(!change_slider_values(dis, props->min, props->max, props->window,
//                     (int)(thumb->origin.x / (dis->dimentions.x) * (props->max - props->min))    ))
                     (int)(props->min + ((props->max - props->min) * (thumb->origin.x / dis->dimentions.x)))    ))
           send_node_event(dis, dis, EVENT_CHANGE, props->value, NULL);
         break;

      case VSLIDER:
         thumb->origin.y = pt->y;

         if(!change_slider_values(dis, props->min, props->max, props->window, 
//                   (int)(((-(thumb->origin.y + thumb->dimentions.y) * (props->max - props->min))/dis->dimentions.y) + props->max - props->min)))
                     (int)(props->max - ((props->max - props->min)*(thumb->origin.y + thumb->dimentions.y))/(dis->dimentions.y))))
           send_node_event(dis, dis, EVENT_CHANGE, props->value, NULL);
         break;
   }
   make_bbox(dis);
}

void draw_tray(treenode *dis)
{
  point origin;
  point dim;

  origin.x = origin.y = 0;
  origin.z = BORDERSCALE;
  dim.x = dis->dimentions.x; 
  dim.y = dis->dimentions.y;
  dim.z = -BORDERSCALE;

  new_border(&origin, &dim, HALFBORDERSCALE, FALSE, 0.05);

  glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    glVertex3f(HALFBORDERSCALE, HALFBORDERSCALE, 0.0);
    glVertex3f(dis->dimentions.x - HALFBORDERSCALE, HALFBORDERSCALE, 0.0);
    glVertex3f(dis->dimentions.x - HALFBORDERSCALE, dis->dimentions.y - HALFBORDERSCALE, 0.0);
    glVertex3f(HALFBORDERSCALE, dis->dimentions.y - HALFBORDERSCALE, 0.0);
  glEnd();
}

void draw_bar(treenode *dis)
{
   if (dis->subtype == SLIDER || dis->subtype == VSLIDER)
   {
//    glColor3f(0.2, 0.2, 0.2);
      draw_tray(dis);
   }

   if (dis->subtype == PROGRESS)
   {
//    glColor3f(0.2, 0.2, 0.2);
      glBegin(GL_QUADS);
         glNormal3f(0, 0, 1);
         glVertex3f(0.0, 0.0, 0.0);
         glVertex3f(dis->dimentions.x, 0.0, 0.0);
         glVertex3f(dis->dimentions.x, dis->dimentions.y, 0.0);
         glVertex3f(0.0, dis->dimentions.y, 0.0);
      glEnd();
   }

// if (dis->parent->activenode == (treenode *)dis)
//    glColor3f(1.0, 0.0, 0.0);
}
