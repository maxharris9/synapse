/* input.c -
 * input routines for System 1.0.
 * Copyright(c) 1998 Objective Reality Computer.
 * By Nick McKinney and Max Harris. 
 */

#include <GL/glut.h> // for GLUT_KEY_*
#include <math.h>
#include "button.h" // for MENUBAR, MENUTOP, MENU
#include "init.h" // for Window
#include "event.h" // for send_event()
#include "resources.h" // for RES_POINTER
#include "window.h" // for destroy_window(), minimize_window()
#include "input.h"
#include "display.h" // for maincamera
#include "tree.h" // for showtree
#include "container.h" // for ray_intersect_nodes()
#include "coord.h"
#include <stdio.h> // printf

extern GLuint resources[];
extern int showbboxes;
extern int showcontrols;
extern int killme;

_new_pointer *new_mouse;

int mouse_button = 0;

static GLfloat lastx, lasty;
static treenode *grabbednode;
static point grabbedpoint;

treenode *get_grabbed_node(void)
{
  return grabbednode;
}

int set_grabbed_node(treenode *disone)
{
  grabbednode = disone;
  return 0;
}

treenode * whoisactive(void)
{
  treenode *dar;

  dar = &rootnode;
  while(dar->autoresolve && dar->activenode)
    dar = dar->activenode;

  return dar;
}

void mousedownmove(int x, int y)
{
  point offset;
  double dir[3];
  camera *cam;

  if(x == lastx && y == lasty)
    return;

    if(mouse_button == 1) // just left
    {
      if(grabbednode)
      {
        dir[2] = 0;
        dir[1] = (y - lasty) * (-new_mouse->dy / new_mouse->nodedef.origin.z);
        dir[0] = (x - lastx) * (new_mouse->dx / new_mouse->nodedef.origin.z);
        cam = current_camera();
        offset.z = (float)sqrt((double)(cam->location.x*cam->location.x + cam->location.y*cam->location.y + (cam->location.z - grabbedpoint.z)*(cam->location.z - grabbedpoint.z)));
        dir[0] *= -offset.z;
        dir[1] *= -offset.z;
        rotate_point_forward(dir, cam->rotation.x, cam->rotation.y, cam->rotation.z);
        offset.x = dir[0];
        offset.y = dir[1];
        offset.z = dir[2];
        send_node_event(grabbednode, grabbednode, EVENT_MOUSEDRAG, 0, &offset);
        grabbedpoint.x += offset.x;
        grabbedpoint.y += offset.y;
        grabbedpoint.z += offset.z;
      }
      lastx = x;
      lasty = y;
      mousemove(x, y);
    }
    else if(mouse_button & 4) // middle
    {
      cam = current_camera();
      offset = cam->rotation;
      rotatecamera(cam, (lasty - y) / 28.1, 0, 0);
      rotatecamera(cam, 0, (lastx - x) / 37.5, 0);
      offset.x = (lasty - y) / 28.1;
      offset.y = (lastx - x) / 37.5;
      offset.z = 0;
      if(mouse_button == 5) // middle and left
      {
        dir[0] = grabbedpoint.x - cam->location.x;
        dir[1] = grabbedpoint.y - cam->location.y;
        dir[2] = grabbedpoint.z - cam->location.z;
        rotate_point_backward(dir, offset.x, offset.y, offset.z);
//      send_node_event(grabbednode, grabbednode, EVENT_TWIST, 0, &offset);
        offset.x = (dir[0] + cam->location.x) - grabbedpoint.x;
        offset.y = (dir[1] + cam->location.y) - grabbedpoint.y;
        offset.z = (dir[2] + cam->location.z) - grabbedpoint.z;
        send_node_event(grabbednode, grabbednode, EVENT_MOUSEDRAG, 0, &offset);
        grabbedpoint.x += offset.x;
        grabbedpoint.y += offset.y;
        grabbedpoint.z += offset.z;
      }
//      else
//      lastx = x;
//      lasty = y;
      glutWarpPointer(lastx, lasty);
      // middle and right is invalid
    }
    else if(mouse_button & 2) // right
    {
      cam = current_camera();
      offset = cam->location;
      movecamera(cam, (lasty - y) / 37.5); 
      if(mouse_button == 3) // left and right
      {
        offset.x = cam->location.x - offset.x;
        offset.y = cam->location.y - offset.y;
        offset.z = cam->location.z - offset.z;
        send_node_event(grabbednode, grabbednode, EVENT_MOUSEDRAG, 0, &offset);
//        movecamera(cam, (lasty - y) / -37.5);    // put this in preferences as an option
        grabbedpoint.x += offset.x;
        grabbedpoint.y += offset.y;
        grabbedpoint.z += offset.z;
      }
      glutWarpPointer(lastx, lasty);
//      lasty = y;
    }
}

