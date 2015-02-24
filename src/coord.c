/* utility functions for coordinate system transformation */
/* Copyright(c) 1998-99 Objective Reality Computer. */


#include <math.h>
#include "tree.h"
#include "coord.h"
#include "nodetypes.h"
#include "reigon.h"

#define COS(x) cos((x)/180*M_PI)
#define SIN(x) sin((x)/180*M_PI)

void grow_node_bbox(treenode *parent, point *origin, point *dimension)
{
  treenode *thing;

  thing = parent->child;
  while(thing)
  {
    if(!thing->visible)
    {
      thing = thing->next;
      continue;
    }

    make_bbox(thing);

    if(thing->b_origin.x < origin->x)
    {
      dimension->x += (origin->x - thing->b_origin.x);
      origin->x = thing->b_origin.x;
    }
    if(thing->b_origin.y < origin->y)
    {
      dimension->y += (origin->y - thing->b_origin.y);
      origin->y = thing->b_origin.y;
    }
    if(thing->b_origin.z < origin->z)
    {
      dimension->z += (origin->z - thing->b_origin.z);
      origin->z = thing->b_origin.z;
    }

    if(thing->b_origin.x + thing->b_dimentions.x > dimension->x + origin->x)
    {
      dimension->x = thing->b_origin.x + thing->b_dimentions.x - origin->x;
    }
    if(thing->b_origin.y + thing->b_dimentions.y > dimension->y + origin->y)
    {
      dimension->y = thing->b_origin.y + thing->b_dimentions.y - origin->y;
    }
    if(thing->b_origin.z + thing->b_dimentions.z > dimension->z + origin->z)
    {
      dimension->z = thing->b_origin.z + thing->b_dimentions.z - origin->z;
    }

    thing = thing->next;
  }
}

void draw_bbox(treenode *thing)
{
  glColor4f(1.0, 1.0, 1.0, 1.0);

  glBegin(GL_LINE_LOOP);
  glColor4f(0.0, 0.0, 1.0, 1.0);
    glVertex3f(thing->bbox[0] - .01, thing->bbox[1] - .01, thing->bbox[2] - .01);       // 0
  glColor4f(1.0, 1.0, 1.0, 1.0);
    glVertex3f(thing->bbox[3] + .01, thing->bbox[4] - .01, thing->bbox[5] - .01);       // 3
    glVertex3f(thing->bbox[6] + .01, thing->bbox[7] + .01, thing->bbox[8] - .01);       // 6
    glVertex3f(thing->bbox[9] - .01, thing->bbox[10] + .01, thing->bbox[11] - .01);     // 9
  glEnd();
  glBegin(GL_LINE_LOOP);
    glVertex3f(thing->bbox[12] - .01, thing->bbox[13] - .01, thing->bbox[14] - .01);    // 12
    glVertex3f(thing->bbox[15] + .01, thing->bbox[16] - .01, thing->bbox[17] - .01);    // 15
  glColor4f(0.0, 0.0, 1.0, 1.0);
    glVertex3f(thing->bbox[18] + .01, thing->bbox[19] + .01, thing->bbox[20] - .01);    // 18
  glColor4f(1.0, 1.0, 1.0, 1.0);
    glVertex3f(thing->bbox[21] - .01, thing->bbox[22] + .01, thing->bbox[23] - .01);    // 21
  glEnd();
  glBegin(GL_LINES);
    glVertex3f(thing->bbox[0] - .01, thing->bbox[1] - .01, thing->bbox[2] - .01);
    glVertex3f(thing->bbox[12] - .01, thing->bbox[13] - .01, thing->bbox[14] - .01);
    glVertex3f(thing->bbox[3] + .01, thing->bbox[4] - .01, thing->bbox[5] - .01);
    glVertex3f(thing->bbox[15] + .01, thing->bbox[16] - .01, thing->bbox[17] - .01);
    glVertex3f(thing->bbox[6] + .01, thing->bbox[7] + .01, thing->bbox[8] - .01);
    glVertex3f(thing->bbox[18] + .01, thing->bbox[19] + .01, thing->bbox[20] - .01);
    glVertex3f(thing->bbox[9] - .01, thing->bbox[10] + .01, thing->bbox[11] - .01);
    glVertex3f(thing->bbox[21] - .01, thing->bbox[22] + .01, thing->bbox[23] - .01);
  glEnd();
}

