/* menu.c - 
 * menu routines for Synapse
 * Copyright(c) 1998-99 Objective Reality Computer.
 * By Nick McKinney and Max Harris. 
 */

#include <stdio.h> // for printf()
#include <stdlib.h> // for malloc(), free()
#include "draw.h" // for border()
#include "button.h" // for make_button()
#include "tree.h"  // for treenode
#include "GL/glut.h" // for glutSolidCube()
#include "nodetypes.h"  // for all object types
#include "coord.h"
#include "menu.h"

//int event_menu(treenode *object, int event, int dsize, void *data)
int event_menu(struct treenode *target, struct treenode *node, int event, int modifier, point *where, unsigned long usec)
{
   switch(event)
   {
      default:
         printf("unrecognized event %d\n", event);
         break;
   }

   return 0;
}

void hide_menu(treenode *object)
{
   object = object->child;
   object->visible = FALSE;
}

void show_menu(treenode *object)
{
   object = object->child;
   object->visible = TRUE;
}

void destroy_menu(treenode *dis)
{
  return;
}

void sort_titles(treenode *six)
{
  GLfloat xpos = 0.0;
  GLfloat ymax = 0.0;
  int x;

  for(x = 0; x < ((sixprop *)(six->prop))->items; x++)
  {
    if(ymax < ((sixprop *)(six->prop))->titlebuttons[x]->dimentions.y)
      ymax = ((sixprop *)(six->prop))->titlebuttons[x]->dimentions.y;
    ((sixprop *)(six->prop))->titlebuttons[x]->origin.x = xpos;
    ((sixprop *)(six->prop))->item[x]->origin.x = xpos;
    ((sixprop *)(six->prop))->item[x]->origin.y = -(((sixprop *)(six->prop))->item[x]->dimentions.y);
    xpos += ((sixprop *)(six->prop))->titlebuttons[x]->dimentions.x;
  }
  for(x = 0; x < ((sixprop *)(six->prop))->items; x++)
  {
    ((sixprop *)(six->prop))->titlebuttons[x]->dimentions.y = ymax;
  }
  six->dimentions.x = xpos;
  six->dimentions.y = ymax;
  six->dimentions.z = DBLBORDERSCALE;
  make_bbox(six);
}

void sort_menu_titles(treenode *thing)
{
  GLfloat ypos = 0.0;
  GLfloat xmax = 0.0;
  int x;

  for(x = ((menu *)(thing->prop))->items - 1; x >= 0; x--)
  {

    if(xmax < ((menu *)(thing->prop))->item[x]->dimentions.x)
      xmax = ((menu *)(thing->prop))->item[x]->dimentions.x;

    ((menu *)(thing->prop))->item[x]->origin.y = ypos;
    ypos += ((menu *)(thing->prop))->item[x]->dimentions.y;
    ((menu *)(thing->prop))->item[x]->origin.x = 0;
  }

  for(x = 0; x < ((menu *)(thing->prop))->items; x++)
  {
    ((menu *)(thing->prop))->item[x]->dimentions.x = xmax;
  }
  thing->dimentions.y = ypos;
  thing->dimentions.x = xmax;
  thing->dimentions.z = DBLBORDERSCALE;
  make_bbox(thing);
}

int add_item_to_menu(treenode *thing, int after, char *title)
{
  int i;

  if(!thing)
    return 1;

  if(thing->subtype != FACE)
    return 1;

  if(after > ((menu *)(thing->prop))->items || after < 0)
    after = ((menu *)(thing->prop))->items;
  else
  {
    for(i = ((menu *)(thing->prop))->items; i > after; i--)
      ((menu *)(thing->prop))->item[i] = ((menu *)(thing->prop))->item[i - 1];
    after--;
  }
  printf("Adding \"%s\" in slot %d\n", title, after);

  ((menu *)(thing->prop))->item[after] = create_button(thing, 0.0, 0.0, 0.0, 1.0, .25, 0, 0, 0, thing->color, BUTTON, MENUBUTTON, title, 1);
  ((menu *)(thing->prop))->items++;

  sort_menu_titles(thing);
  return 0;
}

