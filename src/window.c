/* window.c - 
 * window routines for Synapse
 * Copyright(c) 1998-99 Objective Reality Computer.
 * By Nick McKinney and Max Harris. 
 */

#include <GL/glu.h> // for gluTesselate()
#include <stdio.h> // for printf()
#include <stdlib.h> // for malloc(), free()
#include "draw.h" // for border()
#include "button.h" // for make_button()
#include "window.h"
#include "wmanager.h" // for create_window derivitives
#include "tree.h"  // for treenode
#include "nodetypes.h"  // for all object types
#include "coord.h"
#include "font.h" // for font texmap playing with 
#include "bar.h"
#include "event.h" // for send_app_event

GLfloat app_color[4] = {0.0, 0.0, 0.0, 0.0};

int event_window(struct treenode *target, struct treenode *node, int event, int modifier, point *where, unsigned long usec)
{
  switch(event)
  {
    case EVENT_MOUSEDOWN:
       if(target->parent->activenode != target)
         target->parent->activenode = target;
       else
         return 1;
       break;

    case EVENT_MOUSEUP:
       break;

    case EVENT_COLLIDE:
       return 1;
       break;

    case EVENT_CLICK:
       switch(target->subtype)
       {
          case REGULAR:
          {
             if (node == ((regwinprop *)(target->prop))->closebox)
             {
                if(!(((regwinprop *)(target->prop))->destroying))
                {
                   ((regwinprop *)(target->prop))->destroying = 1;
                   send_node_event(target->parent, target, EVENT_DESTROY, 0, NULL);
                }
                return 0;
             }
             else if(node == ((regwinprop *)(target->prop))->zoombox)
             {
                return 0;
             }
             break;
          }

          case MINI:
          {
             if (node == ((miniwinprop *)(target->prop))->closebox)
             {
                if(!(((miniwinprop *)(target->prop))->destroying))
                {
                   ((miniwinprop *)(target->prop))->destroying = 1;
                   send_node_event(target->parent, target, EVENT_DESTROY, 0, NULL);
                }
                return 0;
             }
             else if(node == ((miniwinprop *)(target->prop))->zoombox)
             {
                return 0;
             }
             break;
          }
       }
       return 1;
       break;

    case EVENT_RESIZE:
       switch(target->subtype)
       {
          case REGULAR:
             resize_window_regular(target);
             break;

          case MINI:
             resize_window_mini(target);
             break;

          case DIALOG:
             resize_window_dialog(target);
             break;
       }
       return 1; // pass it on to the app
       break;

    default:
       break;
  }
  return 0;
}

int destroy_window(treenode *dis)
{
   switch(dis->subtype)
   {
      case REGULAR:
      case MINI:
      case DIALOG:
         free(dis->prop);
         dis->prop = 0;
         break;
   }
   return 0;
}

void minimize_window(treenode *dis)
{
}

treenode *create_window(treenode *parent, GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height,
       GLfloat rotx, GLfloat roty, GLfloat rotz, GLfloat *color, int type, int subtype, char *title, int visible)
{
  treenode *temp;
//static GLfloat ubercan = 0;
  static GLfloat wmanagerhack = -1;

  if (parent->type != ROOT && parent->type != APP)
     return 0;

  temp = create_child(parent, sizeof(treenode));
  if(!temp)
    return 0;

   temp->type = FLATWINDOW;
   temp->origin.x = x;
   temp->origin.y = y;
   temp->origin.z = z + wmanagerhack; //z;
   wmanagerhack += 0.5;
   if(wmanagerhack > 5)
     wmanagerhack = -11;
   temp->dimentions.x = width;
   temp->dimentions.y = height;
   temp->rotation.x = rotx;
   temp->rotation.y = roty;
   temp->rotation.z = rotz;
   temp->visible = visible;

   temp->color[0] = temp->parent->color[0];
   temp->color[1] = temp->parent->color[1];
   temp->color[2] = temp->parent->color[2];
   temp->color[3] = temp->parent->color[3];

   temp->focusable = TRUE;
   temp->subtype = subtype;

  switch(subtype)
  {
    case REGULAR:
      if (create_regular(temp, title))
      {
         destroy_node(temp);
         return 0;
      }
      break;

    case DIALOG:
      if (create_dialog(temp, title))
      {
         destroy_node(temp);
         return 0;
      }
      break;

    case MINI:
      if (create_mini(temp, title))
      {
         destroy_node(temp);
         return 0;
      }
      break;

    default:
      destroy_node(temp);
      return 0;
  }

  make_bbox(temp);

  return temp;
}

