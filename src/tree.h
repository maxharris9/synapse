#include "synapse.h"
#include "nodetypes.h"

#ifndef _TREE_H_
#define _TREE_H_

typedef struct treenode {
   struct treenode *next;
   struct treenode *prev;
   struct treenode *child;
   struct treenode *parent;
   struct treenode *activenode;
   GLfloat color[4];
   point dimentions;
   point rotation;
   point origin;
   point b_origin;
   point b_dimentions;
   double bbox[24];  // 0-11=front, 12-23=back, 0,3,6,9=bl,br,tr,tl, 0,1,2=x,y,z
   int focusable;
   int visible;
   int type;
   int subtype;
   int autoresolve;
   int resizable;
   int mask;
   int (*event) (struct treenode *target, struct treenode *node, int event, int modifier, point *where, unsigned long usec);
   int dirty;  // if TRUE, then a child has been created since this node's last attempt to cache things
               // like bounding boxes and display lists
   struct treenode *plugin;  // if node belongs to a plugin, this points to the plugin
   void *prop;
} treenode;

extern treenode rootnode;

void set_active_node(treenode *dar);
void showtree(void);
void inittree(void);
treenode * create_child(treenode *parent, int size);
treenode * create_sibling(treenode *brother, int size);
int destroy_object(treenode *node);
void destroy_node(treenode *node);
treenode *find_active_node(treenode *top, int type);
void allow_focus(treenode *object);
void disallow_focus(treenode *object);
void cycle_focus(treenode *top, int level);
int color_object(treenode *thing, GLfloat *color);
int set_active(treenode *object);

#endif
