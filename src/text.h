#ifndef _TEXT_H_
#define _TEXT_H_

#include "synapse.h"
#include "tree.h"
#include "font.h"

typedef struct text
{
   char *label;

   point beginoffset;

   GLfloat ptsize;
   font *typeface;
   treenode *vbar;

   int begin, end;
   point selbegin, selat;
} text;

void adjust_multiline_scrollers(treenode *dis);
int reshape_text(treenode *thing, int which, GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height, GLfloat rotx, GLfloat roty, GLfloat rotz);
int reshape_multibox(treenode *thing, int which, GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height, GLfloat rotx, GLfloat roty, GLfloat rotz);

int labelcat(treenode *thing, unsigned long len, char *addition);
int event_text(struct treenode *target, struct treenode *node, int event, int modifier, point *where, unsigned long usec);
GLfloat get_text_width(treenode *text);
GLfloat get_text_height(treenode *text);
void select_text(treenode *target, point *where);
void select_more_text(treenode *target, point *where);

void move_text(treenode *dis, point *pt);
void draw_text(treenode *dis);
void draw_bitmap_text(treenode *dis, int selbegin, int selend);
void resize_text(treenode *dis, point *pt);
void draw_text_tray(treenode *this);
int destroy_text(treenode *dis);

int create_text_label(treenode *temp);
int create_text_box(treenode *temp);
int create_text_multibox(treenode *temp);
treenode *create_text(treenode *parent, GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height,
                      GLfloat rotx, GLfloat roty, GLfloat rotz, int flags, GLfloat pointsize, int subtype,
                      font *face, char *label);

int change_text_label(treenode *thing, char *label);

void center_text_xy(treenode *text);
void center_text_y(treenode *text);

#endif