void twist_window(treenode *dis, point *pt, point *around)
{
   dis->rotation.x = pt->x;
   dis->rotation.y = pt->y;
   dis->rotation.z = pt->z;
   make_bbox(dis);
}

void move_window(treenode *dis, point *pt)
{
   dis->origin.x = pt->x;
   dis->origin.y = pt->y;
   dis->origin.z = pt->z;
   make_bbox(dis);
}

void resize_window(treenode *dis, point *pt)
{
   if(pt->x > 1.0)
     dis->dimentions.x = pt->x;
   else
     dis->dimentions.x = 1.0;

   if(pt->y > 1.0)
     dis->dimentions.y = pt->y;
   else
     dis->dimentions.y = 1.0;

   send_node_event(dis, dis, EVENT_RESIZE, 0, &(dis->dimentions));
   make_bbox(dis);
}

void draw_window(treenode *dis)
{

   if (dis->parent->activenode == (treenode *)dis)
      glColor3f(0.05, 0.0, 0.2);
   else
      glColor4fv(dis->color);

   switch(dis->subtype)
   {
      case DIALOG:
         draw_dialog(dis);
         break;
   }
}

void draw_flatwindow(treenode *dis)
{
   // draw two polys to fill in the window
   glBegin(GL_QUADS);
   glNormal3f(0, 0, 1);
   glTexCoord2f(0.0, 0.0);  glVertex3f(0, 0.25, BORDERSCALE);
   glTexCoord2f(0.0, 1.0);  glVertex3f(dis->dimentions.x - 0.25, 0.25, BORDERSCALE);
   glTexCoord2f(1.0, 1.0);  glVertex3f(dis->dimentions.x - 0.25, dis->dimentions.y, BORDERSCALE);
   glTexCoord2f(1.0, 0.0);  glVertex3f(0, dis->dimentions.y, BORDERSCALE);
   glEnd();
}

void darerror(GLenum error)
{
  printf("tesselation error: %s\n", gluErrorString(error));
}