void make_bbox(treenode *thing)
{
  point origin, dimension;
  int i;

  origin.x = origin.y = origin.z = 0.0;
  dimension = thing->dimentions;

  if(thing->child)
    grow_node_bbox(thing, &origin, &dimension);

  if(thing->type == REIGON)
  {
    reigon *props;

    props = (reigon *)(thing->prop);
    origin.x = (props->viewport.x > origin.x)?(props->viewport.x):(origin.x);
    origin.y = (props->viewport.y > origin.y)?(props->viewport.y):(origin.y);
    origin.z = (props->viewport.z > origin.z)?(props->viewport.z):(origin.z);

    dimension.x = (props->viewsize.x < dimension.x)?(props->viewsize.x):(dimension.x);
    dimension.y = (props->viewsize.y < dimension.y)?(props->viewsize.y):(dimension.y);
    dimension.z = (props->viewsize.z < dimension.z)?(props->viewsize.z):(dimension.z);
  }

//  thing->b_dimentions = dimension;
//  thing->b_origin = origin;
//  thing->b_origin.x += thing->origin.x;
//  thing->b_origin.y += thing->origin.y;
//  thing->b_origin.z += thing->origin.z;

  thing->bbox[0] = origin.x;
  thing->bbox[1] = origin.y;
  thing->bbox[2] = origin.z;
  rotate_point_backward(&(thing->bbox[0]), thing->rotation.x, thing->rotation.y, thing->rotation.z);
  thing->bbox[0] += thing->origin.x;
  thing->bbox[1] += thing->origin.y;
  thing->bbox[2] += thing->origin.z;

  thing->bbox[3] = origin.x + dimension.x;
  thing->bbox[4] = origin.y;
  thing->bbox[5] = origin.z;
  rotate_point_backward(&(thing->bbox[3]), thing->rotation.x, thing->rotation.y, thing->rotation.z);
  thing->bbox[3] += thing->origin.x;
  thing->bbox[4] += thing->origin.y;
  thing->bbox[5] += thing->origin.z;

  thing->bbox[6] = origin.x + dimension.x;
  thing->bbox[7] = origin.y + dimension.y;
  thing->bbox[8] = origin.z;
  rotate_point_backward(&(thing->bbox[6]), thing->rotation.x, thing->rotation.y, thing->rotation.z);
  thing->bbox[6] += thing->origin.x;
  thing->bbox[7] += thing->origin.y;
  thing->bbox[8] += thing->origin.z;

  thing->bbox[9] = origin.x;
  thing->bbox[10] = origin.y + dimension.y;
  thing->bbox[11] = origin.z;
  rotate_point_backward(&(thing->bbox[9]), thing->rotation.x, thing->rotation.y, thing->rotation.z);
  thing->bbox[9] += thing->origin.x;
  thing->bbox[10] += thing->origin.y;
  thing->bbox[11] += thing->origin.z;

  thing->bbox[12] = origin.x;
  thing->bbox[13] = origin.y;
  thing->bbox[14] = origin.z + dimension.z;
  rotate_point_backward(&(thing->bbox[12]), thing->rotation.x, thing->rotation.y, thing->rotation.z);
  thing->bbox[12] += thing->origin.x;
  thing->bbox[13] += thing->origin.y;
  thing->bbox[14] += thing->origin.z;

  thing->bbox[15] = origin.x + dimension.x;
  thing->bbox[16] = origin.y;
  thing->bbox[17] = origin.z + dimension.z;
  rotate_point_backward(&(thing->bbox[15]), thing->rotation.x, thing->rotation.y, thing->rotation.z);
  thing->bbox[15] += thing->origin.x;
  thing->bbox[16] += thing->origin.y;
  thing->bbox[17] += thing->origin.z;

  thing->bbox[18] = origin.x + dimension.x;
  thing->bbox[19] = origin.y + dimension.y;
  thing->bbox[20] = origin.z + dimension.z;
  rotate_point_backward(&(thing->bbox[18]), thing->rotation.x, thing->rotation.y, thing->rotation.z);
  thing->bbox[18] += thing->origin.x;
  thing->bbox[19] += thing->origin.y;
  thing->bbox[20] += thing->origin.z;

  thing->bbox[21] = origin.x;
  thing->bbox[22] = origin.y + dimension.y;
  thing->bbox[23] = origin.z + dimension.z;
  rotate_point_backward(&(thing->bbox[21]), thing->rotation.x, thing->rotation.y, thing->rotation.z);
  thing->bbox[21] += thing->origin.x;
  thing->bbox[22] += thing->origin.y;
  thing->bbox[23] += thing->origin.z;

  thing->b_origin.x = thing->bbox[0];
  thing->b_origin.y = thing->bbox[1];
  thing->b_origin.z = thing->bbox[2];
  thing->b_dimentions.x = thing->b_dimentions.y = thing->b_dimentions.z = 0.0;
  for(i = 0; i < 24; i += 3)
  {
    if(thing->bbox[i] < thing->b_origin.x)
      thing->b_origin.x = thing->bbox[i];
    if(thing->bbox[i] > (thing->b_origin.x + thing->b_dimentions.x))
      thing->b_dimentions.x = thing->bbox[i] - thing->b_origin.x;

    if(thing->bbox[i + 1] < thing->b_origin.y)
      thing->b_origin.y = thing->bbox[i + 1];
    if(thing->bbox[i + 1] > (thing->b_origin.y + thing->b_dimentions.y))
      thing->b_dimentions.y = thing->bbox[i + 1] - thing->b_origin.y;

    if(thing->bbox[i + 2] < thing->b_origin.z)
      thing->b_origin.z = thing->bbox[i + 2];
    if(thing->bbox[i + 2] > (thing->b_origin.z + thing->b_dimentions.z))
      thing->b_dimentions.z = thing->bbox[i + 2] - thing->b_origin.z;
  }
   if(thing->parent->type == REIGON)
      calc_control_bounds(thing->parent);
}

