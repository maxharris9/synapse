#include <GL/gl.h>
#include <GL/glut.h>
#include <stdlib.h> // for malloc
#include <stdio.h> // for printf
#include "tree.h" // for treenode
#include "coord.h" // for bounding boxes
#include "container.h" // for drawtree
#include "draw.h" // for new_border
#include "button.h" // for draw_push
#include "group.h"
#include "font.h" // for findfontbyname()
#include "text.h" // for create_text()

int resize_group(treenode *target, point *size)
{
/*reigon *rprops;

  rprops = (reigon *)(target->prop);
  rprops->viewsize = *size;
  rprops->viewsize.x -= 0.25;
  rprops->viewsize.y -= 0.25;
  calc_control_bounds(target);
*/
  return 0;
}

treenode *create_group(treenode *parent, GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height, GLfloat rotx, GLfloat roty, GLfloat rotz, char *label, int visible)
{
   treenode *temp;

   temp = create_child(parent, sizeof(treenode));
   if (!temp)
      return 0;

   temp->type = GROUP;
   temp->subtype = NONE;
   temp->origin.x = x;
   temp->origin.y = y;  //  - (viewrange_y - height);
   temp->origin.z = z;
   temp->dimentions.x = width;
   temp->dimentions.y = height;
   temp->dimentions.z = 0.1;
   temp->rotation.x = rotx;
   temp->rotation.y = roty;
   temp->rotation.z = rotz;
   temp->color[0] = parent->color[0];
   temp->color[1] = parent->color[1];
   temp->color[2] = parent->color[2];
   temp->color[3] = parent->color[3];
   temp->visible = visible;
   temp->autoresolve = 1;
   temp->event = group_event;
   temp->mask = MASK_ALL;
   temp->prop = malloc(sizeof(group));

   ((group *)(temp->prop))->label = create_text(temp, 0.25, temp->dimentions.y - 0.31, DBLBORDERSCALE + 0.0075, temp->dimentions.x, 0.0, 0, 0, 0, 0, 12, LABEL, find_font_by_name("20thcent.ttf"), label);
   make_bbox(temp);
  
   return temp;
}

int group_event(struct treenode *target, struct treenode *node, int event, int modifier, point *where, unsigned long usec)
{
  switch(event)
  {
    case EVENT_MOUSEDOWN:
       break;

    case EVENT_CHANGE:
      break;
  }
  return 0;
}

void draw_group_label_back(point dimentions)
{
   // draw top triangle
   glBegin(GL_TRIANGLE_STRIP);
      glVertex3f(dimentions.x - HALFBORDERSCALE, dimentions.y - HALFBORDERSCALE, DBLBORDERSCALE);
      glVertex3f(HALFBORDERSCALE, dimentions.y - HALFBORDERSCALE, DBLBORDERSCALE);
      glVertex3f(dimentions.x / 2.0, dimentions.y / 2.0, DBLBORDERSCALE);
   glEnd();

   // draw left side
   glBegin(GL_TRIANGLE_STRIP);
      glNormal3f(-1, 0, 0);
      glVertex3f(0, dimentions.y, 0);
      glVertex3f(0, 0, 0);
      glVertex3f(0, dimentions.y, DBLBORDERSCALE - HALFBORDERSCALE);
      glVertex3f(0, 0, DBLBORDERSCALE - HALFBORDERSCALE);
      glNormal3f(0, 0, 1);
      glVertex3f(HALFBORDERSCALE, dimentions.y - HALFBORDERSCALE, DBLBORDERSCALE);
      glVertex3f(HALFBORDERSCALE, HALFBORDERSCALE, DBLBORDERSCALE);
      glVertex3f(dimentions.x / 2.0, dimentions.y / 2.0, DBLBORDERSCALE);
   glEnd();
  
   // draw bottom
   glBegin(GL_TRIANGLE_STRIP);
      glNormal3f(0, 0, 1);
      glVertex3f(HALFBORDERSCALE, HALFBORDERSCALE, DBLBORDERSCALE);
      glVertex3f(dimentions.x - HALFBORDERSCALE, HALFBORDERSCALE, DBLBORDERSCALE);
      glVertex3f(dimentions.x / 2.0, dimentions.y / 2.0, DBLBORDERSCALE);
   glEnd();
  
   // draw right
   glBegin(GL_TRIANGLE_STRIP);
      glNormal3f(0, 1, 0);
      glVertex3f(dimentions.x, 0, 0);
      glVertex3f(dimentions.x, dimentions.y, 0);
      glVertex3f(dimentions.x, 0, DBLBORDERSCALE - HALFBORDERSCALE);
      glVertex3f(dimentions.x, dimentions.y, DBLBORDERSCALE - HALFBORDERSCALE);
      glNormal3f(0, 0, 1);
      glVertex3f(dimentions.x - HALFBORDERSCALE, HALFBORDERSCALE, DBLBORDERSCALE);
      glVertex3f(dimentions.x - HALFBORDERSCALE, dimentions.y - HALFBORDERSCALE, DBLBORDERSCALE);
      glVertex3f(dimentions.x / 2.0, dimentions.y / 2.0, DBLBORDERSCALE);
   glEnd();
}

void draw_group(treenode *this)
{
   point dim;

   dim.y = 0.25; dim.x = get_text_width(((group *)(this->prop))->label) + 0.2; dim.z = 0;

   glPushMatrix();
      glTranslatef(0.15, this->dimentions.y - 0.10, 0);
      draw_group_label_back(dim);
   glPopMatrix();

   draw_push(this);

   drawtree(this);
}
