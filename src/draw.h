
#ifndef _DRAW_H_
#define _DRAW_H_

#define SUNKEN_BORDER 10

#include <GL/gl.h>
#include "synapse.h"

void new_border(point *origin, point *dimensions, GLfloat thick, int inside, GLfloat rounded);
// void border(int type, GLfloat y, GLfloat x, int inside);
// void line(GLfloat length);

#endif
