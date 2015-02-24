#include <GL/gl.h>
#include <GL/glut.h>
#include <stdlib.h> // for malloc
#include <stdio.h> // for printf
#include "tree.h" // for treenode
#include "coord.h" // for bounding boxes
#include "container.h" // for drawtree
#include "bar.h" // for create_bar
#include "draw.h" // for new_border
#include "barmanager.h"
#include "reigon.h"

typedef struct group
{
   treenode *label;
} group;

int group_event(struct treenode *target, struct treenode *node, int event, int modifier, point *where, unsigned long usec);
int resize_group(treenode *target, point *size);
treenode *create_group(treenode *parent, GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height, GLfloat rotx, GLfloat roty, GLfloat rotz, char *label, int visible);
int group_event(struct treenode *target, struct treenode *node, int event, int modifier, point *where, unsigned long usec);
void draw_group(treenode *this);
