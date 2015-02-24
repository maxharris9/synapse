#ifndef _BARMANAGER_H_
#define _BARMANAGER_H_

typedef struct {
  treenode *upbutton;
  treenode *downbutton;
  treenode *slider;
  int (*change_handler) (struct treenode *target, struct treenode *node, int event, int modifier, point *where, unsigned long usec);
} scrollbarprop;

int create_progress(treenode *temp);
int create_slider(treenode *temp, int min, int max, int window, int value);
int create_scroll(treenode *temp, int min, int max, int window, int value);
int change_slider_values(treenode *thing, int min, int max, int window, int value);

int resize_scrollbar(treenode *thing, GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height, GLfloat rotx, GLfloat roty, GLfloat rotz);

#endif