int add_item_to_six(treenode *six, int after, treenode *menu, char *title)
{
  int i;

  if(!six)
    return 1;
  if(!menu)
    return 1;

  if(menu->subtype != FACE)
    return 1;

  if(after > ((sixprop *)(six->prop))->items || after < 0)
    after = ((sixprop *)(six->prop))->items;
  else
  {
    for(i = ((sixprop *)(six->prop))->items; i > after; i--)
    {
      ((sixprop *)(six->prop))->item[i] = ((sixprop *)(six->prop))->item[i - 1];
      ((sixprop *)(six->prop))->titlebuttons[i] = ((sixprop *)(six->prop))->titlebuttons[i - 1];
    }
    after--;
  }
  ((sixprop *)(six->prop))->item[after] = menu;
  ((sixprop *)(six->prop))->items++;

  ((sixprop *)(six->prop))->titlebuttons[after] = create_button(six, 0.0, 0.0, 0.0, .5, .25, 0, 0, 0, six->color, BUTTON, MENUBUTTON, title, 1);

  sort_titles(six);
  return 0;
}

int create_six(treenode *this)
{
   int x;

// butt = create_button(this, .1, this->dimentions.y - 0.25, 0, 1, .25, 0, 0, 0, 0, BUTTON, PUSH, title, 1);
// disallow_focus(butt);

// butt->eventmask &= ~(MASK_MOUSEDOWN | MASK_MOUSEUP | MASK_MOUSEDRAG);
// butt->event_mousedown = menubar_mousedown;
// butt->event_mouseup = menubar_mouseup;
// butt->event_mousedrag = menubar_mousedrag;
   this->prop = malloc(sizeof(sixprop));
   if(!this->prop)
      return 1;

   ((sixprop *)(this->prop))->items = 0;
   for(x = 0; x < MAX_SIXITEMS; x++)
      ((sixprop *)(this->prop))->item[x] = (treenode *) 0;

   return 0;
}

int menubar_mousedown(struct treenode* thing, point *where, int button, int modifiers)
{
   show_menu(thing);
   return 0;
}

int menubar_mousedrag(struct treenode *thing, point *offset)
{
   point dar;

   dar = *offset;
   dar.x += thing->parent->origin.x;
   dar.y += thing->parent->origin.y;
   dar.z += thing->parent->origin.z;

   printf("dar\n");
// move_window(thing->parent, &dar);

   return 0;
}

int menubar_mouseup(struct treenode *thing, point *where)
{
   hide_menu(thing);
   return 0;
}

int create_face(treenode *this)
{
   int x;

// this->visible = FALSE;
   this->prop = malloc(sizeof(menu));
   if(!this->prop)
      return 1;

   ((menu *)(this->prop))->items = 0;
   for(x = 0; x < MAX_MENUITEMS; x++)
      ((menu *)(this->prop))->item[x] = (treenode *) 0;

   return 0;
}


treenode *create_menu(treenode *parent, GLfloat x, GLfloat y, GLfloat z,
GLfloat rotx, GLfloat roty, GLfloat rotz, int subtype, GLfloat *color)
{
   treenode *temp;

   temp = create_child(parent, sizeof(treenode));

   if (!temp)
      return(0);
 
   temp->prop = malloc(sizeof(menu));

   temp->type = MENU;
   temp->origin.x = x;
   temp->origin.y = y;
   temp->origin.z = z;
   temp->dimentions.x = 0;  // these are calculated when items are
   temp->dimentions.y = 0;  // added or removed
   temp->dimentions.z = 0;
   temp->rotation.x = rotx;
   temp->rotation.y = roty;
   temp->rotation.z = rotz;
   temp->visible = TRUE;
   temp->color[0] = color[0];
   temp->color[1] = color[1];
   temp->color[2] = color[2];
   temp->color[3] = color[3];
   temp->focusable = TRUE;
   temp->subtype = subtype;
   temp->mask = MASK_ALL;
   temp->event = event_menu;

   switch (subtype)
   {
      case SIX:
         if (create_six(temp))
         {
            destroy_node(temp);
            return(0);
         }
         break;

      case FACE:
         if (create_face(temp))
         {
            destroy_node(temp);
            return(0);
         }
         break;
 
      default:
         destroy_node(temp);
         return(0);
   }
 
   make_bbox(temp);
   return(temp);
}

