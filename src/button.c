/* button.c -  
 * button-handling routines for Synapse
 * Copyright(c) 1998-99 Objective Reality Computer.
 * By Nick McKinney and Max Harris. 
 */

#include <stdarg.h> // for vsprintf()
#include <stdlib.h> // for malloc() in make_button()
#include <stdio.h> // for NULL in make_button()
// #include "drag.h" // for startslide(), stopslide(), and others
#include "draw.h" // for border()
#include "font.h" // for findfontbyname()
#include "menu.h" // for show_menu(), hide_menu()
#include "window.h" // for set_active_window(), move_window(), and others
#include "button.h" // duh()
#include "text.h" // for create_text()
#include "window.h" // for control cycling
#include "nodetypes.h" // for CAMERA
#include "event.h" // for EVENT_*
#include "resources.h" // for corner pieces
#include "resource.h" // for corner pieces
#include "coord.h"

extern GLuint resources[64];

int get_button_status(treenode *thing)
{
  if(!thing)
    return -1;

  if(thing->type != BUTTON)
    return -1;

  return ((button *)(thing->prop))->state;
}

int set_button_status(treenode *thing, int status)
{
  if(!thing)
    return -1;
  
  if(thing->type != BUTTON)
    return -1;

  ((button *)(thing->prop))->state = status;
  return 0;
}

int reshape_button(treenode *thing, int which, GLfloat x, GLfloat y, GLfloat z,
                   GLfloat width, GLfloat height,
                   GLfloat rotx, GLfloat roty, GLfloat rotz)
{
  if(which & 1)
  {
    thing->origin.x = x;
    thing->origin.y = y;
    thing->origin.z = z;
  }

  if(which & 2)
  {
    treenode *label;

    thing->dimentions.x = width;
    thing->dimentions.y = height;
    label = thing->child;
    if(label)
      if(label->type == TEXT && label->subtype == LABEL)
      {
        label->dimentions.x = get_text_width(label);
        label->dimentions.y = get_text_height(label);
        label->origin.x = (thing->dimentions.x - label->dimentions.x) / 2.0;
        label->origin.y = (thing->dimentions.y - label->dimentions.y) / 2.0;
        make_bbox(label);
      }
  }

  if(which & 4)
  {
    thing->rotation.x = rotx;
    thing->rotation.y = roty;
    thing->rotation.z = rotz;
  }

  make_bbox(thing);
  return 0;
}

int change_button_label(treenode *dis, char *label)
{
   treenode *thing;

  if(!dis->child)
    thing = create_text(dis, 0, 0, DBLBORDERSCALE + 0.0075, dis->dimentions.x, 0.0 /* 0.1 */, 0, 0, 0, 0, 12, LABEL, find_font_by_name("20thcent.ttf"), label);
  else
  {
    thing = dis->child;
    change_text_label(thing, label);
  }

  thing->dimentions.x = get_text_width(thing);
  thing->dimentions.y = get_text_height(thing);
  thing->origin.x = (thing->parent->dimentions.x - thing->dimentions.x) / 2.0;
  thing->origin.y = (thing->parent->dimentions.y - thing->dimentions.y) / 2.0;
  make_bbox(thing);

  return 0;
}

//int event_button(treenode *object, int event, int dsize, void *data)
int event_button(struct treenode *target, struct treenode *node, int event, int modifier, point *where, unsigned long usec)
{
   static point whereat;
   static int over = 0;

   if (target->subtype == PUSH ||
       target->subtype == RADIO ||
       target->subtype == BORDER ||
       target->subtype ==  CORNER)
   {
      switch(event)
      {
        case EVENT_MOUSEDOWN:
           target->color[2] = 1 - target->color[2];
           target->origin.z -= HALFBORDERSCALE;
           make_bbox(target);
           whereat = *where;
           over = 1;
           break;

        case EVENT_MOUSEUP:
           if(!over)
             break;
           target->color[2] = 1 - target->color[2];
           target->origin.z += HALFBORDERSCALE;
           make_bbox(target);
           send_node_event(target->parent, target, EVENT_CLICK, 0, where);
           break;

        case EVENT_MOUSEDRAG:
           whereat.x += where->x;
           whereat.y += where->y;
           if(over)
           {
             if((whereat.x < 0.0) || (whereat.y < 0.0) || (whereat.x > node->dimentions.x) || (whereat.y > node->dimentions.y))
             {
               target->color[2] = 1 - target->color[2];
               target->origin.z += HALFBORDERSCALE;
               make_bbox(target);
               over = 0;
             }
           }
           else
           {
             if(!((whereat.x < 0.0) || (whereat.y < 0.0) || (whereat.x > node->dimentions.x) || (whereat.y > node->dimentions.y)))
             {
               target->color[2] = 1 - target->color[2];
               target->origin.z -= HALFBORDERSCALE;
               make_bbox(target);
               over = 1;
             }
           }
           break;

        case EVENT_COLLIDE:
           if(target->subtype == CORNER)
              return collide_corner(target, where);
           else
              return 1;

        case EVENT_KEYPRESS:
           if(modifier == '\r')
             send_node_event(target->parent, target, EVENT_CLICK, 0, where);
           else
             return 1;
           break;

        default:
           return 1;
     }
   }
   else
   {
     switch(event)
     {
        case EVENT_MOUSEDOWN:
           // this should really highlight the button until release, then
           // switch the state
           switch(((button *) (target->prop))->state)
           {
              case 0:
                 ((button *) (target->prop))->state = 1;
                 break;

              default:
                 ((button *) (target->prop))->state = 0;
                 break;
           }
           break;

        case EVENT_MOUSEUP:
           break;

        case EVENT_KEYPRESS:
           switch(((button *) (target->prop))->state)
           {
              case 0:
                 ((button *) (target->prop))->state = 1;
                 break;

              default:
                 ((button *) (target->prop))->state = 0;
                 break;
           }
           break;

        case EVENT_COLLIDE:
           return 1;

        default:
           return 1;
      }
   }

   return 0;
}

