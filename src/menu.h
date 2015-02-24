#ifndef _MENU_H_
#define _MENU_H_

#include "synapse.h"
#include "tritri.h"
#include "container.h"
#include "event.h"
#include "tree.h"

#define MAX_SIXITEMS 256
#define MAX_MENUITEMS 256

typedef struct
{
   int items;
   treenode *item[MAX_MENUITEMS];
   int entries;
   treenode *faces[MAX_MENUITEMS];
   void (*call[MAX_MENUITEMS]) (treenode *dis);
} menu;

typedef struct
{
   int items;
   treenode *item[MAX_SIXITEMS];
   treenode *titlebuttons[MAX_SIXITEMS];
} sixprop;

void recalc_six_dimensions(treenode *thing);
void recalc_menu_dimensions(treenode *thing);
void hide_menu(treenode *object);
void show_menu(treenode *object);

treenode *create_menu(treenode *parent, GLfloat x, GLfloat y, GLfloat z,
GLfloat rotx, GLfloat roty, GLfloat rotz, int subtype, GLfloat *color);

int add_item_to_six(treenode *six, int after, treenode *menu, char *title);
int add_item_to_menu(treenode *thing, int after, char *title);
void draw_menu(treenode *dis);
void destroy_menu(treenode *dis);
int event_menu(struct treenode *target, struct treenode *node, int event, int modifier, point *where, unsigned long usec);
int menubar_mousedown(struct treenode* thing, point *where, int button, int modifiers);
int menubar_mousedrag(struct treenode *thing, point *offset);
int menubar_mouseup(struct treenode *thing, point *where);

#endif