void draw_menu(treenode *dis)
{
   return;
   if(dis->dimentions.x == 0.0 || dis->dimentions.y == 0.0)
      return;

   switch(dis->subtype)
   {
      case FACE:
         // draw two polys to fill in the window
         glBegin(GL_QUADS);
            glNormal3f(-1.0, 0.0, 0.0);
            glVertex3f(0.0, 0.0, 0.0);
            glVertex3f(0.0, 0.0, dis->dimentions.z);
            glVertex3f(0.0, dis->dimentions.y, dis->dimentions.z);
            glVertex3f(0.0, dis->dimentions.y, 0.0);

            glNormal3f(1.0, 0.0, 0.0);
            glVertex3f(dis->dimentions.x, 0.0, dis->dimentions.z);
            glVertex3f(dis->dimentions.x, 0.0, 0.0);
            glVertex3f(dis->dimentions.x, dis->dimentions.y, 0.0);
            glVertex3f(dis->dimentions.x, dis->dimentions.y, dis->dimentions.z);

            glNormal3f(0.0, 1.0, 0.0);
            glVertex3f(dis->dimentions.x, dis->dimentions.y, 0.0);
            glVertex3f(0.0, dis->dimentions.y, 0.0);
            glVertex3f(0.0, dis->dimentions.y, dis->dimentions.z);
            glVertex3f(dis->dimentions.x, dis->dimentions.y, dis->dimentions.z);

            glNormal3f(0.0, -1.0, 0.0);
            glVertex3f(0.0, 0.0, 0.0);
            glVertex3f(dis->dimentions.x, 0.0, 0.0);
            glVertex3f(dis->dimentions.x, 0.0, dis->dimentions.z);
            glVertex3f(0.0, 0.0, dis->dimentions.z);
         glEnd();
         break;

      case SIX:
         // draw two polys to fill in the window
         glBegin(GL_QUADS); 
            glNormal3f(-1.0, 0.0, 0.0);
            glVertex3f(-0.04, 0.0, 0.0);
            glVertex3f(-0.04, 0.0, dis->dimentions.z);
            glVertex3f(-0.04, dis->dimentions.y + 0.04, dis->dimentions.z);
            glVertex3f(-0.04, dis->dimentions.y + 0.04, 0.0);
            
            glNormal3f(1.0, 0.0, 0.0);
            glVertex3f(dis->dimentions.x + 0.04, 0.0, dis->dimentions.z);
            glVertex3f(dis->dimentions.x + 0.04, 0.0, 0.0);
            glVertex3f(dis->dimentions.x + 0.04, dis->dimentions.y + 0.04, 0.0);
            glVertex3f(dis->dimentions.x + 0.04, dis->dimentions.y + 0.04, dis->dimentions.z);
            
            glNormal3f(0.0, 1.0, 0.0);
            glVertex3f(dis->dimentions.x + 0.04, dis->dimentions.y + 0.04, 0.0);
            glVertex3f(-0.04, dis->dimentions.y + 0.04, 0.0);
            glVertex3f(-0.04, dis->dimentions.y + 0.04, dis->dimentions.z);
            glVertex3f(dis->dimentions.x + 0.04, dis->dimentions.y + 0.04, dis->dimentions.z);
            
            glNormal3f(0.0, -1.0, 0.0);
            glVertex3f(-0.04, 0.0, 0.0);
            glVertex3f(dis->dimentions.x + 0.04, 0.0, 0.0);
            glVertex3f(dis->dimentions.x + 0.04, 0.0, dis->dimentions.z);
            glVertex3f(-0.04, 0.0, dis->dimentions.z);
         glEnd();
         break;
   }
}