void draw_button(treenode *dis)
{
   if (dis->visible == VISIBLE && dis->type == BUTTON)
   {
      switch(dis->subtype)
      {
         case CHECK:
            glPushMatrix();
            glTranslatef(0.0, 0.03, 0.0);
         if (((button *) (dis->prop))->state == 0)
            draw_check();

           glColor4fv(dis->color); 
           draw_push(dis);
           glPopMatrix();
            break;

         case PUSH:
            draw_push(dis);
            break;

         case MENUBUTTON:
            draw_menubutton(dis);
            break;

         case CORNER:
            draw_corner(dis);
            break;

         case BORDER:
            draw_border(dis);
            break;

         case FILLER:
            draw_filler(dis);
            break;

         case RADIO:
            draw_radio();
            break; 

         default:
            printf("unknown entity type %d\n", dis->type);
            break;
       } 
   }
}

void draw_check(void)
{
   glPushMatrix();
   glTranslatef(0.0, 0.0, 0.2);
   glColor4f(0, 0, 0, 0);

   glCallList(resources[RES_CHECK]);

   glColor3f(0.3, 0.3, 0.3);
   glEnd();
   glPopMatrix();
}

void draw_radio(void)
{
   glPushMatrix();
   glTranslatef(0.0, 0.0, 0.2);
// glColor4f(0.3, 0.6, 0, 0);

   glCallList(resources[RES_RADIO]);

// glColor3f(0.3, 0.3, 0.3);
// glEnd();
   glPopMatrix();
}

void draw_push(treenode *dis)
{
// if(dis->parent->activenode == dis)
//    glColor4f(dis->color[0], dis->color[1], 1 - dis->color[2], dis->color[3]);

   // draw top triangle
   glBegin(GL_TRIANGLE_STRIP);
      glNormal3f(0, 1, 0);
      glVertex3f(dis->dimentions.x, dis->dimentions.y, 0);
      glVertex3f(0, dis->dimentions.y, 0);
      glVertex3f(dis->dimentions.x, dis->dimentions.y, DBLBORDERSCALE - HALFBORDERSCALE);
      glVertex3f(0, dis->dimentions.y, DBLBORDERSCALE - HALFBORDERSCALE);
      glNormal3f(0, 0, 1);
      glVertex3f(dis->dimentions.x - HALFBORDERSCALE, dis->dimentions.y - HALFBORDERSCALE, DBLBORDERSCALE);
      glVertex3f(HALFBORDERSCALE, dis->dimentions.y - HALFBORDERSCALE, DBLBORDERSCALE);
      glVertex3f(dis->dimentions.x / 2.0, dis->dimentions.y / 2.0, DBLBORDERSCALE);
   glEnd();

      // draw left side
      glBegin(GL_TRIANGLE_STRIP);
         glNormal3f(-1, 0, 0);
         glVertex3f(0, dis->dimentions.y, 0);
         glVertex3f(0, 0, 0);
         glVertex3f(0, dis->dimentions.y, DBLBORDERSCALE - HALFBORDERSCALE);
         glVertex3f(0, 0, DBLBORDERSCALE - HALFBORDERSCALE);
         glNormal3f(0, 0, 1);
         glVertex3f(HALFBORDERSCALE, dis->dimentions.y - HALFBORDERSCALE, DBLBORDERSCALE);
         glVertex3f(HALFBORDERSCALE, HALFBORDERSCALE, DBLBORDERSCALE);
         glVertex3f(dis->dimentions.x / 2.0, dis->dimentions.y / 2.0, DBLBORDERSCALE);
      glEnd();

      // draw bottom
      glBegin(GL_TRIANGLE_STRIP);
         glNormal3f(0, -1, 0);
         glVertex3f(0, 0, 0);
         glVertex3f(dis->dimentions.x, 0, 0);
         glVertex3f(0, 0, DBLBORDERSCALE - HALFBORDERSCALE);
         glVertex3f(dis->dimentions.x, 0, DBLBORDERSCALE - HALFBORDERSCALE);
         glNormal3f(0, 0, 1);
         glVertex3f(HALFBORDERSCALE, HALFBORDERSCALE, DBLBORDERSCALE);
         glVertex3f(dis->dimentions.x - HALFBORDERSCALE, HALFBORDERSCALE, DBLBORDERSCALE);
         glVertex3f(dis->dimentions.x / 2.0, dis->dimentions.y / 2.0, DBLBORDERSCALE);
      glEnd();

      // draw right
      glBegin(GL_TRIANGLE_STRIP);
         glNormal3f(1, 0, 0);
         glVertex3f(dis->dimentions.x, 0, 0);
         glVertex3f(dis->dimentions.x, dis->dimentions.y, 0);
         glVertex3f(dis->dimentions.x, 0, DBLBORDERSCALE - HALFBORDERSCALE);
         glVertex3f(dis->dimentions.x, dis->dimentions.y, DBLBORDERSCALE - HALFBORDERSCALE);
         glNormal3f(0, 0, 1);
         glVertex3f(dis->dimentions.x - HALFBORDERSCALE, HALFBORDERSCALE, DBLBORDERSCALE);
         glVertex3f(dis->dimentions.x - HALFBORDERSCALE, dis->dimentions.y - HALFBORDERSCALE, DBLBORDERSCALE);
         glVertex3f(dis->dimentions.x / 2.0, dis->dimentions.y / 2.0, DBLBORDERSCALE);
      glEnd();
}

