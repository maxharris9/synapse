#include <GL/gl.h>
#include "conn.h"

#ifndef _API_H_
#define _API_H_

static inline int floattoint(GLfloat f)
{
  int i = 0;
  GLfloat *fp;
  int *ip;

  fp = &f;
  ip = &i;
  *ip = *((int *)fp);
  return i;
}

static inline GLfloat inttofloat(int i)
{
  GLfloat f = 0.0;
  GLfloat *fp;
  int *ip;

  fp = &f;
  ip = &i;
  *fp = *((GLfloat *)ip);
  return f;
}

int do_api(connection *this, int size, int type, void *message);

#endif
