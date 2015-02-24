#include <GL/gl.h>
#include <GL/glut.h>
#include <stdlib.h> // for malloc
#include <stdio.h> // for printf
#include "tree.h" // for treenode
#include "coord.h" // for bounding boxes
#include "container.h" // for drawtree
#include "bar.h" // for create_bar
#include "draw.h" // for new_border
#include "barmanager.h"
#include "reigon.h"

int reigon_event(struct treenode *target, struct treenode *node, int event, int modifier, point *where, unsigned long usec);

int destroy_reigon(treenode *dis)
{
  free(dis->prop);
  return 0;
}

int move_reigon(treenode *target, point *where)
{
  reigon *rprops;

  rprops = (reigon *)(target->prop);
  rprops->viewport = *where;
  rprops->viewport.y += 0.25;

  make_bbox(target);

  return 0;
}

int resize_reigon(treenode *target, point *size)
{
  reigon *rprops;

  rprops = (reigon *)(target->prop);
  rprops->viewsize = *size;
  rprops->viewsize.x -= 0.25;
  rprops->viewsize.y -= 0.25;

  resize_scrollbar(rprops->vbar, target->origin.x + rprops->viewport.x + rprops->viewsize.x, target->origin.y + rprops->viewport.y - 0.25, 0,
                                 0.25, rprops->viewsize.y + 0.25,
                                 0, 0, 0);
  resize_scrollbar(rprops->hbar, target->origin.x + rprops->viewport.x, target->origin.y + rprops->viewport.y - 0.25, 0,
                                 rprops->viewsize.x, 0.25,
                                 0, 0, 0);
  calc_control_bounds(target);

  return 0;
}

int calc_control_bounds(treenode *thing)
{
  treenode *dar;
  point min_p, max_p;
  int i;
  reigon *rprops;

  rprops = (reigon *)(thing->prop);
  dar = thing->child;

  if(!dar)
  {
    rprops->c_origin.x = rprops->c_dim.x = 0.0;
    rprops->c_origin.y = rprops->c_dim.y = 0.0;
    rprops->c_origin.z = rprops->c_dim.z = 0.0;
    return 0;
  }

  min_p.x = max_p.x = dar->bbox[0];
  min_p.y = max_p.y = dar->bbox[1];
  min_p.z = max_p.z = dar->bbox[2];

  while(dar)
  {
    for(i = 3; i < 24; i += 3)
    {
      min_p.x = (dar->bbox[i + 0] < min_p.x) ? (dar->bbox[i + 0]) : (min_p.x);
      min_p.y = (dar->bbox[i + 1] < min_p.y) ? (dar->bbox[i + 1]) : (min_p.y);
      min_p.z = (dar->bbox[i + 2] < min_p.z) ? (dar->bbox[i + 2]) : (min_p.z);
      max_p.x = (dar->bbox[i + 0] > max_p.x) ? (dar->bbox[i + 0]) : (max_p.x);
      max_p.y = (dar->bbox[i + 1] > max_p.y) ? (dar->bbox[i + 1]) : (max_p.y);
      max_p.z = (dar->bbox[i + 2] > max_p.z) ? (dar->bbox[i + 2]) : (max_p.z);
    }

    dar = dar->next;
  }

  rprops->c_origin = min_p;
  rprops->c_dim.x = max_p.x - min_p.x;
  rprops->c_dim.y = max_p.y - min_p.y;
  rprops->c_dim.z = max_p.z - min_p.z;

  union_reigon_size(thing);

  return 0;
}