void draw_filler(treenode *dis)
{
   // draw top triangle
   glBegin(GL_TRIANGLE_STRIP);
      glNormal3f(0, 1, 0);
      glVertex3f(dis->dimentions.x, dis->dimentions.y, 0);
      glVertex3f(0, dis->dimentions.y, 0);
      glVertex3f(dis->dimentions.x, dis->dimentions.y, DBLBORDERSCALE - HALFBORDERSCALE);
      glVertex3f(0, dis->dimentions.y, DBLBORDERSCALE - HALFBORDERSCALE);
      glNormal3f(0, 0, 1);
      glVertex3f(dis->dimentions.x - HALFBORDERSCALE, dis->dimentions.y - HALFBORDERSCALE, DBLBORDERSCALE);
      glVertex3f(HALFBORDERSCALE, dis->dimentions.y - HALFBORDERSCALE, DBLBORDERSCALE);
      glVertex3f(dis->dimentions.x / 2.0, dis->dimentions.y / 2.0, DBLBORDERSCALE);
   glEnd();

      // draw left side
      glBegin(GL_TRIANGLE_STRIP);
         glNormal3f(-1, 0, 0);
         glVertex3f(0, dis->dimentions.y, 0);
         glVertex3f(0, 0, 0);
         glVertex3f(0, dis->dimentions.y, DBLBORDERSCALE - HALFBORDERSCALE);
         glVertex3f(0, 0, DBLBORDERSCALE - HALFBORDERSCALE);
         glNormal3f(0, 0, 1);
         glVertex3f(HALFBORDERSCALE, dis->dimentions.y - HALFBORDERSCALE, DBLBORDERSCALE);
         glVertex3f(HALFBORDERSCALE, HALFBORDERSCALE, DBLBORDERSCALE);
         glVertex3f(dis->dimentions.x / 2.0, dis->dimentions.y / 2.0, DBLBORDERSCALE);
      glEnd();

      // draw bottom
      glBegin(GL_TRIANGLE_STRIP);
         glNormal3f(0, -1, 0);
         glVertex3f(0, 0, 0);
         glVertex3f(dis->dimentions.x, 0, 0);
         glVertex3f(0, 0, DBLBORDERSCALE - HALFBORDERSCALE);
         glVertex3f(dis->dimentions.x, 0, DBLBORDERSCALE - HALFBORDERSCALE);
         glNormal3f(0, 0, 1);
         glVertex3f(HALFBORDERSCALE, HALFBORDERSCALE, DBLBORDERSCALE);
         glVertex3f(dis->dimentions.x - HALFBORDERSCALE, HALFBORDERSCALE, DBLBORDERSCALE);
         glVertex3f(dis->dimentions.x / 2.0, dis->dimentions.y / 2.0, DBLBORDERSCALE);
      glEnd();

      // draw right
      glBegin(GL_TRIANGLE_STRIP);
         glNormal3f(0, 1, 0);
         glVertex3f(dis->dimentions.x, 0, 0);
         glVertex3f(dis->dimentions.x, dis->dimentions.y, 0);
         glVertex3f(dis->dimentions.x, 0, DBLBORDERSCALE - HALFBORDERSCALE);
         glVertex3f(dis->dimentions.x, dis->dimentions.y, DBLBORDERSCALE - HALFBORDERSCALE);
         glNormal3f(0, 0, 1);
         glVertex3f(dis->dimentions.x - HALFBORDERSCALE, HALFBORDERSCALE, DBLBORDERSCALE);
         glVertex3f(dis->dimentions.x - HALFBORDERSCALE, dis->dimentions.y - HALFBORDERSCALE, DBLBORDERSCALE);
         glVertex3f(dis->dimentions.x / 2.0, dis->dimentions.y / 2.0, DBLBORDERSCALE);
      glEnd();
}

