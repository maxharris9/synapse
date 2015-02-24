/* glyph.c -  
 * glyph-handling routines for System 1.0. 
 * Copyright(c) 1998 Objective Reality Computer.  
 * By Nick McKinney and Max Harris. 
 */

#include <stdio.h> // for NULL in make_button()
#include <stdlib.h> // for exit
#include "glyph.h" // for header
#include "event.h"
#include "container.h"
#include "coord.h"
#include "reigon.h"

void move_glyph(treenode *dis, point *pt);

int event_glyph(struct treenode *target, struct treenode *node, int event, int modifier, point *where, unsigned long usec)
{
   switch(event)
   {
      case EVENT_MOUSEDOWN:
           send_node_event(target->parent, target, EVENT_CLICK, 0, where);
         return 0;
         break;

      case EVENT_MOUSEUP:
         return 0;
        break;

      case EVENT_MOUSEDRAG:
         move_glyph(target, where);
         break;
       
   }
   return(0);
}

void move_glyph(treenode *dis, point *pt)
{
   dis->origin.x += pt->x;
   dis->origin.y += pt->y;
   dis->origin.z += pt->z;
   make_bbox(dis);
}

void draw_glyph(treenode *dis)
{
   glCallList(dis->subtype);
}

int set_glyph_lock(treenode *dis, int locked)
{
   if(!dis)
      return -1;
   if(dis->type != GLYPH)
      return -1;

   if(!locked)
      dis->mask &= ~MASK_MOUSEDRAG;
   else
      dis->mask |= MASK_MOUSEDRAG;

   return 0;
}

treenode *create_glyph(treenode *parent, GLfloat x, GLfloat y, GLfloat z,
GLfloat width, GLfloat height, GLfloat rotx, GLfloat roty, GLfloat rotz,
int flags, GLfloat *color, int dlist, int visible)
{
   treenode *temp;

   temp = create_child(parent, sizeof(treenode));
   if(!temp)
   {
      return 0;
      exit(1);
   }

   temp->visible = visible;
   temp->origin.x = x;
   temp->origin.y = y;
   temp->origin.z = z;
   temp->dimentions.x = width;
   temp->dimentions.y = height;
   temp->rotation.x = rotx;
   temp->rotation.y = roty;
   temp->rotation.z = rotz;

   temp->mask = MASK_ALL & ~(MASK_MOUSEDOWN | MASK_MOUSEUP);
   temp->event = event_glyph;

   if (color[0] == 2)
   {
      temp->color[0] = parent->color[0] + 0.1;
      temp->color[1] = parent->color[1] + 0.1;
      temp->color[2] = parent->color[2] + 0.1;
      temp->color[3] = parent->color[3];
   }

   if (color[0] != 2) 
   {   
      temp->color[0] = color[0];
      temp->color[1] = color[1];
      temp->color[2] = color[2];
      temp->color[3] = color[3];
   }

   temp->type = GLYPH;
   if(glIsList(dlist) != GL_TRUE)
   {
      destroy_node(temp);
      return 0;
   }
   temp->subtype = dlist;

   make_bbox(temp);

   if(temp->parent->type == REIGON)
      calc_control_bounds(temp->parent);

   return temp;
}

void destroy_glyph(treenode *thing)
{
  return;
}