void draw_dialog(treenode *dis)
{
   GLUtriangulatorObj *dtess;
   GLdouble *vertices[400];  // enough for 99 objects
   int vertex = 0;
   treenode *thing;
   point origin, dim;

//   glColor4fv(dis->color);
   glColor3fv(dis->color);

   glBegin(GL_QUADS);
   glNormal3f(0, 0, 1);
   glVertex3f(0, 0.0, 0.0);
   glVertex3f(0, dis->dimentions.y, 0.0);
   glVertex3f(dis->dimentions.x, dis->dimentions.y, 0.0);
   glVertex3f(dis->dimentions.x, 0.0, 0.0);
   glEnd();

   origin.x = origin.y = origin.z = 0.0;
   dim.x = dis->dimentions.x;
   dim.y = dis->dimentions.y - 0.25;
   dim.z = DBLBORDERSCALE;
   new_border(&origin, &dim, HALFBORDERSCALE, FALSE, 0.75);

// if(dis->dirty)  make display list;

   dtess = gluNewTess();
   gluTessCallback(dtess,GLU_BEGIN,glBegin);
   gluTessCallback(dtess,GLU_END,glEnd);
   gluTessCallback(dtess,GLU_VERTEX,glVertex3dv);  // glVertex3df
   gluTessCallback(dtess,GLU_ERROR,darerror);

   gluBeginPolygon(dtess);

   // draw the exterior contour
   gluNextContour(dtess, GLU_EXTERIOR);
   vertices[vertex] = (GLdouble *)malloc(sizeof(GLdouble) * 3);
   vertices[vertex][0] = HALFBORDERSCALE;
   vertices[vertex][1] = HALFBORDERSCALE;
   vertices[vertex][2] = DBLBORDERSCALE;
   gluTessVertex(dtess, vertices[vertex], (void *)vertices[vertex]);
   vertex++;

   vertices[vertex] = (GLdouble *)malloc(sizeof(GLdouble) * 3);
   vertices[vertex][0] = dis->dimentions.x - HALFBORDERSCALE;
   vertices[vertex][1] = HALFBORDERSCALE;
   vertices[vertex][2] = DBLBORDERSCALE;
   gluTessVertex(dtess, vertices[vertex], (void *)vertices[vertex]);
   vertex++;

   vertices[vertex] = (GLdouble *)malloc(sizeof(GLdouble) * 3);
   vertices[vertex][0] = dis->dimentions.x - HALFBORDERSCALE;
   vertices[vertex][1] = dis->dimentions.y - 0.25 - HALFBORDERSCALE;
   vertices[vertex][2] = DBLBORDERSCALE;
   gluTessVertex(dtess, vertices[vertex], (void *)vertices[vertex]);
   vertex++;

   vertices[vertex] = (GLdouble *)malloc(sizeof(GLdouble) * 3);
   vertices[vertex][0] = HALFBORDERSCALE;
   vertices[vertex][1] = dis->dimentions.y - 0.25 - HALFBORDERSCALE;
   vertices[vertex][2] = DBLBORDERSCALE;
   gluTessVertex(dtess, vertices[vertex], (void *)vertices[vertex]);
   vertex++;

   // draw each interior contour
   thing = dis->child;
   while(thing)
   {
      if(!(thing->type == BAR || (thing->type == TEXT && thing->subtype == BOX) || thing->type == GROUP || thing->type == LIST))
      {
         thing = thing->next;
         continue;
      }

      gluNextContour(dtess, GLU_INTERIOR);
      vertices[vertex] = (GLdouble *)malloc(sizeof(GLdouble) * 3);
      vertices[vertex][0] = thing->origin.x - HALFBORDERSCALE;
      vertices[vertex][1] = thing->origin.y - HALFBORDERSCALE;
      vertices[vertex][2] = DBLBORDERSCALE;
      gluTessVertex(dtess, vertices[vertex], (void *)vertices[vertex]);
      vertex++;

      vertices[vertex] = (GLdouble *)malloc(sizeof(GLdouble) * 3);
      vertices[vertex][0] = thing->origin.x - HALFBORDERSCALE;
      vertices[vertex][1] = thing->origin.y + thing->dimentions.y + HALFBORDERSCALE;
      vertices[vertex][2] = DBLBORDERSCALE;
      gluTessVertex(dtess, vertices[vertex], (void *)vertices[vertex]);
      vertex++;

      vertices[vertex] = (GLdouble *)malloc(sizeof(GLdouble) * 3);
      vertices[vertex][0] = thing->origin.x + thing->dimentions.x + HALFBORDERSCALE;
      vertices[vertex][1] = thing->origin.y + thing->dimentions.y + HALFBORDERSCALE;
      vertices[vertex][2] = DBLBORDERSCALE;
      gluTessVertex(dtess, vertices[vertex], (void *)vertices[vertex]);
      vertex++;

      vertices[vertex] = (GLdouble *)malloc(sizeof(GLdouble) * 3);
      vertices[vertex][0] = thing->origin.x + thing->dimentions.x + HALFBORDERSCALE;
      vertices[vertex][1] = thing->origin.y - HALFBORDERSCALE;
      vertices[vertex][2] = DBLBORDERSCALE;
      gluTessVertex(dtess, vertices[vertex], (void *)vertices[vertex]);
      vertex++;

      origin.x = thing->origin.x - HALFBORDERSCALE;
      origin.y = thing->origin.y - HALFBORDERSCALE;
      origin.z = 0.0;
      dim.x = thing->dimentions.x + BORDERSCALE;
      dim.y = thing->dimentions.y + BORDERSCALE;
      dim.z = DBLBORDERSCALE;

      if (thing->parent->activenode == thing)
         glColor3f(thing->parent->color[0] + 0.1, thing->parent->color[1] + 0.1, thing->parent->color[2] + 0.1);

      new_border(&origin, &dim, HALFBORDERSCALE, TRUE, 0.75);

      glColor4fv(dis->color);
//    glColor3f(0.3, 0.3, 0.3);// glColor4fv(thing->color);

      thing = thing->next;

      if(vertex > 395)
         thing = 0;
   }
   gluEndPolygon(dtess);
   gluDeleteTess(dtess);

   while(vertex > 0)
      free(vertices[--vertex]);
}