void draw_menubutton(treenode *dis)
{
  GLfloat halfdepth = dis->dimentions.z / 2.0;

  if (((button *) (dis->prop))->state)
  {
    glBegin(GL_QUADS);
      glNormal3f(1.0, 0.0, halfdepth);
      glVertex3f(0.0, 0.0, dis->dimentions.z);
      glVertex3f(0.0, 0.0, halfdepth);
      glVertex3f(0.0, dis->dimentions.y, halfdepth);
      glVertex3f(0.0, dis->dimentions.y, dis->dimentions.z);

      glNormal3f(-1.0, 0.0, 0.0);
      glVertex3f(dis->dimentions.x, 0.0, halfdepth);
      glVertex3f(dis->dimentions.x, 0.0, dis->dimentions.z);
      glVertex3f(dis->dimentions.x, dis->dimentions.y, dis->dimentions.z);
      glVertex3f(dis->dimentions.x, dis->dimentions.y, halfdepth);

      glNormal3f(-1.0, 0.0, 0.0);
      glVertex3f(0.0, dis->dimentions.y, halfdepth);
      glVertex3f(dis->dimentions.x, dis->dimentions.y, halfdepth);
      glVertex3f(dis->dimentions.x, dis->dimentions.y, dis->dimentions.z);
      glVertex3f(0.0, dis->dimentions.y, dis->dimentions.z);

      glNormal3f(1.0, 0.0, 0.0);
      glVertex3f(dis->dimentions.x, 0.0, halfdepth);
      glVertex3f(0.0, 0.0, halfdepth);
      glVertex3f(0.0, 0.0, dis->dimentions.z);
      glVertex3f(dis->dimentions.x, 0.0, dis->dimentions.z);

      glNormal3f(0.0, 0.0, 1.0);
      glVertex3f(0.0, 0.0, halfdepth);
      glVertex3f(dis->dimentions.x, 0.0, halfdepth);
      glVertex3f(dis->dimentions.x, dis->dimentions.y, halfdepth);
      glVertex3f(0.0, dis->dimentions.y, halfdepth);
      dis->child->origin.z = halfdepth + 0.02;  // hack
    glEnd();
  }
  else 
  {
    glBegin(GL_QUADS);
      glNormal3f(0.0, 0.0, 1.0);
      glVertex3f(0.0, 0.0, dis->dimentions.z);
      glVertex3f(dis->dimentions.x, 0.0, dis->dimentions.z);
      glVertex3f(dis->dimentions.x, dis->dimentions.y, dis->dimentions.z);
      glVertex3f(0.0, dis->dimentions.y, dis->dimentions.z);
    glEnd();
    dis->child->origin.z = dis->dimentions.z + 0.02;  // hack
  }
}

void draw_border(treenode *dis)
{
  if(dis->dimentions.x > dis->dimentions.y)
  {
    if(dis->origin.y > (dis->parent->dimentions.y / 2))
    {
      glBegin(GL_TRIANGLE_STRIP);
        glNormal3f(0, 1, 0);
        glVertex3f(dis->dimentions.x, dis->dimentions.y, 0);
        glVertex3f(0, dis->dimentions.y, 0);
        glVertex3f(dis->dimentions.x, dis->dimentions.y, BORDERSCALE);
        glVertex3f(0, dis->dimentions.y, BORDERSCALE);
        glNormal3f(0, 0, 1);
        glVertex3f(dis->dimentions.x, dis->dimentions.y * 0.3, DBLBORDERSCALE);
        glVertex3f(0, dis->dimentions.y * 0.3, DBLBORDERSCALE);
        glNormal3f(0, -1, 0);
        glVertex3f(dis->dimentions.x, 0, BORDERSCALE);
        glVertex3f(0, 0, BORDERSCALE);
      glEnd();
    }
    else
    {
      glBegin(GL_TRIANGLE_STRIP);
        glNormal3f(0, 1, BORDERSCALE);
        glVertex3f(dis->dimentions.x, dis->dimentions.y, BORDERSCALE);
        glVertex3f(0, dis->dimentions.y, BORDERSCALE);
        glNormal3f(0, 0, 1);
        glVertex3f(dis->dimentions.x, dis->dimentions.y * 0.7, DBLBORDERSCALE);
        glVertex3f(0, dis->dimentions.y * 0.7, DBLBORDERSCALE);
        glNormal3f(0, -1, 0);
        glVertex3f(dis->dimentions.x, 0, BORDERSCALE);
        glVertex3f(0, 0, BORDERSCALE);
        glVertex3f(dis->dimentions.x, 0, 0);
        glVertex3f(0, 0, 0);
      glEnd();
    }
  }
  else
  {
    if(dis->origin.x > (dis->parent->dimentions.x / 2))
    {
      glBegin(GL_TRIANGLE_STRIP);
        glNormal3f(-1, 0, 0);
        glVertex3f(0, dis->dimentions.y, BORDERSCALE);
        glVertex3f(0, 0, BORDERSCALE);
        glNormal3f(0, 0, 1);
        glVertex3f(dis->dimentions.x * 0.3, dis->dimentions.y, DBLBORDERSCALE);
        glVertex3f(dis->dimentions.x * 0.3, 0, DBLBORDERSCALE);
        glNormal3f(1, 0, 0);
        glVertex3f(dis->dimentions.x, dis->dimentions.y, BORDERSCALE);
        glVertex3f(dis->dimentions.x, 0, BORDERSCALE);
        glVertex3f(dis->dimentions.x, dis->dimentions.y, 0);
        glVertex3f(dis->dimentions.x, 0, 0);
      glEnd();
    }
    else
    {
      glBegin(GL_TRIANGLE_STRIP);
        glNormal3f(-1, 0, 0);
        glVertex3f(0, dis->dimentions.y, 0);
        glVertex3f(0, 0, 0);
        glVertex3f(0, dis->dimentions.y, BORDERSCALE);
        glVertex3f(0, 0, BORDERSCALE);
        glNormal3f(0, 0, 1);
        glVertex3f(dis->dimentions.x * 0.7, dis->dimentions.y, DBLBORDERSCALE);
        glVertex3f(dis->dimentions.x * 0.7, 0, DBLBORDERSCALE);
        glNormal3f(1, 0, 0);
        glVertex3f(dis->dimentions.x, dis->dimentions.y, BORDERSCALE);
        glVertex3f(dis->dimentions.x, 0, BORDERSCALE);
      glEnd();
    }
  }
}

