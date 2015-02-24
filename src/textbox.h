#ifndef _TEXTBOX_H_
#define _TEXTBOX_H_

#include "synapse.h"
#include "container.h"

typedef struct
{
   int type;
   point local;
   int entrynum;
   int startnid;
} textboxprop;

container * make_textbox(point * origin, point * dimentions, int state, int *nid);
void color_textbox(container * this, GLfloat * c);
void destroy_textbox(int menu);
void draw_textbox(container * this);
void move_textbox(container * this, point * pt);
void resize_texbox(container * this, point * pt);

#endif
