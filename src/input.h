#ifndef _INPUT_H_
#define _INPUT_H_

#include <GL/gl.h>
#include "container.h"
#include "tree.h"

extern GLfloat Xpos, Ypos;

typedef struct _new_pointer
{
  treenode nodedef;
  int glyph;
  int locked;
  float dx, dy;
} _new_pointer;

/*
typedef struct {
    point location;
    int state;
    int sel;
    int node;
} pointer;
*/
extern _new_pointer *new_mouse;

int set_grabbed_node(treenode *disone);
treenode * get_grabbed_node(void);
_new_pointer * new_create_pointer(treenode *parent, GLfloat x, GLfloat y, GLfloat z, GLfloat rx, GLfloat ry, GLfloat rz, int glyph);
void new_move_pointer(_new_pointer *pointer, GLfloat x, GLfloat y, GLfloat z);
void new_draw_pointer(_new_pointer *pointer);
void drawselected(GLenum mode);
void clickonstuff(int button, int state, int x, int y);
void specialkey(int key, int x, int y);
void key(unsigned char key, int x, int y);
void mousemove(int x, int y);
void mousedownmove(int x, int y);
void drawpointer();
void initmouse();

#endif