void draw_corner(treenode *dis)
{
  switch(((button *) (dis->prop))->state)
  {
    case 0:  // bottom-left
      glPushMatrix();
      if(dis->dimentions.x != 0.3375)
        glScalef(dis->dimentions.x / 0.3375, dis->dimentions.y / 0.3375, 1);
      glCallList(resources[RES_CORNERBL1]);
      glPopMatrix();
      break;

    case 1:  // top-left
      glPushMatrix();
      glTranslatef(0, dis->dimentions.y, 0);
      glRotatef(-90, 0.0, 0.0, 1.0);
      if(dis->dimentions.x != 0.3375)
        glScalef(dis->dimentions.x / 0.3375, dis->dimentions.y / 0.3375, 1);
      glCallList(resources[RES_CORNERBL1]);
      glPopMatrix();
      break;

    case 2:  // bottom-right
      glPushMatrix();
      glTranslatef(dis->dimentions.x, 0, 0);
      glRotatef(90, 0.0, 0.0, 1.0);
      if(dis->dimentions.x != 0.3375)
        glScalef(dis->dimentions.x / 0.3375, dis->dimentions.y / 0.3375, 1);
      glCallList(resources[RES_CORNERBL1]);
      glPopMatrix();
      break;

    case 3:  // bottom-right
      glPushMatrix();
      glTranslatef(dis->dimentions.x, dis->dimentions.y, 0);
      glRotatef(180, 0.0, 0.0, 1.0);
      if(dis->dimentions.x != 0.3375)
        glScalef(dis->dimentions.x / 0.3375, dis->dimentions.y / 0.3375, 1);
      glCallList(resources[RES_CORNERBL1]);
      glPopMatrix();
      break;

    default:
      draw_hardcorner(dis);
      break;
  }
}

#define PITIN 0.05        // MOVE ME
#define HALFPITIN 0.025   // MOVE ME
#define BORDERSIZE 0.0625
#define HALFBORDERSIZE 0.03125

int collide_corner(treenode *dis, point *pt)
{
  int state;
  float size;

  size = (dis->dimentions.x / 0.3375) * BORDERSIZE;
  state = ((button *) (dis->prop))->state;

  if(state == 0 || state == 1)
  {
    if(pt->x < size)
      return 1;
  }
  else
  {
    if(pt->x > (dis->dimentions.x - size))
      return 1;
  }

  if(state == 0 || state == 2)
  {
    if(pt->y < size)
      return 1;
  }
  else
  {
    if(pt->y > (dis->dimentions.y - size))
      return 1;
  }

  return 0;
}

