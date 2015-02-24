/* example plugin */

#include "../plugin.h"
#include "../event.h"
#include "../coord.h"
#include <stdlib.h>
#include <GL/gl.h>
#include <stdio.h>
#include <stdarg.h>
#include <dlfcn.h>

#include "test.h"

static int nodestart;

int plugin_init(void)
{
  return spa_init(1, 1, "ORC_Flapper", &nodestart);
}

int flapper_event(treenode *target, treenode *node, int event, int modifier, point *where, unsigned long usec)
{
  return 1;
}

int plugin_message(msg_header *header, void *msgdata)
{
  switch(header->id)
  {
    case 0:
    {
      struct create_msg *create;
      treenode *temp;

      create = (struct create_msg *)(msgdata);
      temp = create_child(create->parent, sizeof(treenode));
      if(!temp)
      {
        return 0;
        exit(1);
      }

      temp->visible = create->visible;
      temp->origin.x = create->x;
      temp->origin.y = create->y;
      temp->origin.z = create->z;
      temp->dimentions.x = create->width;
      temp->dimentions.y = create->height;
      temp->dimentions.z = create->depth;
      temp->rotation.x = create->rotx;
      temp->rotation.y = create->roty;
      temp->rotation.z = create->rotz;

      temp->mask = MASK_ALL;
      temp->event = flapper_event;

      if (create->color[0] == 2)
      {
        temp->color[0] = create->parent->color[0] + 0.1;
        temp->color[1] = create->parent->color[1] + 0.1;
        temp->color[2] = create->parent->color[2] + 0.1;
        temp->color[3] = create->parent->color[3];
      }
      else
      {
        temp->color[0] = create->color[0];
        temp->color[1] = create->color[1];
        temp->color[2] = create->color[2];
        temp->color[3] = create->color[3];
      }

      temp->type = nodestart;
      temp->subtype = 0;

      temp->prop = malloc(sizeof(float));
      if(create->percent < 0.0)
        create->percent = 0.0;
      if(create->percent > 100.0)
        create->percent = 100.0;
      *((float *)(temp->prop)) = create->percent;

      make_bbox(temp);
    }

  }
  return -1;
}

void plugin_draw(treenode *object, GLfloat distance)
{
  glBegin(GL_TRIANGLES);
    glVertex3f(0, 0, -5);
    glVertex3f(0, -1, -5);
    glVertex3f(1, -1, -5);
  glEnd();
  return;
}

void plugin_cleanup(void)
{
  return;
}