int union_reigon_size(treenode *thing)
{
  reigon *rprops;
  scrollbarprop *sprops;
  bar *slprops;
  point min_p, max_p;

  rprops = (reigon *)(thing->prop);

  min_p.x = (rprops->viewport.x < rprops->c_origin.x) ? (rprops->viewport.x) : (rprops->c_origin.x);
  min_p.y = (rprops->viewport.y < rprops->c_origin.y) ? (rprops->viewport.y) : (rprops->c_origin.y);
  min_p.z = (rprops->viewport.z < rprops->c_origin.z) ? (rprops->viewport.z) : (rprops->c_origin.z);
  max_p.x = (rprops->viewport.x + rprops->viewsize.x > rprops->c_origin.x + rprops->c_dim.x) ? (rprops->viewport.x + rprops->viewsize.x) : (rprops->c_origin.x + rprops->c_dim.x);
  max_p.y = (rprops->viewport.y + rprops->viewsize.y > rprops->c_origin.y + rprops->c_dim.y) ? (rprops->viewport.y + rprops->viewsize.y) : (rprops->c_origin.y + rprops->c_dim.y);
  max_p.z = (rprops->viewport.z + rprops->viewsize.z > rprops->c_origin.x + rprops->c_dim.z) ? (rprops->viewport.z + rprops->viewsize.z) : (rprops->c_origin.x + rprops->c_dim.z);

//  thing->origin = min_p;
  thing->origin.x = -rprops->viewport.x;
  thing->origin.y = -(rprops->viewport.y - 0.25);
  thing->origin.z = -rprops->viewport.z;
  thing->dimentions.x = max_p.x - min_p.x;
  thing->dimentions.y = max_p.y - min_p.y;
  thing->dimentions.z = max_p.z - min_p.z;

  sprops = (scrollbarprop *)(rprops->vbar->prop);
  slprops = (bar *)(sprops->slider->prop);
  if(min_p.y != max_p.y)
    change_slider_values(sprops->slider, (int)(min_p.y * 100), (int)(max_p.y * 100), (int)(rprops->viewsize.y * 100),
         (int) ((max_p.y - (rprops->viewport.y + rprops->viewsize.y) + min_p.y) * 100)        );

  sprops = (scrollbarprop *)(rprops->hbar->prop);
  slprops = (bar *)(sprops->slider->prop);
  if(min_p.x != max_p.x)
    change_slider_values(sprops->slider, (int)(min_p.x * 100), (int)(max_p.x * 100), (int)(rprops->viewsize.x * 100), (int)(rprops->viewport.x * 100));

  return 0;
}

int change_reigon(struct treenode *target, struct treenode *node, int event, int modifier, point *where, unsigned long usec)
{
  scrollbarprop *props;
  reigon *rprops;
  treenode *reg;
  treenode *rbar;
  GLfloat diff;

  reg = target->parent->child;
  while(reg)
  {
    if(reg->type == REIGON)
      if(((reigon *)(reg->prop))->hbar == target)
      {
         rprops = (reigon *)(reg->prop);
         rbar = rprops->hbar;
         props = (scrollbarprop *)(rbar->prop);
         diff = rprops->viewport.x;
         rprops->viewport.x = modifier / 100.0;
         diff -= rprops->viewport.x;
         reg->origin.x += diff;
         break;
      }
      if(((reigon *)(reg->prop))->vbar == target)
      {
         rprops = (reigon *)(reg->prop);
         rbar = rprops->vbar;
         props = (scrollbarprop *)(rbar->prop);
         diff = rprops->viewport.y;
         rprops->viewport.y = ( ((bar *)(props->slider->prop))->max - modifier + ((bar *)(props->slider->prop))->min)  / 100.0 - rprops->viewsize.y;
         diff -= rprops->viewport.y;
         reg->origin.y += diff;
         break;
      }
    reg = reg->next;
  }

  union_reigon_size(reg);
  return 0;
}

treenode *create_reigon(treenode *parent, GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height, GLfloat rotx, GLfloat roty, GLfloat rotz, int visible)
{
   treenode *temp;

   temp = create_child(parent, sizeof(treenode));
   if (!temp)
      return 0;

   temp->type = REIGON;
   temp->subtype = NONE;
   temp->origin.x = x;
   temp->origin.y = y;  //  - (viewrange_y - height);
   temp->origin.z = z;
   temp->dimentions.x = temp->dimentions.y = temp->dimentions.z = 0.0;
   temp->rotation.x = rotx;
   temp->rotation.y = roty;
   temp->rotation.z = rotz;
   temp->color[0] = parent->parent->color[0];
   temp->color[1] = parent->parent->color[1];
   temp->color[2] = parent->parent->color[2];
   temp->color[3] = parent->parent->color[3];
   temp->visible = visible;
   temp->autoresolve = 1;
   temp->event = reigon_event;
   temp->mask = MASK_ALL & ~(MASK_CHANGE | MASK_MOUSEDOWN);

   width -= 0.25;  // HACK
   height -= 0.25;  // HACK

   temp->prop = malloc(sizeof(reigon));
   ((reigon *)(temp->prop))->viewport.x = 0.0;
   ((reigon *)(temp->prop))->viewport.y = 0.25;
   ((reigon *)(temp->prop))->viewport.z = 0.0;
   ((reigon *)(temp->prop))->viewsize.x = width;
   ((reigon *)(temp->prop))->viewsize.y = height;
   ((reigon *)(temp->prop))->viewsize.z = 0.0;

   ((reigon *)(temp->prop))->hbar = create_bar(parent, x, y, z, width, 0.25, 0, 0, 0, 0, 1, 0, 1, parent->color, BAR, SCROLL, 1);
   ((scrollbarprop *)((((reigon *)(temp->prop))->hbar)->prop))->change_handler = change_reigon;
   ((reigon *)(temp->prop))->vbar = create_bar(parent, x + width, y, z, 0.25, height + 0.25, 0, 0, 0, 0, 1, 0, 1, parent->color, BAR, VSCROLL, 1);
   ((scrollbarprop *)((((reigon *)(temp->prop))->vbar)->prop))->change_handler = change_reigon;
 
   make_bbox(temp->parent);
   
   return temp;
}