// this function is zooming huge.  it screams "make me into a display list!"
// (which i'll do later, when tweaking is done -- njm)
void draw_hardcorner(treenode *dis)
{
  switch(((button *) (dis->prop))->state)
  {
    case 0:  // bottom-left
      glBegin(GL_QUAD_STRIP);
      printf("Q\n");
        glNormal3f(-1.0, 0.0, 0.0);
        printf("[%f %f %f ]", -1.0, 0.0, 0.0);
        glVertex3f(0.0, dis->dimentions.y, 0.0);
        printf("{%f %f %f }", 0.0, dis->dimentions.y, 0.0);
        glVertex3f(HALFBORDERSIZE, dis->dimentions.y - HALFPITIN, 0.0);
        printf("{%f %f %f }", HALFBORDERSIZE, dis->dimentions.y - HALFPITIN, 0.0);
        glVertex3f(0.0, dis->dimentions.y, BORDERSCALE);
        printf("{%f %f %f }", 0.0, dis->dimentions.y, BORDERSCALE);
        glVertex3f(HALFBORDERSIZE, dis->dimentions.y - HALFPITIN, HALFBORDERSCALE);
        printf("{%f %f %f }", HALFBORDERSIZE, dis->dimentions.y - HALFPITIN, HALFBORDERSCALE);
        glNormal3f(0.0, 0.0, 1.0);
        printf("[%f %f %f ]", 0.0, 0.0, 1.0);
        glVertex3f(BORDERSIZE * 0.7, dis->dimentions.y, DBLBORDERSCALE);
        printf("{%f %f %f }", BORDERSIZE * 0.7, dis->dimentions.y, DBLBORDERSCALE);
        glVertex3f(BORDERSIZE * 0.85, dis->dimentions.y - HALFPITIN, BORDERSCALE);
        printf("{%f %f %f }", BORDERSIZE * 0.85, dis->dimentions.y - HALFPITIN, BORDERSCALE);
        glNormal3f(1.0, 0.0, 0.0);
        printf("[%f %f %f ]", 1.0, 0.0, 0.0);
        glVertex3f(BORDERSIZE, dis->dimentions.y, BORDERSCALE);
        printf("{%f %f %f }", BORDERSIZE, dis->dimentions.y, BORDERSCALE);
        glVertex3f(BORDERSIZE, dis->dimentions.y - HALFPITIN, HALFBORDERSCALE);
        printf("{%f %f %f }", BORDERSIZE, dis->dimentions.y - HALFPITIN, HALFBORDERSCALE);
      glEnd();
      printf("\nE\n");

      glBegin(GL_QUAD_STRIP);
      printf("Q\n");
        glNormal3f(-1.0, 0.0, 0.0);
        printf("[%f %f %f ]", -1.0, 0.0, 0.0);
        glVertex3f(HALFBORDERSIZE, dis->dimentions.y - HALFPITIN, 0.0);
        printf("{%f %f %f }", HALFBORDERSIZE, dis->dimentions.y - HALFPITIN, 0.0);
        glVertex3f(0.0, dis->dimentions.y - PITIN, 0.0);
        printf("{%f %f %f }", 0.0, dis->dimentions.y - PITIN, 0.0);
        glVertex3f(HALFBORDERSIZE, dis->dimentions.y - HALFPITIN, HALFBORDERSCALE);
        printf("{%f %f %f }", HALFBORDERSIZE, dis->dimentions.y - HALFPITIN, HALFBORDERSCALE);
        glVertex3f(0.0, dis->dimentions.y - PITIN, BORDERSCALE);
        printf("{%f %f %f }", 0.0, dis->dimentions.y - PITIN, BORDERSCALE);
        glNormal3f(0.0, 0.0, 1.0);
        printf("[%f %f %f ]", 0.0, 0.0, 1.0);
        glVertex3f(BORDERSIZE * 0.85, dis->dimentions.y - HALFPITIN, BORDERSCALE);
        printf("{%f %f %f }", BORDERSIZE * 0.85, dis->dimentions.y - HALFPITIN, BORDERSCALE);
        glVertex3f(BORDERSIZE * 0.7, dis->dimentions.y - PITIN, DBLBORDERSCALE);
        printf("{%f %f %f }", BORDERSIZE * 0.7, dis->dimentions.y - PITIN, DBLBORDERSCALE);
        glNormal3f(1.0, 0.0, 0.0);
        printf("[%f %f %f ]", 1.0, 0.0, 0.0);
        glVertex3f(BORDERSIZE, dis->dimentions.y - HALFPITIN, HALFBORDERSCALE);
        printf("{%f %f %f }", BORDERSIZE, dis->dimentions.y - HALFPITIN, HALFBORDERSCALE);
        glVertex3f(BORDERSIZE, dis->dimentions.y - PITIN, BORDERSCALE);
        printf("{%f %f %f }", BORDERSIZE, dis->dimentions.y - PITIN, BORDERSCALE);
      glEnd();
      printf("\nE\n");

      glBegin(GL_QUAD_STRIP);
      printf("Q\n");
        glNormal3f(-1.0, 0.0, 0.0);
        printf("[%f %f %f ]", -1.0, 0.0, 0.0);
        glVertex3f(0.0, dis->dimentions.y - PITIN, 0.0);
        printf("{%f %f %f }", 0.0, dis->dimentions.y - PITIN, 0.0);
        glVertex3f(0.0, 0.0, 0.0);
        printf("{%f %f %f }", 0.0, 0.0, 0.0);
        glVertex3f(0.0, dis->dimentions.y - PITIN, BORDERSCALE);
        printf("{%f %f %f }", 0.0, dis->dimentions.y - PITIN, BORDERSCALE);
        glVertex3f(0.0, 0.0, BORDERSCALE);
        printf("{%f %f %f }", 0.0, 0.0, BORDERSCALE);
        glNormal3f(0.0, 0.0, 1.0);
        printf("[%f %f %f ]", 0.0, 0.0, 1.0);
        glVertex3f(BORDERSIZE * 0.7, dis->dimentions.y - PITIN, DBLBORDERSCALE);
        printf("{%f %f %f }", BORDERSIZE * 0.7, dis->dimentions.y - PITIN, DBLBORDERSCALE);
        glVertex3f(BORDERSIZE * 0.7, BORDERSIZE * 0.7, DBLBORDERSCALE);
        printf("{%f %f %f }", BORDERSIZE * 0.7, BORDERSIZE * 0.7, DBLBORDERSCALE);
        glNormal3f(1.0, 0.0, 0.0);
        printf("[%f %f %f ]", 1.0, 0.0, 0.0);
        glVertex3f(BORDERSIZE, dis->dimentions.y - PITIN, BORDERSCALE);
        printf("{%f %f %f }", BORDERSIZE, dis->dimentions.y - PITIN, BORDERSCALE);
        glVertex3f(BORDERSIZE, BORDERSIZE, BORDERSCALE);
        printf("{%f %f %f }", BORDERSIZE, BORDERSIZE, BORDERSCALE);
      glEnd();
      printf("\nE\n");

      glBegin(GL_QUAD_STRIP);
      printf("Q\n");
        glNormal3f(0.0, -1.0, 0.0);
        printf("[%f %f %f ]", 0.0, -1.0, 0.0);
        glVertex3f(0.0, 0.0, 0.0);
        printf("{%f %f %f }", 0.0, 0.0, 0.0);
        glVertex3f(dis->dimentions.x - PITIN, 0.0, 0.0);
        printf("{%f %f %f }", dis->dimentions.x - PITIN, 0.0, 0.0);
        glVertex3f(0.0, 0.0, BORDERSCALE);
        printf("{%f %f %f }", 0.0, 0.0, BORDERSCALE);
        glVertex3f(dis->dimentions.x - PITIN, 0.0, BORDERSCALE);
        printf("{%f %f %f }", dis->dimentions.x - PITIN, 0.0, BORDERSCALE);
        glNormal3f(0.0, 0.0, 1.0);
        printf("[%f %f %f ]", 0.0, 0.0, 1.0);
        glVertex3f(BORDERSIZE * 0.7, BORDERSIZE * 0.7, DBLBORDERSCALE);
        printf("{%f %f %f }", BORDERSIZE * 0.7, BORDERSIZE * 0.7, DBLBORDERSCALE);
        glVertex3f(dis->dimentions.x - PITIN, BORDERSIZE * 0.7, DBLBORDERSCALE);
        printf("{%f %f %f }", dis->dimentions.x - PITIN, BORDERSIZE * 0.7, DBLBORDERSCALE);
        glNormal3f(0.0, 1.0, 0.0);
        printf("[%f %f %f ]", 0.0, 1.0, 0.0);
        glVertex3f(BORDERSIZE, BORDERSIZE, BORDERSCALE);
        printf("{%f %f %f }", BORDERSIZE, BORDERSIZE, BORDERSCALE);
        glVertex3f(dis->dimentions.x - PITIN, BORDERSIZE, BORDERSCALE);
        printf("{%f %f %f }", dis->dimentions.x - PITIN, BORDERSIZE, BORDERSCALE);
      glEnd();
      printf("\nE\n");

      glBegin(GL_QUAD_STRIP);
      printf("Q\n");
        glNormal3f(0.0, -1.0, 0.0);
        printf("[%f %f %f ]", 0.0, -1.0, 0.0);
        glVertex3f(dis->dimentions.x - PITIN, 0.0, 0.0);
        printf("{%f %f %f }", dis->dimentions.x - PITIN, 0.0, 0.0);
        glVertex3f(dis->dimentions.x - HALFPITIN, HALFBORDERSIZE, 0.0);
        printf("{%f %f %f }", dis->dimentions.x - HALFPITIN, HALFBORDERSIZE, 0.0);
        glVertex3f(dis->dimentions.x - PITIN, 0.0, BORDERSCALE);
        printf("{%f %f %f }", dis->dimentions.x - PITIN, 0.0, BORDERSCALE);
        glVertex3f(dis->dimentions.x - HALFPITIN, HALFBORDERSIZE, HALFBORDERSCALE);
        printf("{%f %f %f }", dis->dimentions.x - HALFPITIN, HALFBORDERSIZE, HALFBORDERSCALE);
        glNormal3f(0.0, 0.0, 1.0);
        printf("[%f %f %f ]", 0.0, 0.0, 1.0);
        glVertex3f(dis->dimentions.x - PITIN, BORDERSIZE * 0.7, DBLBORDERSCALE);
        printf("{%f %f %f }", dis->dimentions.x - PITIN, BORDERSIZE * 0.7, DBLBORDERSCALE);
        glVertex3f(dis->dimentions.x - HALFPITIN, BORDERSIZE * 0.85, BORDERSCALE);
        printf("{%f %f %f }", dis->dimentions.x - HALFPITIN, BORDERSIZE * 0.85, BORDERSCALE);
        glNormal3f(0.0, 1.0, 0.0);
        printf("[%f %f %f ]", 0.0, 1.0, 0.0);
        glVertex3f(dis->dimentions.x - PITIN, BORDERSIZE, BORDERSCALE);
        printf("{%f %f %f }", dis->dimentions.x - PITIN, BORDERSIZE, BORDERSCALE);
        glVertex3f(dis->dimentions.x - HALFPITIN, BORDERSIZE, HALFBORDERSCALE);
        printf("{%f %f %f }", dis->dimentions.x - HALFPITIN, BORDERSIZE, HALFBORDERSCALE);
      glEnd();
      printf("\nE\n");

      glBegin(GL_QUAD_STRIP);
      printf("Q\n");
        glNormal3f(0.0, -1.0, 0.0);
        printf("[%f %f %f ]", 0.0, -1.0, 0.0);
        glVertex3f(dis->dimentions.x - HALFPITIN, HALFBORDERSIZE, 0.0);
        printf("{%f %f %f }", dis->dimentions.x - HALFPITIN, HALFBORDERSIZE, 0.0);
        glVertex3f(dis->dimentions.x, 0.0, 0.0);
        printf("{%f %f %f }", dis->dimentions.x, 0.0, 0.0);
        glVertex3f(dis->dimentions.x - HALFPITIN, HALFBORDERSIZE, HALFBORDERSCALE);
        printf("{%f %f %f }", dis->dimentions.x - HALFPITIN, HALFBORDERSIZE, HALFBORDERSCALE);
        glVertex3f(dis->dimentions.x, 0.0, BORDERSCALE);
        printf("{%f %f %f }", dis->dimentions.x, 0.0, BORDERSCALE);
        glNormal3f(0.0, 0.0, 1.0);
        printf("[%f %f %f ]", 0.0, 0.0, 1.0);
        glVertex3f(dis->dimentions.x - HALFPITIN, BORDERSIZE * 0.85, BORDERSCALE);
        printf("{%f %f %f }", dis->dimentions.x - HALFPITIN, BORDERSIZE * 0.85, BORDERSCALE);
        glVertex3f(dis->dimentions.x, BORDERSIZE * 0.7, DBLBORDERSCALE);
        printf("{%f %f %f }", dis->dimentions.x, BORDERSIZE * 0.7, DBLBORDERSCALE);
        glNormal3f(0.0, 1.0, 0.0);
        printf("[%f %f %f ]", 0.0, 1.0, 0.0);
        glVertex3f(dis->dimentions.x - HALFPITIN, BORDERSIZE, HALFBORDERSCALE);
        printf("{%f %f %f }", dis->dimentions.x - HALFPITIN, BORDERSIZE, HALFBORDERSCALE);
        glVertex3f(dis->dimentions.x, BORDERSIZE, BORDERSCALE);
        printf("{%f %f %f }", dis->dimentions.x, BORDERSIZE, BORDERSCALE);
      glEnd();
      printf("\nE\n\n\n\n");
      break;
  }
}