void mousemove(int x, int y)
{
  new_move_pointer(new_mouse, (GLfloat) (x - WIDTH / 2) * new_mouse->dx, (GLfloat) (y - HEIGHT / 2) * -new_mouse->dy, new_mouse->nodedef.origin.z);
}

void key(unsigned char key, int x, int y)
{
   switch (key)
   {
      case 27:
      {
         if (glutGetModifiers() == (GLUT_ACTIVE_ALT | GLUT_ACTIVE_CTRL | GLUT_ACTIVE_SHIFT))
         {
//            glutDestroyWindow(Window);
//            exit(0);
            killme = 1;
         }
         break;
      }

      case 9:
      {
         if (glutGetModifiers() == GLUT_ACTIVE_ALT)
         {
            cycle_focus(&rootnode, 1);
            break;
         }

         if (glutGetModifiers() == GLUT_ACTIVE_CTRL)
         {
            cycle_focus(&rootnode, 2);
            break;
         }
         else
            cycle_focus(&rootnode, 3);
         break;
      }

      default:
      {
         int mkey, mods;

         mkey = key;
         mods = glutGetModifiers();
         mkey |= (mods & GLUT_ACTIVE_SHIFT) ? (0x00010000) : (0);
         mkey |= (mods & GLUT_ACTIVE_CTRL) ? (0x00020000) : (0);
         mkey |= (mods & GLUT_ACTIVE_ALT) ? (0x00040000) : (0);
         send_node_event(whoisactive(), whoisactive(), EVENT_KEYPRESS, mkey, NULL);
         break;
      }
   }
}

void specialkey(int key, int x, int y)
{
   switch (key)
   {
     case GLUT_KEY_F1:
        showtree();
        break;

     case GLUT_KEY_F2:
        showbboxes = !(showbboxes);
        break;

     case GLUT_KEY_F3:
        showcontrols = !(showcontrols);
        break;

     case GLUT_KEY_F4:
     {
        camera *cam;

        cam = current_camera();
        cam->location.x = cam->location.y = cam->location.z = 0;
        cam->rotation.x = cam->rotation.y = cam->rotation.z = 0;
        positionview(cam);
        break;
     }

     case GLUT_KEY_UP:
        send_node_event(whoisactive(), whoisactive(), EVENT_KEYPRESS, SYN_KEY_UP, NULL);
        break;

     case GLUT_KEY_DOWN:
        send_node_event(whoisactive(), whoisactive(), EVENT_KEYPRESS, SYN_KEY_DOWN, NULL);
        break;

     case GLUT_KEY_LEFT:
        send_node_event(whoisactive(), whoisactive(), EVENT_KEYPRESS, SYN_KEY_LEFT, NULL);
        break;

     case GLUT_KEY_RIGHT:
        send_node_event(whoisactive(), whoisactive(), EVENT_KEYPRESS, SYN_KEY_RIGHT, NULL);
        break;
   }
}

