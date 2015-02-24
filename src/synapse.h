#ifndef _SYNAPSE_H_
#define _SYNAPSE_H_

#include <GL/gl.h>

#define FALSE 0
#define TRUE 1

extern int __synwidth, __synheight;

#define WIDTH __synwidth
#define HEIGHT __synheight

#define BORDERSCALE 0.1
#define DBLBORDERSCALE 0.2
#define HALFBORDERSCALE 0.05

typedef struct
{
   GLfloat x, y, z;
} point;

typedef struct {
    GLint x, y, z;
} ipoint;

typedef struct
{
   point p1, p2, p3, n;
   GLfloat color[4];
} triangle;

#endif
