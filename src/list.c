/* list.c -
 * list routines for Synapse
 * Copyright(c) 1998-99 Objective Reality Computer.
 * By Nick McKinney and Max Harris.
 */

#include <GL/glu.h> // for gluTesselate()
#include <stdio.h> // for printf()
#include <stdlib.h> // for malloc(), free()
#include "draw.h" // for border()
#include "button.h" // for make_button()
#include "window.h"
#include "wmanager.h" // for create_window derivitives
#include "tree.h"  // for treenode
#include "nodetypes.h"  // for all object types
#include "coord.h"
#include "font.h" // for font texmap playing with
#include "bar.h"
#include "event.h" // for send_app_event
#include "reigon.h" // for create_reigon
#include "list.h" // duh
#include "coord.h" // for make_bbox
#include "glyph.h" // for create_glyph

void draw_list(treenode *this)
{
//   drawtree(this);
}

treenode *create_list(treenode *parent, GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height, 
                      GLfloat rotx, GLfloat roty, GLfloat rotz, GLfloat *color, int columns, int visible)
{
   treenode *temp, *region;
   int j, zappa; // moon unit alpha...

   temp = create_child(parent, sizeof(treenode));
   if (!temp)
      return 0;

   temp->type = LIST;
   temp->origin.x = x;
   temp->origin.y = y;
   temp->origin.z = z;

   temp->dimentions.x = width;
   temp->dimentions.y = height;
   temp->dimentions.z = 0;
   temp->rotation.x = rotx;
   temp->rotation.y = roty;
   temp->rotation.z = rotz;
   temp->visible = 1;

   temp->color[0] = parent->color[0];
   temp->color[1] = parent->color[1];
   temp->color[2] = parent->color[2];
   temp->color[3] = parent->color[3];

   temp->focusable = TRUE;
   temp->subtype = NONE;
   temp->prop = malloc(sizeof(list));

   ((list *)(temp->prop))->sortbox = create_button(temp, temp->dimentions.x - 0.25, temp->dimentions.y - 0.25, 0.0, .25, .25, 0.0, 0.0, 0.0, 0, BUTTON, PUSH, "r", 1);

   ((list *)(temp->prop))->region = create_reigon(temp, 0, 0, 0, temp->dimentions.x, temp->dimentions.y - 0.25, 0, 0, 0, 1);
   region = ((list *)(temp->prop))->region;

   {
      GLfloat color[4] = {2, 2, 2, 2};
   create_glyph(region, 0, 0, 0, 0.25, 0.25, 0, 0, 0, 0, color, 21, 1);
   }

   for (j = 0; j < columns; j++)
   {
      if (j != 0)
         create_button(temp, ((temp->dimentions.x - 0.25) / columns) * j, temp->dimentions.y - 0.25, 0.0, ((temp->dimentions.x - 0.25) / columns), 0.25, 0.0, 0.0, 0.0, 0, BUTTON, PUSH, "Column", 1);
      else
         create_button(temp, 0, temp->dimentions.y - 0.25, 0.0, ((temp->dimentions.x - 0.25) / columns), 0.25, 0.0, 0.0, 0.0, 0, BUTTON, PUSH, "Column", 1);
   }

   for (zappa = 0; zappa < 2; zappa++)
   {
      for (j = 0; j < columns; j++)
      {
//       if (j != 0)
//          create_button(region, ((temp->dimentions.x - 0.25) / columns) * j, temp->dimentions.y - (0.5 + (0.25 * zappa)), 0.0, ((temp->dimentions.x - 0.25) / columns), 0.25, 0.0, 0.0, 0.0, 0, BUTTON, PUSH, "zoom", 1);
//       else
//          create_button(region, 0, temp->dimentions.y - (0.5 + (0.25 * zappa)), 0.0, ((temp->dimentions.x - 0.25) / columns), 0.25, 0.0, 0.0, 0.0, 0, BUTTON, PUSH, "zoom", 1);
      }
   }

   make_bbox(temp);
   return temp;
}

item *new_item(item *finder)
{
  item *temp;

  if(!finder)
    return 0;

  temp = finder;
  finder = malloc(sizeof(item));
  if(!finder)
  {
    finder = temp;
    return 0;
  }

  finder->prev = 0;
  finder->next = temp;
  if(temp)
    temp->prev = finder;

  finder->data = NULL;

  return finder;
}
/*
void delete_item(item *thisone, finderwin *finder)
{
  if(!thisone || !finder)
    return;

  if(thisone->next)
    thisone->next->prev = thisone->prev;
  if(thisone->prev)
    thisone->prev->next = thisone->next;

  if(finder->items == thisone)
    finder->items = finders->items->next;

  free(thisone);

  return;
}*/
