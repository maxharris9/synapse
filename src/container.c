/* container.c -
 * container routines for Synapse
 * Copyright(c) 1998-98 Objective Reality Computer.
 * By Nick McKinney and Max Harris. 
 */

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "button.h" // for MENUBAR
#include "window.h" // for draw_window()
#include "deepwindow.h" // for draw_deepwindow()
#include "menu.h" // for draw_menu()
#include "tree.h" // for struct treenode
#include "text.h" // for draw_text()
#include "display.h" // for camera
#include "input.h"  // for pointer
#include "nodetypes.h"  // all object types
#include "raytri.h"  // for intersect_triangle()
#include "coord.h" // for rotate_tri;
#include "glyph.h" // for glyphs
#include "bar.h" // for bar
#include "draw.h" // for border()
#include "coord.h" // for draw_bbox()
#include "reigon.h" // for draw_reigon()
#include "group.h" // for draw_group()
#include "list.h" // for draw_list()
#include "plugin.h"

treenode rootnode;
extern GLuint texname;
int showbboxes = 0;
int showcontrols = 1;

static point clickpoint;

void initnodes()
{
   rootnode.next = rootnode.prev = 0;
   rootnode.child = rootnode.parent = 0;
   rootnode.type = -1;
   clickpoint.x = clickpoint.y = clickpoint.z = 0.0;
}

void drawtree(treenode *parent)
{
  treenode *this;
  int clipout;

  this = parent->child;
  while(this)
  {
    clipout = 0;
    if(this->visible && this->type != ROOT)
    {
      if(showbboxes && !(this->type == APP || this->type == CAMERA || this->type == POINTER))
        draw_bbox(this);

      if(this->parent->type == REIGON)
      {
        if(this->bbox[0] > ((reigon *)(this->parent->prop))->viewport.x + ((reigon *)(this->parent->prop))->viewsize.x)
          clipout = 1;
        if(this->bbox[1] > ((reigon *)(this->parent->prop))->viewport.y + ((reigon *)(this->parent->prop))->viewsize.y)
          clipout = 1;
        if(this->bbox[6] < ((reigon *)(this->parent->prop))->viewport.x)
          clipout = 1;
        if(this->bbox[7] < ((reigon *)(this->parent->prop))->viewport.y)
          clipout = 1;
      }

      glPushMatrix();
      if(this->origin.x != 0.0 || this->origin.y != 0.0 || this->origin.z != 0.0)
        glTranslatef(this->origin.x, this->origin.y, this->origin.z);
      if(this->rotation.x != 0.0)
        glRotatef(this->rotation.x, 1, 0, 0);
      if(this->rotation.y != 0.0)
        glRotatef(this->rotation.y, 0, 1, 0);
      if(this->rotation.z != 0.0)
        glRotatef(this->rotation.z, 0, 0, 1);

      glColor4fv(this->color);

    if((showcontrols && !clipout) || (this->type == CAMERA || this->type == POINTER))
    {
    if(this->type >= FIRST_NODE)
    {
      if(!this->plugin)
        this->plugin = find_plugin_by_nodetype(this->type);
      // send_plugin_draw((plugin_module *)(this->plugin->prop), this);
    }
    else
    switch(this->type)
    {
      case FLATWINDOW:
        draw_window(this);
        break;

      case DEEPWINDOW:
        draw_deepwindow(this);
        break;

      case CAMERA:
        if(this->child)
        {
          glPushMatrix();
          glTranslatef((((camera *)this)->location.x),  // these are in the camera-specific attributes
                       (((camera *)this)->location.y),  // the standard attributes should always be zero
                       (((camera *)this)->location.z));
          glRotatef(((camera *)this)->rotation.z, 0, 0, 1);
          glRotatef(((camera *)this)->rotation.y, 0, 1, 0);  // hence the differences in transformation
          glRotatef(((camera *)this)->rotation.x, 1, 0, 0);  // cameras are different animals,
          drawtree(this);
          glPopMatrix();
        }
        break;

      case POINTER:
        new_draw_pointer((_new_pointer *)this);
        break;

      case BUTTON:
        draw_button(this);
        break;

        case TEXT:
          draw_text(this);
          break;

        case BAR:
          draw_bar(this);
          break;

       case MENU:
          draw_menu(this);
          break;

        case APP: // ignore
         break;

        case GLYPH:
          draw_glyph(this);
          break;

        case REIGON:
          draw_reigon(this);
          break;

        case GROUP:
           draw_group(this);
           break;

        case LIST:
           draw_list(this);
           break;

        default:
//        printf("unrecognized node type %d (%X)\n", this->type, this->subtype);
          break;
      }
      }
      if(this->child && this->type != CAMERA && this->type != REIGON && !clipout)
        drawtree(this);
      glPopMatrix();
    } 
    this = this->next;
  }
}

