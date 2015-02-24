#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "synapse.h"
#include "container.h"

typedef struct button
{
   int state;
} button;

int get_button_status(treenode *thing);
int set_button_status(treenode *thing, int status);

int change_button_label(treenode *dis, char *label);
int reshape_button(treenode *thing, int which, GLfloat x, GLfloat y, GLfloat z,
                   GLfloat width, GLfloat height,
                   GLfloat rotx, GLfloat roty, GLfloat rotz);

void draw_button(treenode *dis);
int destroy_button(treenode *disone);
treenode *create_button(treenode *parent, GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height,
                        GLfloat rotx, GLfloat roty, GLfloat rotz, GLfloat *color, int type, int subtype, char *label, int visible);
int create_push(treenode *temp, char *label);
int create_check(treenode *temp, char *label);
int create_radio(treenode *temp, char *label);
int create_border(treenode *temp);
int create_corner(treenode *temp);
int create_filler(treenode *temp);
int create_menubutton(treenode *temp, char *label);

void draw_check(void);
void draw_radio(void);
void draw_push(treenode *dis);
void draw_corner(treenode *dis);
void draw_hardcorner(treenode *dis);
void draw_border(treenode *dis);
void draw_filler(treenode *dis);
void draw_menubutton(treenode *dis);

int collide_corner(treenode *dis, point *pt);

void cycle_active_control(void);

#endif
