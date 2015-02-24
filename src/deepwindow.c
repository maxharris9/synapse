/* deepwindow.c - 
 * deepwindow routines for Synapse
 * Copyright(c) 1998-99 Objective Reality Computer.
 * By Nick McKinney and Max Harris. 
 */

#include <GL/glut.h> // for gluTesselate()
#include <stdio.h> // for printf()
#include <stdlib.h> // for malloc(), free()
#include "deepwindow.h"
#include "tree.h"  // for treenode
#include "nodetypes.h"  // for all object types
#include "coord.h"
#include "resources.h"

extern GLint resources[64];

int event_deepwindow(treenode *object, int event, int dsize, void *data)
{
  return 0;
}

void destroy_deepwindow(treenode *dis)
{
}

treenode *create_deepwindow(treenode *parent, GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height,
    GLfloat depth, GLfloat rotx, GLfloat roty, GLfloat rotz, GLfloat *color, int type, int subtype, char *title)
{
  treenode *temp;

  if (parent->type != ROOT && parent->type != APP)
     return 0;

  temp = create_child(parent, sizeof(treenode));
  if(!temp)
    return 0;

  temp->type = DEEPWINDOW;
  temp->origin.x = x;
  temp->origin.y = y;
  temp->origin.z = z;
  temp->dimentions.x = width;
  temp->dimentions.y = height;
  temp->dimentions.z = depth;
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

  make_bbox(temp);

  return temp;
}

#define HSQ 0.088388348

void draw_deepwindow(treenode *dis)
{
   glColor4f(1, 1, 0, 1);
   glPushMatrix();
     glTranslatef(-HSQ, -HSQ, HSQ);
     glCallList(resources[RES_DEEPGRIP]);
   glPopMatrix();

   glPushMatrix();
     glTranslatef(dis->dimentions.x + HSQ, -HSQ, HSQ);
     glRotatef(90, 0, 0, 1);
     glCallList(resources[RES_DEEPGRIP]);
   glPopMatrix();

   glPushMatrix();
     glTranslatef(dis->dimentions.x + HSQ, dis->dimentions.y + HSQ, HSQ);
     glRotatef(180, 0, 0, 1);
     glCallList(resources[RES_DEEPGRIP]);
   glPopMatrix();

   glPushMatrix();
     glTranslatef(-HSQ, dis->dimentions.y + HSQ, HSQ);
     glRotatef(-90, 0, 0, 1);
     glCallList(resources[RES_DEEPGRIP]);
   glPopMatrix();

   glPushMatrix();
     glTranslatef(-HSQ, -HSQ, -(dis->dimentions.z + HSQ));  // the z is WRONG
     glRotatef(-90, 0, 1, 0);
     glCallList(resources[RES_DEEPGRIP]);
   glPopMatrix();

   glPushMatrix();
     glTranslatef(dis->dimentions.x + HSQ, -HSQ, -(dis->dimentions.z + HSQ));
     glRotatef(90, 0, 0, 1);
     glRotatef(-90, 0, 1, 0);
     glCallList(resources[RES_DEEPGRIP]);
   glPopMatrix();

   glPushMatrix();
     glTranslatef(dis->dimentions.x + HSQ, dis->dimentions.y + HSQ, -(dis->dimentions.z + HSQ));
     glRotatef(180, 0, 0, 1);
     glRotatef(-90, 0, 1, 0);
     glCallList(resources[RES_DEEPGRIP]);
   glPopMatrix();

   glPushMatrix();
     glTranslatef(-HSQ, dis->dimentions.y + HSQ, -(dis->dimentions.z + HSQ));
     glRotatef(-90, 0, 0, 1);
     glRotatef(-90, 0, 1, 0);
     glCallList(resources[RES_DEEPGRIP]);
   glPopMatrix();

   glColor4f(1, 1, 1, 1);
   glLineWidth(2.0);
   glBegin(GL_LINE_LOOP);
      glVertex3f(0, 0, 0);
      glVertex3f(dis->dimentions.x, 0, 0);
      glVertex3f(dis->dimentions.x, dis->dimentions.y, 0);
      glVertex3f(0, dis->dimentions.y, 0);
   glEnd();
   glBegin(GL_LINE_LOOP);
      glVertex3f(0, 0, -dis->dimentions.z);
      glVertex3f(dis->dimentions.x, 0, -dis->dimentions.z);
      glVertex3f(dis->dimentions.x, dis->dimentions.y, -dis->dimentions.z);
      glVertex3f(0, dis->dimentions.y, -dis->dimentions.z);
   glEnd();

   glBegin(GL_LINES);
      glVertex3f(0, 0, 0);
      glVertex3f(0, 0, -dis->dimentions.z);
      glVertex3f(dis->dimentions.x, 0, 0);
      glVertex3f(dis->dimentions.x, 0, -dis->dimentions.z);
      glVertex3f(dis->dimentions.x, dis->dimentions.y, 0);
      glVertex3f(dis->dimentions.x, dis->dimentions.y, -dis->dimentions.z);
      glVertex3f(0, dis->dimentions.y, 0);
      glVertex3f(0, dis->dimentions.y, -dis->dimentions.z);
   glEnd();
}