int destroy_button(treenode *disone)
{
  free(disone->prop);
  disone->prop = 0;
  return 0;
}

treenode *create_button(treenode *parent, GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height,
 GLfloat rotx, GLfloat roty, GLfloat rotz, GLfloat *color, int type, int subtype, char * label, int visible)
{
   treenode *temp;

   temp = create_child(parent, sizeof(treenode));
   if(!temp)
   {
      return 0;
      exit(1);
   }

   temp->visible = VISIBLE;
   temp->type = type;
   temp->origin.x = x;
   temp->origin.y = y;
   temp->origin.z = z;
   temp->dimentions.x = width;
   temp->dimentions.y = height;
   temp->dimentions.z = DBLBORDERSCALE;
   temp->rotation.x = rotx;
   temp->rotation.y = roty;
   temp->rotation.z = rotz;
   if(!color)
   {
      temp->color[0] = temp->parent->color[0] + 0.1;
      temp->color[1] = temp->parent->color[1] + 0.1;
      temp->color[2] = temp->parent->color[2] + 0.1;
      temp->color[3] = temp->parent->color[3];
   }
   else
   {
      temp->color[0] = color[0];
      temp->color[1] = color[1];
      temp->color[2] = color[2];
      temp->color[3] = color[3];
   }
   temp->visible = visible;
   temp->focusable = TRUE;
   temp->autoresolve = FALSE;
   temp->mask = MASK_ALL & ~(MASK_KEYPRESS | MASK_MOUSEDOWN | MASK_MOUSEUP | MASK_MOUSEDRAG | MASK_COLLIDE);
   temp->event = event_button;
   temp->prop = malloc(sizeof(button));
   ((button *)(temp->prop))->state = FALSE;
   temp->subtype = subtype;
 
   switch(subtype)
   {
      case PUSH:
      if(create_push(temp, label))
      {
         destroy_node(temp);
         return(0);
      }
      break;

      case MENUBUTTON:
      if(create_menubutton(temp, label))
      {
         destroy_node(temp);
         return(0);
      }
      break;

      case CHECK:
      if(create_check(temp, label))
      {
         destroy_node(temp);
         return(0);
      }
      break;

      case RADIO:
      if (create_radio(temp, label))
      {
         destroy_node(temp);
         return(0);
      }
      break;

      case BORDER:
      if (create_border(temp))
      {
         destroy_node(temp);
         return(0);
      }
      break;

      case CORNER:
      if (create_corner(temp))
      {
         destroy_node(temp);
         return(0);
      }
      break;

      case FILLER:
      if (create_filler(temp))
      {
         destroy_node(temp);
         return(0);
      }
      break;

      default:
         destroy_node(temp);
         return(0);
         break; // haha
   }

   make_bbox(temp);

   return temp;
}

