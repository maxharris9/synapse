/* utility functions for tree maintenance */

#include "tree.h"
#include "nodetypes.h"
#include "display.h"  // for camera
#include <stdlib.h> // for malloc()
#include <stdio.h>  // for printf()

#include "window.h"
#include "button.h"
#include "text.h"
#include "reigon.h"
#include "bar.h"
#include "conn.h"
#include "api_defs.h"
#include "input.h"
#include "plugin.h"

treenode rootnode;

// maximum tree traversal in showtree
#define LEV_THRESHOLD 8

void set_active_node(treenode *dar)
{
  while(dar->parent)
  {
    dar->parent->activenode = dar;
    dar = dar->parent;
  }
}

void showbranch(treenode *top, int level)
{
  treenode *thing;
  treenode *tmp;

  int x, y;

  thing = top->child;
  while(thing)
  {
    for(x = 0; x < level; x++)
    {
      tmp = thing;
      for(y = x; y < level; y++)
        tmp = tmp->parent;
      if(tmp->next)
        printf("  |");
      else
        printf("   ");
    }

    if(!thing->next)
      printf("  `");
    else
      printf("  |");

    printf("- ");
    printnodetype(thing->type);
    printf(", ");
    printnodesubtype(thing->subtype);

    if(thing->visible)
      printf("  V");
    else
      printf("  -");

    if(thing->parent->activenode == thing)
      printf("A");
    else
      printf("-");

    if(thing->autoresolve)
      printf("R");
    else
      printf("-");
    
    if(thing->type != CAMERA)
      printf("   (%.2f, %.2f, %.2f) %.2fx%.2fx%.2f", thing->origin.x, thing->origin.y, thing->origin.z, thing->dimentions.x, thing->dimentions.y, thing->dimentions.z);
    else
      printf("   (%.2f, %.2f, %.2f) [%.2f, %.2f, %.2f]", 
               ((camera *)(thing))->location.x,
               ((camera *)(thing))->location.y,
               ((camera *)(thing))->location.z,
               ((camera *)(thing))->rotation.x,
               ((camera *)(thing))->rotation.y,
               ((camera *)(thing))->rotation.z);
    printf(" %X\n", (unsigned int) thing);
    if(thing->child && level < LEV_THRESHOLD)
      showbranch(thing, level + 1);
    thing = thing->next;
  }
}
 
int set_active(treenode *object)
{
   object->parent->activenode = object;
   return(0);
}
 
void showtree(void)
{
  printf("root\n");
  showbranch(&rootnode, 0);
}

void allow_focus(treenode *object)
{
   object->focusable = TRUE;
}

void disallow_focus(treenode *object)
{
   object->focusable = FALSE;
}

void inittree(void)
{
  rootnode.next = rootnode.prev = 0;
  rootnode.parent = rootnode.child = 0;
  rootnode.plugin = 0;
  rootnode.type = ROOT;
  rootnode.autoresolve = TRUE;      // all automatic behavior
  rootnode.mask = MASK_ALL;  // standard behavior for all
}

void initnode(treenode *node)
{
  node->origin.x = node->origin.y = node->origin.z = 0;
  node->dimentions.x = node->dimentions.y = node->dimentions.z = 0;
  node->rotation.x = node->rotation.y = node->rotation.z = 0;
  node->b_origin.x = node->b_origin.y = node->b_origin.z = 0;
  node->b_dimentions.x = node->b_dimentions.y = node->b_dimentions.z = 0;
  node->autoresolve = TRUE;      // all automatic behavior
  node->mask = MASK_ALL;  // for sanity
  node->dirty = TRUE;
  node->visible = TRUE;
  node->parent->dirty = TRUE;  // should setting parent->dirty be *this* standard?
  node->event = (void *)0;
  node->plugin = 0;
}

treenode * create_child(treenode *parent, int size)
{
  if(!parent || size < sizeof(treenode))
    return 0;

  if(parent->child)
    return create_sibling(parent->child, size);

  parent->child = malloc(size);
  if(!parent->child)
    return 0;

  parent->child->parent = parent;
  parent->child->next = parent->child->prev = parent->child->child = 0;

  parent->activenode = parent->child;

  initnode(parent->child);

  return parent->child;
}

treenode * create_sibling(treenode *brother, int size)
{
  treenode *temp;

  if(!brother || size < sizeof(treenode))
    return 0;

  temp = malloc(size);
  if(!temp)
    return 0;

  temp->next = brother;
  temp->prev = brother->prev;
  brother->prev = temp;
  temp->parent = brother->parent;
  temp->child = 0;
  initnode(temp);

  while(temp->parent->child->prev)
    temp->parent->child = temp->parent->child->prev;  // rewind the list

  return temp;
}

int destroy_object(treenode *node)
{
   destroy_node(node);
   return 0;
}

void destroy_node(treenode *node)
{
  treenode *temp;

  if(!node || (node == &rootnode))
    return;

  temp = node->child;
  while(temp)
  {
    destroy_node(temp);
    temp = node->child;
  }

  temp = node;
  while(temp != &rootnode)
  {
    temp = temp->parent;
    if(temp->type == APP && !((connection *)(temp))->kaput)
    {
      int msg[3];
      msg[0] = sizeof(int)*3;
      msg[1] = API_DESTROY_OBJECT;
      msg[2] = (int) node;
      sendmessage((connection *)temp, msg, msg[0]);
      break;
    }
  }

  if(node->type >= FIRST_NODE)
  {
    if(!node->plugin)
      node->plugin = find_plugin_by_nodetype(node->type);
    send_plugin_destroy((plugin_module *)(node->plugin->prop), node);
  }
  else
  switch(node->type)
  {
    case FLATWINDOW:
      destroy_window(node);
      break;

    case BUTTON:
      destroy_button(node);
      break;

    case BAR:
      destroy_bar(node);
      break;

    case TEXT:
      destroy_text(node);
      break;

    case REIGON:
      destroy_reigon(node);
      break;

    default:
      break;
  }

  if(node == get_grabbed_node())
  {
    set_grabbed_node((treenode *) 0);
  }

  if(node->parent->child == node)
  {
    if(node->prev)
      node->parent->child = node->prev;
    else
      node->parent->child = node->next; // if node is an only child, this one should set child to NULL
  }

  if(node->prev)
    node->prev->next = node->next;

  if(node->next)
    node->next->prev = node->prev;

  free(node);
}

void cycle_focus(treenode *thing, int level)
{
   int counter = 0;

   while (thing && counter < level)
   {
      thing = thing->child;
      printf("%d\n", counter);
      counter++;
   }

   showbranch(thing, 1);
}

int color_object(treenode *thing, GLfloat *color)
{
   if (thing->subtype == PROGRESS || thing->subtype == SLIDER)
      thing = thing->child;

   if (thing->subtype == SCROLL)
   {
      thing = thing->child->child;
      thing->color[0] = color[0];
      thing->color[1] = color[1];
      thing->color[2] = color[2];
      thing->color[3] = color[3];
      thing = thing->child;
   }

   thing->color[0] = color[0];
   thing->color[1] = color[1];
   thing->color[2] = color[2];
   thing->color[3] = color[3];

   return(0);
}
