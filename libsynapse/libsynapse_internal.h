#include <GL/gl.h>

#ifndef _LIBSYN_INTERNAL_H_
#define _LIBSYN_INTERNAL_H_

typedef struct {
  unsigned int len;
  unsigned int type;
} header;

typedef struct {
  GLfloat x, y, z;
} Point;

static inline int glfloat_to_int(GLfloat f)
{
  int i = 0;
  GLfloat *fp;
  int *ip;

  fp = &f;
  ip = &i;
  *ip = *((int *)fp);
  return i;
}

static inline GLfloat int_to_glfloat(int i)
{
  GLfloat f = 0.0;
  GLfloat *fp;
  int *ip;

  fp = &f;
  ip = &i;
  *fp = *((float *)ip);
  return f;
}

extern int LS_socket;

#endif