int create_push(treenode *temp, char *label)
{
   treenode *thing;

   if(label)
   {
      thing = create_text(temp, 0, 0, DBLBORDERSCALE + 0.0075 /* + HALFBORDERSCALE */, temp->dimentions.x, 0.0 /* 0.1 */, 0, 0, 0, 0, 12, LABEL, find_font_by_name("20thcent.ttf"), label);
      thing->dimentions.x = get_text_width(thing);
      thing->dimentions.y = get_text_height(thing);
      thing->origin.x = (thing->parent->dimentions.x - thing->dimentions.x) / 2.0;
      thing->origin.y = (thing->parent->dimentions.y - thing->dimentions.y) / 2.0;
      make_bbox(thing);
//    center_text_y(thing);
   }
   return 0;
}

int create_check(treenode *temp, char *label)
{
   treenode *lbl;

   lbl = create_text(temp, 0.15, 0, 0.125 /* DBLBORDERSCALE + 0.1 */, 1, 1, 0, 0, 0, 0, 8, LABEL, find_font_by_name("20thcent.ttf"), label);
   lbl->dimentions.x = get_text_width(lbl);
   lbl->dimentions.y = get_text_height(lbl);
   make_bbox(lbl);
   return 0;
}

int create_menubutton(treenode *temp, char *label)
{
   treenode *thing;

   temp->dimentions.z = DBLBORDERSCALE;

   thing = create_text(temp, 0, 0, temp->dimentions.z + 0.1, temp->dimentions.x, 0.1, 0, 0, 0, 0, 9, LABEL, find_font_by_name("20thcent.ttf"), label);

   return 0;
}

int create_radio(treenode *temp, char *label)
{
   treenode *lbl;

   lbl = create_text(temp, 0.15, 0, 0.125 /* DBLBORDERSCALE + 0.1 */, 1, 1, 0, 0, 0, 0, 8, LABEL, find_font_by_name("20thcent.ttf"), label);
   lbl->dimentions.x = get_text_width(lbl);
   lbl->dimentions.y = get_text_height(lbl);
   make_bbox(lbl);
// create_text(temp, 0, 0, DBLBORDERSCALE + 0.01, 1, 1, 0, 0, 0, 0, 12, LABEL, find_font_by_name("20thcent.ttf"), label);
   return 0;
}

int create_filler(treenode *temp)
{
   return 0;
}

int create_border(treenode *temp)
{
   return 0;
}

// corner state variable
// 1-------3
// |       |
// 0-------2

int create_corner(treenode *temp)
{
   // this code is as close to AI as this program gets (for now) :P

   if (temp->origin.x < temp->parent->dimentions.x / 2)  // on the left side
   {
      if (temp->origin.y < temp->parent->dimentions.y / 2) // on the bottom
         ((button *) (temp->prop))->state = 0;
      else
         ((button *) (temp->prop))->state = 1;  // on the top
   }
   else  // on the right
   {
      if (temp->origin.y < temp->parent->dimentions.y / 2) // on the bottom
         ((button *) (temp->prop))->state = 2;
      else
         ((button *) (temp->prop))->state = 3;  // on the top
   }

   return 0;
}
