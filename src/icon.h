
#ifndef _ICON_H_
#define _ICON_H_

#include "synapse.h"
#include "container.h"

void make_icon(int i_type, GLfloat x, GLfloat y, int state, GLfloat width, GLfloat height, treenode *parent);
void draw_icon(treenode *this);

#endif
