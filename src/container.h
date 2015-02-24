#ifndef _CONTAINER_H_
#define _CONTAINER_H_

#include <GL/gl.h>
#include "synapse.h"
#include "tree.h"

#define HIDDEN 0
#define VISIBLE 1

typedef struct collision
{
  struct collision *next;
  struct collision *prev;
  treenode *thing;
  point at;
} collision;

void drawtree(treenode *parent);
treenode * ray_intersect_nodes(treenode *top, double orig[3], double dir[3], double *t, double *u, double *v);
int ray_intersect_node(treenode *thing, double orig[3], double dir[3], double *t, double *u, double *v);
void rotate_tri(double tri[9], double rotx, double roty, double rotz);
void rotate_point(double tri[3], double rotx, double roty, double rotz);
treenode * resolve_to_node(treenode * top, double *x, double *y);

#endif