treenode * ray_intersect_nodes(treenode *top, double orig[3], double dir[3], double *t, double *u, double *v)
{
  double closestdist = 0, x = 0, y = 0;
  treenode *closestnode = 0;
  treenode *thing;
  treenode *resolv_temp;
  point click;

  thing = top->child;
  while(thing)
  {
    if(thing->type == POINTER || thing->type == CAMERA || !thing->visible)
    {
      thing = thing->next;
      continue;
    }

    if(thing->type == APP)
    {
      double tt, tu, tv;
      treenode *dar;
      if((dar = ray_intersect_nodes(thing, orig, dir, &tt, &tu, &tv)))
      {
        click.x = tu;
        click.y = tv;
        click.z = tt;
        if(!closestnode || click.z > closestdist)
        {
          closestnode = dar;
          closestdist = click.z;
          x = click.x;
          y = click.y;
        }
      }
    }
    else if(ray_intersect_node(thing, orig, dir, t, u, v))
    {
      click.x = *u;
      click.y = *v;
      click.z = *t;
      resolv_temp = thing;
      while(resolv_temp->type != ROOT)
      { 
        parentxy(resolv_temp, &click);
        resolv_temp = resolv_temp->parent;
      }

      if(!closestnode || click.z > closestdist)
      {
        closestnode = thing;
        closestdist = click.z;
        x = *u;
        y = *v;
      }
    }

    thing = thing->next;
  }
  *t = closestdist;
  *u = x;
  *v = y;
  if(closestnode)
  {
    clickpoint.x = x + closestnode->origin.x;
    clickpoint.y = y + closestnode->origin.y;
  }
  clickpoint.z = closestdist;
  return closestnode;
}

int ray_intersect_node(treenode *thing, double orig[3], double dir[3], double *t, double *u, double *v)
{
  if (!intersect_triangle(orig, dir, &(thing->bbox[18]), &(thing->bbox[21]), &(thing->bbox[15]), t, u, v))  // front face
  {
    if (!intersect_triangle(orig, dir, &(thing->bbox[12]), &(thing->bbox[15]), &(thing->bbox[21]), t, u, v))  // front face
    {
      if (!intersect_triangle(orig, dir, &(thing->bbox[6]), &(thing->bbox[9]), &(thing->bbox[3]), t, u, v))  // back face
      {
        if (!intersect_triangle(orig, dir, &(thing->bbox[0]), &(thing->bbox[3]), &(thing->bbox[9]), t, u, v))  // back face
        {
          return 0; // no collission
        }
        *u = (*u * thing->b_dimentions.x) + (thing->b_origin.x - thing->origin.x); // back face
        *v = (*v * thing->b_dimentions.y) + (thing->b_origin.y - thing->origin.y);
        *t = (thing->b_origin.z - thing->origin.z);
        return 1;
      }
      *u = thing->b_dimentions.x - (*u * thing->b_dimentions.x) + (thing->b_origin.x - thing->origin.x); // back face
      *v = thing->b_dimentions.y - (*v * thing->b_dimentions.y) + (thing->b_origin.y - thing->origin.y);
      *t = (thing->b_origin.z - thing->origin.z);
      return 1;
    }
    *u = (*u * thing->b_dimentions.x) + (thing->b_origin.x - thing->origin.x); // front face
    *v = (*v * thing->b_dimentions.y) + (thing->b_origin.y - thing->origin.y);
    *t = (thing->b_origin.z - thing->origin.z) + thing->b_dimentions.z;
    return 1;
  }
  *u = thing->b_dimentions.x - (*u * thing->b_dimentions.x) + (thing->b_origin.x - thing->origin.x); // front face
  *v = thing->b_dimentions.y - (*v * thing->b_dimentions.y) + (thing->b_origin.y - thing->origin.y);
  *t = (thing->b_origin.z - thing->origin.z) + thing->b_dimentions.z;
  return 1;
}

// this does a bounding box test only.  no depth testing is done, and no
// test for overlapping controls is done
treenode * resolve_to_node(treenode *top, double *x, double *y)
{
  treenode *thing;
  point pt;

  thing = top->child;
  while(thing)
  {
    if(*y > thing->b_origin.y)
    {
      if(*y < thing->b_origin.y + thing->b_dimentions.y)
      {
        if(*x > thing->b_origin.x)
        {
          if(*x < thing->b_origin.x + thing->b_dimentions.x)
          {
            pt.x = *x;
            pt.y = *y;
            pt.z = 0;
            childxy(thing, &pt);
            if(send_node_event(thing, thing, EVENT_COLLIDE, 0, &pt))
            {
              *x = pt.x;
              *y = pt.y;
              return thing;
            }
          }
        }
      }
    }

    thing = thing->next;
  }
  return 0;
}
