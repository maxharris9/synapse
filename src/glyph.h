#ifndef _GLYPH_H_
#define _GLYPH_H_

#include "synapse.h"
#include "tree.h"

int set_glyph_lock(treenode *dis, int locked);
void draw_glyph(treenode *dis);
treenode *create_glyph(treenode *parent, GLfloat x, GLfloat y, GLfloat z,
                       GLfloat width, GLfloat height, GLfloat rotx, GLfloat roty, GLfloat rotz,
                       int flags, GLfloat *color, int dlist, int visible);

#endif