// "forward" and "backward" indicate the direction the rotations are
// applied.  forward is x, then y, then z;  backward is the opposite order.
void rotate_point_forward(double tri[3], double rotx, double roty, double rotz)
{
  double sna, csa;
  double newtri[3];

  // x
  sna = SIN(rotx);
  csa = COS(rotx);
  newtri[0] = tri[0];
  newtri[1] = (tri[1] * csa) - (tri[2] * sna); // -
  newtri[2] = (tri[1] * sna) + (tri[2] * csa); // +

  // y
  sna = SIN(roty);
  csa = COS(roty);
  tri[0] = (newtri[0] * csa) + (newtri[2] * sna); // +
  tri[1] = newtri[1];
  tri[2] = (newtri[2] * csa) - (newtri[0] * sna); // -

  // z
  sna = SIN(rotz);
  csa = COS(rotz);
  newtri[0] = (tri[0] * csa) - (tri[1] * sna); // -
  newtri[1] = (tri[0] * sna) + (tri[1] * csa); // +
  newtri[2] = tri[2];

  tri[0] = newtri[0];
  tri[1] = newtri[1];
  tri[2] = newtri[2];
}

void rotate_point_backward(double tri[3], double rotx, double roty, double rotz)
{
  double sna, csa;
  double newtri[3];

  // z
  sna = SIN(rotz);
  csa = COS(rotz);
  newtri[0] = (tri[0] * csa) - (tri[1] * sna); // -
  newtri[1] = (tri[0] * sna) + (tri[1] * csa); // +
  newtri[2] = tri[2];

  // y
  sna = SIN(roty);
  csa = COS(roty);
  tri[0] = (newtri[0] * csa) + (newtri[2] * sna); // +
  tri[1] = newtri[1];
  tri[2] = (newtri[2] * csa) - (newtri[0] * sna); // -

  // x
  sna = SIN(rotx);
  csa = COS(rotx);
  newtri[0] = tri[0];
  newtri[1] = (tri[1] * csa) - (tri[2] * sna); // -
  newtri[2] = (tri[1] * sna) + (tri[2] * csa); // +

  tri[0] = newtri[0];
  tri[1] = newtri[1];
  tri[2] = newtri[2];
}

void rotate_tri(double tri[9], double rotx, double roty, double rotz)
{
  rotate_point_backward(&tri[0], rotx, roty, rotz);
  rotate_point_backward(&tri[3], rotx, roty, rotz);
  rotate_point_backward(&tri[6], rotx, roty, rotz);
}

void childxy(treenode *child, point *pt)
{
  // in today's episode, child is the child you would like to translate
  // your coordinates into 

  double p[3];

  // the order of transformations goes like this:
  //   (parent) translate->rotx->roty->rotz (child)
  // so, in this function, we need to reverse this order (it's from the
  // child's point of view)

  if(!child)
    return;
  if(!pt)
    return;

  p[0] = pt->x;
  p[1] = pt->y;
  p[2] = pt->z;

  p[0] -= child->origin.x;
  p[1] -= child->origin.y;
  p[2] -= child->origin.z;

  rotate_point_backward(p, child->rotation.x, child->rotation.y, child->rotation.z);

  pt->x = p[0];
  pt->y = p[1];
  pt->z = p[2];
}

void parentxy(treenode *child, point *pt)
{
  double p[3];

  if(!child)
    return;
  if(!pt)
    return;

  p[0] = pt->x;
  p[1] = pt->y;
  p[2] = pt->z;

  rotate_point_forward(p, child->rotation.x, child->rotation.y, child->rotation.z);

  p[0] += child->origin.x;
  p[1] += child->origin.y;
  p[2] += child->origin.z;

  pt->x = p[0];
  pt->y = p[1];
  pt->z = p[2];
}