int reigon_event(struct treenode *target, struct treenode *node, int event, int modifier, point *where, unsigned long usec)
{
  switch(event)
  {
    case EVENT_MOUSEDOWN:
         return 1;
       break;

    case EVENT_MOUSEUP:
       break;

    case EVENT_COLLIDE:
       return 1;

/*  case EVENT_CLICK:
       return 1;
*/

    case EVENT_CHANGE:
      break;

    case EVENT_CLICK:
      return 1;
      break;
  
//  default:
//    return 1;
  }
  return 0;
}

void draw_reigon_tray(GLfloat ox, GLfloat oy, GLfloat dx, GLfloat dy)
{
  point origin;
  point dim;

  origin.x = ox; origin.y = oy; origin.z = BORDERSCALE;
  dim.x = dx; dim.y = dy; dim.z = -BORDERSCALE;

  new_border(&origin, &dim, HALFBORDERSCALE, FALSE, 0.05); // 0.25

  glPushMatrix();
  glTranslatef(ox, oy, 0);
  // draw top triangle
  glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    glVertex3f(HALFBORDERSCALE, HALFBORDERSCALE, 0.0);
    glVertex3f(dim.x - HALFBORDERSCALE, HALFBORDERSCALE, 0.0);
    glVertex3f(dim.x - HALFBORDERSCALE, dim.y - HALFBORDERSCALE, 0.0);
    glVertex3f(HALFBORDERSCALE, dim.y - HALFBORDERSCALE, 0.0);
  glEnd();
  glPopMatrix();
}

void draw_reigon(treenode *this)
{
   reigon *props;

   GLdouble left[4] = {1.0, 0.0, 0.0, 0.0};
   GLdouble down[4] = {0.0, 1.0, 0.0, 0.0};
   GLdouble right[4] = {-1.0, 0.0, 0.0, 0.0};
   GLdouble up[4] = {0.0, -1.0, 0.0, 0.0};

   props = (reigon *)(this->prop);

   glColor4fv(this->color);
   draw_reigon_tray(props->viewport.x, props->viewport.y, props->viewsize.x, props->viewsize.y);

   glPushMatrix();

   glTranslatef(props->viewport.x, props->viewport.y, props->viewport.z);

   glClipPlane(GL_CLIP_PLANE0, left);
   glEnable(GL_CLIP_PLANE0);
   glClipPlane(GL_CLIP_PLANE1, down);
   glEnable(GL_CLIP_PLANE1);

   glTranslatef(props->viewsize.x, props->viewsize.y, props->viewsize.z);

   glClipPlane(GL_CLIP_PLANE2, right);
   glEnable(GL_CLIP_PLANE2);
   glClipPlane(GL_CLIP_PLANE3, up);
   glEnable(GL_CLIP_PLANE3);

   glPopMatrix();

   drawtree(this);

   glDisable(GL_CLIP_PLANE0);
   glDisable(GL_CLIP_PLANE1);
   glDisable(GL_CLIP_PLANE2);
   glDisable(GL_CLIP_PLANE3);


// #define __DEBUG__REIGONS
#ifdef __DEBUG__REIGONS
   glBegin(GL_LINE_LOOP);
   glColor3f(1, 0, 0);
   glVertex3f(props->viewport.x, props->viewport.y, props->viewport.z);
   glVertex3f(props->viewport.x + props->viewsize.x, props->viewport.y, props->viewport.z);
   glVertex3f(props->viewport.x + props->viewsize.x, props->viewport.y + props->viewsize.y, props->viewport.z);
   glVertex3f(props->viewport.x, props->viewport.y + props->viewsize.y, props->viewport.z);
   glEnd();

   glBegin(GL_LINE_LOOP);
   glColor3f(0, 1, 0);
   glVertex3f(0, 0, 0);
   glVertex3f(this->dimentions.x, 0, 0);
   glVertex3f(this->dimentions.x, this->dimentions.y, 0);
   glVertex3f(0, this->dimentions.y, 0);
   glEnd();

   glBegin(GL_LINE_LOOP);
   glColor3f(0, 0, 1);
   glVertex3f(props->c_origin.x, props->c_origin.y, props->c_origin.z);
   glVertex3f(props->c_origin.x + props->c_dim.x, props->c_origin.y, props->c_origin.z);
   glVertex3f(props->c_origin.x + props->c_dim.x, props->c_origin.y + props->c_dim.y, props->c_origin.z);
   glVertex3f(props->c_origin.x, props->c_origin.y + props->c_dim.y, props->c_origin.z);
   glEnd();
#endif
}
