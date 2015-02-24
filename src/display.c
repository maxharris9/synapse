/* display.c -
 * display routines for Synapse
 * Copyright(c) 1998-99 Objective Reality Computer.
 * By Nick McKinney and Max Harris. 
 */
#include <GL/glut.h> // for glutSwapBuffers()
#include <math.h>
#include "display.h"
#include <stdio.h>
#include "init.h" // for ScreenWidth and ScreeHeight
#include "input.h" // for drawpointer()
#include "tree.h" // for rootnode
#include "window.h" // remove me
#include "resources.h" // and me
#include "resource.h" // me too
extern GLuint resources[]; // me as well

static camera *currentcamera;

void display(void)
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glPushMatrix();
   drawtree(&rootnode);

   glPopMatrix();

   glutSwapBuffers();
   glFlush();
}

void reshape(int width, int height)
{
   static GLfloat position[4] = {-5.0, 5.0, 2.0, 0.0};  // -5 -5 2 0

   ScreenWidth = width;
   ScreenHeight = height;

   if (width > WIDTH)
      width = WIDTH;

   if (height > HEIGHT)
      height = HEIGHT;

   glViewport(0, 0, width, height);
   positionview(currentcamera);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   glLightfv(GL_LIGHT0, GL_POSITION, position);

   glEnable(GL_LIGHT0);
   glEnable(GL_LIGHTING);
}

camera * new_create_camera(treenode *parent, GLfloat x, GLfloat y, GLfloat z, GLfloat rx, GLfloat ry, GLfloat rz, GLfloat v)
{
  camera *temp;

  if(parent->type != ROOT)  // root node
    return 0;

  temp = (camera *)create_child(parent, sizeof(camera));
  if(!temp)
    return 0;

  temp->nodedef.type = CAMERA;
  temp->location.x = x;
  temp->location.y = y;
  temp->location.z = z;
  temp->rotation.x = rx;
  temp->rotation.y = ry;
  temp->rotation.z = rz;
  temp->velocity = v;
  temp->nodedef.visible = 1;  // so pointer may draw
  temp->nodedef.mask = 0xFFFFFFFF;

  return temp;
}

// returns old current camera
camera * set_current_camera(camera *curr)
{
  camera * temp = currentcamera;

  if(curr)
    if(curr->nodedef.type == CAMERA)
    {
      currentcamera = curr;
      return temp;
    }

  return 0;
}

camera * current_camera(void)
{
  return currentcamera;
}

void positionview(camera *cam)
{
   // stack manipulation?

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(FOV, (float)WIDTH/(float)HEIGHT, 1, 200.0); // 10 == 0.1
   glRotatef(-cam->rotation.x, 1.0, 0.0, 0.0);
   glRotatef(-cam->rotation.y, 0.0, 1.0, 0.0);
   glRotatef(-cam->rotation.z, 0.0, 0.0, 1.0);
   glTranslatef(-cam->location.x, -cam->location.y, -cam->location.z);
   glMatrixMode(GL_MODELVIEW);
}

#define COS(x) cos((x)/180*M_PI)
#define SIN(x) sin((x)/180*M_PI)

void movecamera(camera *cam, GLfloat amt)
{
   GLfloat x, y, z;

   x = -amt * (COS(cam->rotation.x)*SIN(cam->rotation.y)*COS(cam->rotation.z) - SIN(cam->rotation.x)*SIN(cam->rotation.z));
   y = -amt * (COS(cam->rotation.x)*SIN(cam->rotation.y)*SIN(cam->rotation.z) - SIN(cam->rotation.x)*COS(cam->rotation.z));
   z = -amt * (COS(cam->rotation.x)*COS(cam->rotation.y));
   cam->location.x += x;
   cam->location.y += y;
   cam->location.z += z;
   positionview(cam); // this can be a lot more optimal
}

void rotatecamera(camera *cam, GLfloat dx, GLfloat dy, GLfloat dz)
{
   cam->rotation.x += dx;
   cam->rotation.y += dy;
   cam->rotation.z += dz;
   positionview(cam); // this can be a lot more optimal
}
