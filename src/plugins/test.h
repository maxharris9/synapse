#include <GL/gl.h>

#ifndef __TEST_H_
#define __TEST_H_

struct create_msg
{
  treenode *parent;
  GLfloat x, y, z;
  GLfloat width, height, depth;
  GLfloat rotx, roty, rotz;
  GLfloat color[4];
  int visible;
  float percent;
};

struct change_msg
{
  treenode *thing;
  float percent;
};

#endif