_new_pointer * new_create_pointer(treenode *parent, GLfloat x, GLfloat y, GLfloat z,
                                  GLfloat rx, GLfloat ry, GLfloat rz, int glyph)
{
   _new_pointer *temp;

   if(!parent)
      return 0;

   if(parent->type != CAMERA)
      return 0;

   temp = (_new_pointer *)create_child(parent, sizeof(_new_pointer));
   if(!temp)
      return 0;

   temp->nodedef.type = POINTER;
   temp->nodedef.origin.x = x;
   temp->nodedef.origin.y = y;
   temp->nodedef.origin.z = z;
   temp->nodedef.rotation.x = rx;
   temp->nodedef.rotation.y = ry;
   temp->nodedef.rotation.z = rz;
   temp->nodedef.visible = 1;
   temp->glyph = glyph;
   temp->locked = 0;

   temp->dx = (REALSW * -temp->nodedef.origin.z) / WIDTH;
   temp->dy = (REALSH * -temp->nodedef.origin.z) / HEIGHT;

   return temp;
}

void new_move_pointer(_new_pointer *pointer, GLfloat x, GLfloat y, GLfloat z)
{
   if (pointer->locked == 0)
   {
      pointer->nodedef.origin.x = x;
      pointer->nodedef.origin.y = y;
      pointer->nodedef.origin.z = z;
   }
}

void new_draw_pointer(_new_pointer *pointer)
{
   glDisable(GL_DEPTH_TEST);
   glDisable(GL_LIGHTING);
   glColor3f(0.6, 0.1, 0);
   glCallList(resources[RES_POINTER]);
   glEnable(GL_LIGHTING);
   glEnable(GL_DEPTH_TEST);
}

void clickonstuff(int button, int state, int x, int y)
{
  double orig[3], dir[3], t, u, v;
  treenode *thing = 0;
  treenode *resolv_temp;
  camera *cam;
  point click;

  if(button == GLUT_LEFT_BUTTON)
    mouse_button ^= 1;
  if(button == GLUT_RIGHT_BUTTON)
    mouse_button ^= 2;
  if(button == GLUT_MIDDLE_BUTTON)
    mouse_button ^= 4;

  lastx = x;
  lasty = y;

  if(button == GLUT_LEFT_BUTTON)
  {
  if(state == GLUT_DOWN)
  {
    cam = current_camera();
    orig[0] = cam->location.x;
    orig[1] = cam->location.y;
    orig[2] = cam->location.z;
    dir[0] = new_mouse->nodedef.origin.x;
    dir[1] = new_mouse->nodedef.origin.y;
    dir[2] = new_mouse->nodedef.origin.z;
    rotate_point_forward(dir, cam->rotation.x, cam->rotation.y, cam->rotation.z);

    thing = ray_intersect_nodes(&rootnode, orig, dir, &t, &u, &v);

    if (thing)
    {
      resolv_temp = thing;
      while(resolv_temp && thing->autoresolve)
      {
        resolv_temp = resolve_to_node(thing, &u, &v);
        if(resolv_temp)
          thing = resolv_temp;
      }

      click.x = grabbedpoint.x = u;
      click.y = grabbedpoint.y = v;
      click.z = grabbedpoint.z = 0;  // not quite, but closer than before
      grabbednode = thing;
      resolv_temp = thing;
      while(resolv_temp->type != ROOT)
      {
        parentxy(resolv_temp, &grabbedpoint);
        resolv_temp = resolv_temp->parent;
      }
      send_node_event(thing, thing, EVENT_MOUSEDOWN, 0, &click);
    }
  }
  else if(grabbednode)  // this probably isn't how to do this
  {
    send_node_event(grabbednode, grabbednode, EVENT_MOUSEUP, 0, NULL);
    grabbednode = 0;
  }
  }
  else if(button == GLUT_MIDDLE_BUTTON)
  {
    if(grabbednode)
      send_node_event(grabbednode, grabbednode, EVENT_SETTWIST, 0, &grabbedpoint);
  }
}
