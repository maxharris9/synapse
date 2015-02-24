/* text.c -  
 * text-handling routines for System 1.0. 
 * Copyright(c) 1998 Objective Reality Computer.  
 * By Nick McKinney and Max Harris. 
 */

#include <GL/glut.h> // for glutSolidCube() on line 59
#include <string.h>  // for strncpy()
#include <stdarg.h> // for vsprintf()
#include <stdlib.h> // for malloc() in make_button()
#include <stdio.h> // for NULL in make_button()
// #include "drag.h" // for startslide(), stopslide(), and others
#include "draw.h" // for border()
#include "font.h" // for real_output()
#include "menu.h" // for show_menu(), hide_menu()
#include "window.h" // for set_active_window(), move_window(), and others
#include "font.h"
#include "text.h"
#include "button.h"
#include "coord.h"
#include "bar.h"
#include "barmanager.h"
#include "event.h"

int reshape_text(treenode *thing, int which, GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height, GLfloat rotx, GLfloat roty, GLfloat rotz)
{
  switch(thing->subtype)
  {
    case MULTIBOX:
      return reshape_multibox(thing, which, x, y, z, width, height, rotx, roty, rotz);

    default:
      return -1;
  }
}

int reshape_multibox(treenode *thing, int which, GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height, GLfloat rotx, GLfloat roty, GLfloat rotz)
{
  if(which & 1)
  {
    thing->origin.x = x;
    thing->origin.y = y;
    thing->origin.z = z;
  }

  if(which & 2)
  {
    thing->dimentions.x = width - 0.25;
    thing->dimentions.y = height;
    adjust_multiline_scrollers(thing);
    resize_scrollbar(((text *)(thing->prop))->vbar, width - 0.25, 0, 0, 0.25, height, 0, 0, 0);
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

void adjust_multiline_scrollers(treenode *dis)
{
  text *prp;
  int max;
  int window, value;
  GLfloat base2base;

  prp = (text *)(dis->prop);
  base2base = prp->typeface->base2base * (prp->ptsize / 24.0);

  max = get_text_height(dis) / base2base;
  window = dis->dimentions.y / base2base;
  value = prp->beginoffset.y / base2base;

  change_slider_values(((scrollbarprop *)(prp->vbar->prop))->slider, 0, max, window, value);
}

int labelcat(treenode *thing, unsigned long len, char *addition)
{
  char *newlabel;

  newlabel = malloc(strlen(((text *)(thing->prop))->label) + len + 1);
  strcpy(newlabel, ((text *)(thing->prop))->label);
  strncat(newlabel, addition, len);
  return change_text_label(thing, newlabel);
}

int change_text_label(treenode *thing, char *label)
{
  if(((text *)(thing->prop))->label)
    free(((text *)(thing->prop))->label);
  ((text *)(thing->prop))->label = strdup(label);
  if(thing->subtype == MULTIBOX)
    adjust_multiline_scrollers(thing);
  return 0;
}

int event_text(struct treenode *target, struct treenode *node, int event, int modifier, point *where, unsigned long usec)
{
  switch(event)
  {
    case EVENT_MOUSEDOWN:
       set_active_node(target);
       select_text(target, where);
       break;

    case EVENT_MOUSEUP:
       break;

    case EVENT_COLLIDE:
       if(target->subtype == LABEL)
         return 0;
       return 1;
       break;

    case EVENT_MOUSEDRAG:
       select_more_text(target, where);
       break;

    case EVENT_KEYPRESS:  // this will work for single and multi-line text
    {
       char *newlabel;
       int newlen, ibegin, iend;
       struct text *prp;

       prp = (text *)(target->prop);
       newlen = strlen(prp->label);
       if(prp->begin > prp->end)
       {
         ibegin = prp->end;
         iend = prp->begin;
       }
       else
       {
         ibegin = prp->begin;
         iend = prp->end;
       }

       switch(modifier)
       {
         case 8:  // backspace
           if(ibegin == iend)
           {
             if(ibegin > 0)
               ibegin--;
             else
               return 0;
           }
           newlen -= iend - ibegin;
           break;

         case SYN_KEY_UP:
           return 0;
           break;

         case SYN_KEY_DOWN:
           return 0;
           break;

         case SYN_KEY_LEFT:
           if(iend > ibegin)
           {
             prp->end = prp->begin = ibegin;
             return 0;
           }

           if(ibegin > 0)
             prp->begin = prp->end = ibegin - 1;

           return 0;
           break;

         case SYN_KEY_RIGHT:
           if(iend > ibegin)
           {   
             prp->end = prp->begin = iend;
             return 0;
           }
           
           if(iend < newlen)
             prp->begin = prp->end = iend + 1;

           return 0;
           break;

         case '\r':
           modifier = '\n';  // FALL THROUGH
         default:
           newlen += 1 - (iend - ibegin);
       }

       newlabel = (char *) malloc(newlen + 1);

       switch(modifier)
       {
         case 8:
           strncpy(newlabel, prp->label, ibegin);
           newlabel[ibegin] = '\0';
           strcat(newlabel, &(prp->label[iend]));
           prp->end = prp->begin = ibegin;;
           break;

         default:
           if(ibegin > 0)
             strncpy(newlabel, prp->label, ibegin);
           else
             newlabel[0] = '\0';
           newlabel[ibegin] = (char) modifier;
           newlabel[++ibegin] = '\0';
           strcat(newlabel, &(prp->label[iend]));
           prp->end = prp->begin = ibegin;;
           break;
       }
       free(prp->label);
       prp->label = newlabel;
       if(target->subtype == MULTIBOX)
         adjust_multiline_scrollers(target);
       }
         break;

    case EVENT_CHANGE:
    {
       struct text *prp;

       prp = (text *)(target->prop);
       prp->beginoffset.y = modifier * prp->typeface->base2base * (prp->ptsize / 24.0);
    }
       break;

    default:
       printf("dar unrecognized event %d\n", event);
       break;
  }
  return 0;
}

int get_pos_from_point(treenode *dis, point *where)
{
  char *label;
  char current;
  point base;
  text *thing;
  GLfloat base2base = 0.0;
  int textat = 0;

  thing = (text *)(dis->prop);
  label = thing->label;

  base2base = thing->typeface->base2base * (thing->ptsize / 24.0);
  base.x = 0.0;
  base.z = 0.0;
//  base.y = dis->dimentions.y - base2base;
  base.y = dis->dimentions.y - base2base + thing->beginoffset.y;;

  if(where->y > (base.y + base2base))
    return 0;  // selection at the beginning

  while(*label)
  {
    current = *label;

    if (current == '\n' || current == '\r')
    {
      if(base.y < where->y)
        return textat;
      base.x = 0.0;
      base.y -= base2base;
      if(base.y < -base2base)
        break;
    }
    else
    {
      if(base.x + thing->typeface->metrics[(int)current].xadvance * (thing->ptsize / 24.0) > where->x &&
            where->y > base.y &&
            where->y < (base.y + base2base))
         return textat;

      base.x += thing->typeface->metrics[(int)current].xadvance * (thing->ptsize / 24.0);
    }
    label++;
    textat++;
  }
  return textat;
}

void select_text(treenode *target, point *where)
{
  ((text *)(target->prop))->selbegin.x = where->x;
  ((text *)(target->prop))->selbegin.y = where->y;
  ((text *)(target->prop))->selbegin.z = where->z;

  ((text *)(target->prop))->end = ((text *)(target->prop))->begin = get_pos_from_point(target, &(((text *)(target->prop))->selbegin));

  ((text *)(target->prop))->selat = ((text *)(target->prop))->selbegin;
}

void select_more_text(treenode *target, point *where)
{
  ((text *)(target->prop))->selat.x += where->x;
  ((text *)(target->prop))->selat.y += where->y;
  ((text *)(target->prop))->selat.z += where->z;

  ((text *)(target->prop))->end = get_pos_from_point(target, &(((text *)(target->prop))->selat));
}

GLfloat get_text_width(treenode *dis)
{
  return get_string_width(((text *)(dis->prop))->label, ((text *)(dis->prop))->typeface, ((text *)(dis->prop))->ptsize);
}
/*
GLfloat get_text_width(treenode *dis)
{
  return get_string_width(((text *)(dis->prop))->label, ((text *)(dis->prop))->typeface, ((text *)(dis->prop))->ptsize);
}
*/
GLfloat get_text_height(treenode *dis)
{
  return get_string_height(((text *)(dis->prop))->label, ((text *)(dis->prop))->typeface, ((text *)(dis->prop))->ptsize);
}

void draw_text_tray(treenode *dis)
{
  point origin;
  point dim;

  origin.x = origin.y = 0;
  origin.z = BORDERSCALE;
  dim.x = dis->dimentions.x;
  dim.y = dis->dimentions.y;
  dim.z = -BORDERSCALE;

  new_border(&origin, &dim, HALFBORDERSCALE, FALSE, 0.05); // 0.25

  // draw top triangle
  glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    glVertex3f(HALFBORDERSCALE, HALFBORDERSCALE, 0.0);
    glVertex3f(dis->dimentions.x - HALFBORDERSCALE, HALFBORDERSCALE, 0.0);
    glVertex3f(dis->dimentions.x - HALFBORDERSCALE, dis->dimentions.y - HALFBORDERSCALE, 0.0);
    glVertex3f(HALFBORDERSCALE, dis->dimentions.y - HALFBORDERSCALE, 0.0);
  glEnd();
}

int color_haq(treenode *dis)
{
   GLfloat color = 0;

   color = ((dis->color[0] * 0.296875) + (dis->color[1] * 0.5859375) + (dis->color[2] * 0.11328125));

   if (color > 0.3333333)
      return(0);
   else
      return(1);
}

void draw_text(treenode *dis)
{
  switch(dis->subtype)
  {
    case LABEL:
    {
//       glColor4fv
       draw_bitmap_text(dis, -1, -1);
       break;
    }

    case BOX:
    case MULTIBOX:
    {
//    GLfloat begin, end;
//    char *temp;
      int ibegin, iend;

      glColor4f(1.0, 1.0, 1.0, 1.0);
//    glColor4fv(dis->color);

      glPushMatrix();
         glTranslatef(0, 0, 0.05);
         draw_text_tray(dis);
      glPopMatrix();

      ibegin = ((text *)(dis->prop))->begin;
      iend = ((text *)(dis->prop))->end;
      if(ibegin > iend)
      {
        iend = ((text *)(dis->prop))->begin;
        ibegin = ((text *)(dis->prop))->end;
      }

      glPushMatrix();
      {
         glColor4fv(dis->color);
         glTranslatef(0.0, 0.0, 0.15);
         draw_bitmap_text(dis, ibegin, iend);
      }
      glPopMatrix();
    }
      break;
  }
}

#define ORIGIN thing->typeface->origin[(int)current]
#define DIMENSIONS thing->typeface->dimensions[(int)current]

// a lot of the following could be cached instead of
// being regenerated every refresh
// (possibly with a display list or a set of them)
void draw_bitmap_text(treenode *dis, int selbegin, int selend)
{
  char *label;
  char current;
  point base;
  text *thing;
  int texture = -1;
  GLfloat xbearing, ybearing, xsize, ysize;
  GLfloat white[4] = {1, 1, 1, 1};
  GLfloat saveamb[4];
  GLfloat base2base = 0.0;

  point beginsel;
  beginsel.x = 0; beginsel.y = 0;
  point endsel;
  int inselection = 0, textat = 0;

  GLdouble left[4] = {1.0, 0.0, 0.0, 0.0};
  GLdouble down[4] = {0.0, 1.0, 0.0, 0.0};
  GLdouble right[4] = {-1.0, 0.0, 0.0, 0.0};
  GLdouble up[4] = {0.0, -1.0, 0.0, 0.0};

  if(dis->subtype != LABEL)
  {
    glPushMatrix();

    glTranslatef(0.05, 0.05, 0);

    glClipPlane(GL_CLIP_PLANE0, left);
    glEnable(GL_CLIP_PLANE0);
    glClipPlane(GL_CLIP_PLANE1, down);
    glEnable(GL_CLIP_PLANE1);

    glTranslatef(dis->dimentions.x - 0.1, dis->dimentions.y - 0.1, dis->dimentions.z);

    glClipPlane(GL_CLIP_PLANE2, right);
    glEnable(GL_CLIP_PLANE2);
    glClipPlane(GL_CLIP_PLANE3, up);
    glEnable(GL_CLIP_PLANE3);

    glPopMatrix();
  }

  thing = (text *)(dis->prop);
  label = thing->label;

  base2base = thing->typeface->base2base * (thing->ptsize / 24.0);
  base.x = 0.0;
  base.z = 0.0;
  base.y = dis->dimentions.y - (thing->typeface->ascent * (thing->ptsize / 24.0)) + thing->beginoffset.y;;

  if(dis->subtype == MULTIBOX || dis->subtype == BOX)
  {
    base.x += 0.05;
    base.y -= 0.05;
  }

  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
//  glEnable(GL_BLEND);
  glAlphaFunc(GL_GEQUAL, 0.0625);
  glGetLightfv(GL_LIGHT0, GL_AMBIENT, saveamb);
  glLightfv(GL_LIGHT0, GL_AMBIENT, white);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glNormal3f(0, 0, 1);
  glEnable(GL_ALPHA_TEST);

  glPushMatrix();
  glTranslatef(base.x, base.y, base.z);
  while(*label)
  {
    current = *label;

    if(textat == selbegin)
    {
      inselection = 1;
      glColor4f(1.0 - dis->color[0], 1.0 - dis->color[1], 1.0 - dis->color[2], 1);
      beginsel = base;
      beginsel.z -= 0.05;
    }

    if(textat == selend)
    {
      glPopMatrix();
      glPushMatrix();
      glColor4f(dis->color[0], dis->color[1], dis->color[2], 1);
      inselection = 0;
      endsel = base;
      endsel.y += base2base;
      endsel.z -= 0.05;

      if(selend > selbegin)
      {
        glDisable(GL_ALPHA_TEST);
        glDisable(GL_TEXTURE_2D);
        glBegin(GL_QUADS);
          glVertex3f(beginsel.x, endsel.y, beginsel.z);
          glVertex3f(beginsel.x, beginsel.y, beginsel.z);
          glVertex3f(endsel.x, beginsel.y, endsel.z);
          glVertex3f(endsel.x, endsel.y, endsel.z);
        glEnd();
        glEnable(GL_ALPHA_TEST);
        glEnable(GL_TEXTURE_2D);
      }
      else
      {
        glDisable(GL_ALPHA_TEST);
        glDisable(GL_TEXTURE_2D);
        glBegin(GL_LINES);
          glVertex3f(beginsel.x, beginsel.y, beginsel.z);
          glVertex3f(endsel.x, endsel.y, endsel.z);
        glEnd();
        glEnable(GL_ALPHA_TEST);
        glEnable(GL_TEXTURE_2D);
      }
      glPopMatrix();
      glPushMatrix();
      glTranslatef(base.x, base.y, base.z);
      glColor4f(dis->color[0], dis->color[1], dis->color[2], dis->color[3]);
    }

    if(current > 32)
    {
      if(!(base.x > dis->dimentions.x || base.x < 0.0) && base.y < dis->dimentions.y+base2base)
      {
        xbearing = thing->typeface->metrics[(int)current].xbearing * (thing->ptsize / 24.0);
        ybearing = thing->typeface->metrics[(int)current].ybearing * (thing->ptsize / 24.0);
        xsize = thing->typeface->metrics[(int)current].xsize * (thing->ptsize / 24.0);
        ysize = thing->typeface->metrics[(int)current].ysize * (thing->ptsize / 24.0);
        if(texture != thing->typeface->textures[thing->typeface->origin[(int)current].z])
        {
          glBindTexture(GL_TEXTURE_2D, thing->typeface->textures[thing->typeface->origin[(int)current].z]);
          texture = thing->typeface->textures[thing->typeface->origin[(int)current].z];
        }
        glBegin(GL_QUADS);
          glTexCoord2d(ORIGIN.x / 256.0, (ORIGIN.y + DIMENSIONS.y) / 256.0);
            glVertex3f(xbearing,
                       ybearing,
                       0.0);
          glTexCoord2d(ORIGIN.x / 256.0, ORIGIN.y / 256.0);
            glVertex3f(xbearing,
                       ybearing - ysize,
                       0.0);
          glTexCoord2d((ORIGIN.x + DIMENSIONS.x) / 256.0, ORIGIN.y / 256.0);
            glVertex3f(xbearing + xsize,
                       ybearing - ysize,
                       0.0);
          glTexCoord2d((ORIGIN.x + DIMENSIONS.x) / 256.0, (ORIGIN.y + DIMENSIONS.y) / 256.0);
            glVertex3f(xbearing + xsize,
                       ybearing,
                       0.0);
        glEnd();
      }
    }
//  else
//  {
      if (current == '\n' || current == '\r')
      {
        if(inselection)
        {
          glPopMatrix();
          glPushMatrix();
          glColor4f(dis->color[0], dis->color[1], dis->color[2], 1);
          endsel = base;
          endsel.y += base2base;
          endsel.z -= 0.05;

          if(selend > selbegin)
          {
            glDisable(GL_ALPHA_TEST);
            glDisable(GL_TEXTURE_2D);
            glBegin(GL_QUADS);
              glVertex3f(beginsel.x, endsel.y, beginsel.z);
              glVertex3f(beginsel.x, beginsel.y, beginsel.z);
              glVertex3f(endsel.x, beginsel.y, endsel.z);
              glVertex3f(endsel.x, endsel.y, endsel.z);
            glEnd();
            glEnable(GL_TEXTURE_2D);
            glEnable(GL_ALPHA_TEST);
          }

          beginsel.x = 0;
          beginsel.y -= base2base;
          glColor4f(1.0 - dis->color[0], 1.0 - dis->color[1], 1.0 - dis->color[2], 1);
        }

        base.x = 0.0;
        base.y -= base2base;
        if(base.y < -base2base)
          break;

        if(dis->subtype == MULTIBOX || dis->subtype == BOX)
          base.x += 0.05;

        glPopMatrix();
        glPushMatrix();
        glTranslatef(base.x, base.y, base.z);
      }
//    control character processing here;
//  }
    base.x += thing->typeface->metrics[(int)current].xadvance * (thing->ptsize / 24.0);
    glTranslatef(thing->typeface->metrics[(int)current].xadvance * (thing->ptsize / 24.0), 0.0, 0.0);
    label++;
    textat++;
  }
    glPopMatrix();
    glPushMatrix();
    endsel = base;
    endsel.y += base2base;
    endsel.z -= 0.05;

    glDisable(GL_ALPHA_TEST);
    glDisable(GL_TEXTURE_2D);

    glColor4f(dis->color[0], dis->color[1], dis->color[2], 1);

    if(inselection)
    {
      glBegin(GL_QUADS);
        glVertex3f(beginsel.x, endsel.y, beginsel.z);
        glVertex3f(beginsel.x, beginsel.y, beginsel.z);
        glVertex3f(endsel.x, beginsel.y, endsel.z);
        glVertex3f(endsel.x, endsel.y, endsel.z);
      glEnd();
    }
    else if(selend == textat)
    {
      glBegin(GL_LINES);
        glVertex3f(endsel.x, endsel.y - base2base, endsel.z);
        glVertex3f(endsel.x, endsel.y, endsel.z);
      glEnd();
    }

  glPopMatrix();

  glBlendFunc(GL_ONE, GL_ZERO);
  glLightfv(GL_LIGHT0, GL_AMBIENT, saveamb);
//  glDisable(GL_BLEND);

  if(dis->subtype != LABEL)
  {
    glDisable(GL_CLIP_PLANE0);
    glDisable(GL_CLIP_PLANE1);
    glDisable(GL_CLIP_PLANE2);
    glDisable(GL_CLIP_PLANE3);
  }
}

int destroy_text(treenode *dis)
{
  free(dis->prop);
  return 0;
}

treenode *create_text(treenode *parent, GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height,
                      GLfloat rotx, GLfloat roty, GLfloat rotz, int flags, GLfloat pointsize, int subtype,
                      font *face, char *label)
{
   treenode *temp;

   temp = create_child(parent, sizeof(treenode));
   if(!temp)
   {
      return 0;
      exit(1);
   }

   temp->prop = malloc(sizeof(text));

   temp->autoresolve = FALSE;

   temp->visible = TRUE;
   temp->type = TEXT;
   temp->origin.x = x;
   temp->origin.y = y;
   temp->origin.z = z;
   temp->dimentions.x = width;
   temp->dimentions.y = height;
   temp->rotation.x = rotx;
   temp->rotation.y = roty;
   temp->rotation.z = rotz;

   if (color_haq(parent) == 0)
   {
      temp->color[0] = 0;
      temp->color[1] = 0;
      temp->color[2] = 0;
      temp->color[3] = 1;
   }
   else
   {
      temp->color[0] = 1;
      temp->color[1] = 1;
      temp->color[2] = 1;
      temp->color[3] = 1;
   }

   // catch a color that's 2, and don't do autocolor
/* temp->color[0] = temp->parent->color[0] + 0.1;
   temp->color[1] = temp->parent->color[1] + 0.1;
   temp->color[2] = temp->parent->color[2] + 0.1;
   temp->color[3] = temp->parent->color[3]; */

   temp->subtype = subtype; 
   temp->mask = MASK_ALL & ~(MASK_MOUSEDOWN | MASK_MOUSEUP | MASK_COLLIDE | MASK_MOUSEDRAG | MASK_CHANGE);
   temp->event = event_text;
   ((text *) (temp->prop))->label = strdup(label);
   ((text *) (temp->prop))->typeface = face;
   ((text *) (temp->prop))->beginoffset.x = 0.0;
   ((text *) (temp->prop))->beginoffset.y = 0.0;
   ((text *) (temp->prop))->beginoffset.z = 0.0;
   ((text *) (temp->prop))->ptsize = pointsize;

   switch(subtype)
   {
      case LABEL:
      if (create_text_label(temp))
      {
         destroy_node(temp);
         return(0);
      }
      break;

      case BOX:
      if (create_text_box(temp))
      {
         destroy_node(temp);
         return(0);
      }
      break;

      case MULTIBOX:
      if (create_text_multibox(temp))
      {
         destroy_node(temp);
         return(0);
      }
      break;

      default:
         destroy_node((treenode *)temp);
         return(0);
         break; // haha
   }

   make_bbox(temp);
   return temp;
}

void center_text_xy(treenode *thing)
{
// struct text *prp;
// prp = (text *)(thing->prop);

   thing->origin.x = (thing->parent->dimentions.x / 2) - (thing->dimentions.x / 2);
   thing->origin.y = (thing->parent->dimentions.y / 2) - (thing->dimentions.y / 2);
}

void center_text_y(treenode *thing)
{
// struct text *prp;
// prp = (text *)(thing->prop);

   thing->origin.y = (thing->parent->dimentions.y / 2) - (thing->dimentions.y / 2);
   thing->origin.x = thing->origin.y;
}

int create_text_label(treenode *temp)
{
   if(temp->dimentions.x == 0.0)
      temp->dimentions.x = get_text_width(temp);
   if(temp->dimentions.y == 0.0)
   {
      temp->dimentions.y = get_text_height(temp);
      temp->origin.y += temp->dimentions.y;
   }
   return 0;
}

int create_text_box(treenode *temp)
{
   struct text *prp;
   prp = (text *)(temp->prop);
   prp->begin = prp->end = 0;
   temp->mask &= ~(MASK_KEYPRESS);

   return 0;
}

int create_text_multibox(treenode *temp)
{
   struct text *prp;

   prp = (text *)(temp->prop);
   prp->begin = prp->end = 0;
   temp->mask &= ~(MASK_KEYPRESS);

   temp->autoresolve = 1;
   temp->dimentions.x -= 0.25;

// prp->hbar = create_bar(parent, x, y, z, width, 0.25, 0, 0, 0, 0, 100, 0, (width / viewrange_x) * 100, parent->color, BAR, SCROLL, 1);
// ((scrollbarprop *)(prp->hbar))->prop->change_handler = scroll_multiline;
   prp->vbar = create_bar(temp, temp->dimentions.x, 0, 0.1, 0.25, temp->dimentions.y, 0, 0, 0, 0, 1, 0, 1, temp->color, BAR, VSCROLL, 1);
   temp->activenode = NULL;

   adjust_multiline_scrollers(temp);

   make_bbox(temp);

   
   return 0;
}
