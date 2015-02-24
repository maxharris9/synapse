/* draw.c -
 * drawing routines for Synapse
 * Copyright(c) 1998 Objective Reality Computer.
 * By Nick McKinney and Max Harris. 
 */

#include "synapse.h"
#include "draw.h"

void new_border(point *origin, point *dimensions, GLfloat thick, int inside, GLfloat sharpness)
{
   point p1, p2;
   GLfloat d1, d2, d3;
   GLfloat pol = 1;

   p1.x = origin->x;
   p1.y = origin->y;
   p1.z = origin->z;
   p2.x = origin->x + dimensions->x;
   p2.y = origin->y + dimensions->y;
   d1 = origin->z;
   d2 = origin->z + (dimensions->z * sharpness);
   d3 = origin->z + dimensions->z;

   if(dimensions->z < 0)
      pol = -1;

   if(inside)
   {
      // draw top triangle
      glBegin(GL_TRIANGLE_STRIP);
         glNormal3f(0, -1.0 * pol, 0);
         glVertex3f(p1.x + thick, p2.y - thick, d1);
         glVertex3f(p2.x - thick, p2.y - thick, d1);
         glVertex3f(p1.x + thick, p2.y - thick, d2);
         glVertex3f(p2.x - thick, p2.y - thick, d2);
         glNormal3f(0, 0, 1);
         glVertex3f(p1.x, p2.y, d3);
         glVertex3f(p2.x, p2.y, d3);
      glEnd();

      // draw left side
      glBegin(GL_TRIANGLE_STRIP);
         glNormal3f(1.0 * pol, 0, 0);
         glVertex3f(p1.x + thick, p1.y + thick, d1);
         glVertex3f(p1.x + thick, p2.y - thick, d1);
         glVertex3f(p1.x + thick, p1.y + thick, d2);
         glVertex3f(p1.x + thick, p2.y - thick, d2);
         glNormal3f(0, 0, 1);
         glVertex3f(p1.x, p1.y, d3);
         glVertex3f(p1.x, p2.y, d3);
      glEnd();

      // draw bottom
      glBegin(GL_TRIANGLE_STRIP);
         glNormal3f(0, 1.0 * pol, 0);
         glVertex3f(p2.x - thick, p1.y + thick, d1);
         glVertex3f(p1.x + thick, p1.y + thick, d1);
         glVertex3f(p2.x - thick, p1.y + thick, d2);
         glVertex3f(p1.x + thick, p1.y + thick, d2);
         glNormal3f(0, 0, 1);
         glVertex3f(p2.x, p1.y, d3);
         glVertex3f(p1.x, p1.y, d3);
      glEnd();

      // draw right
      glBegin(GL_TRIANGLE_STRIP);
         glNormal3f(-1.0 * pol, 0, 0);
         glVertex3f(p2.x - thick, p2.y - thick, d1);
         glVertex3f(p2.x - thick, p1.y + thick, d1);
         glVertex3f(p2.x - thick, p2.y - thick, d2);
         glVertex3f(p2.x - thick, p1.y + thick, d2);
         glNormal3f(0, 0, 1);
         glVertex3f(p2.x, p2.y, d3);
         glVertex3f(p2.x, p1.y, d3);
      glEnd();
   }
   else
   {
      // draw top triangle
      glBegin(GL_TRIANGLE_STRIP);
         glNormal3f(0, 1.0 * pol, 0);
         glVertex3f(p2.x, p2.y, d1);
         glVertex3f(p1.x, p2.y, d1);
         glVertex3f(p2.x, p2.y, d2);
         glVertex3f(p1.x, p2.y, d2);
         glNormal3f(0, 0, 1);
         glVertex3f(p2.x - thick, p2.y - thick, d3);
         glVertex3f(p1.x + thick, p2.y - thick, d3);
      glEnd();

      // draw left side
      glBegin(GL_TRIANGLE_STRIP);
         glNormal3f(-1.0 * pol, 0, 0);
         glVertex3f(p1.x, p2.y, d1);
         glVertex3f(p1.x, p1.y, d1);
         glVertex3f(p1.x, p2.y, d2);
         glVertex3f(p1.x, p1.y, d2);
         glNormal3f(0, 0, 1);
         glVertex3f(p1.x + thick, p2.y - thick, d3);
         glVertex3f(p1.x + thick, p1.y + thick, d3);
      glEnd();

      // draw bottom
      glBegin(GL_TRIANGLE_STRIP);
         glNormal3f(0, -1.0 * pol, 0);
         glVertex3f(p1.x, p1.y, d1);
         glVertex3f(p2.x, p1.y, d1);
         glVertex3f(p1.x, p1.y, d2);
         glVertex3f(p2.x, p1.y, d2);
         glNormal3f(0, 0, 1);
         glVertex3f(p1.x + thick, p1.y + thick, d3);
         glVertex3f(p2.x - thick, p1.y + thick, d3);
      glEnd();

      // draw right
      glBegin(GL_TRIANGLE_STRIP);
         glNormal3f(1.0 * pol, 0, 0);
         glVertex3f(p2.x, p1.y, d1);
         glVertex3f(p2.x, p2.y, d1);
         glVertex3f(p2.x, p1.y, d2);
         glVertex3f(p2.x, p2.y, d2);
         glNormal3f(0, 0, 1);
         glVertex3f(p2.x - thick, p1.y + thick, d3);
         glVertex3f(p2.x - thick, p2.y - thick, d3);
      glEnd();
   }
}

