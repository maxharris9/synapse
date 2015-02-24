#ifndef _WMANAGER_H_
#define _WMANAGER_H_

#include "tree.h"

int event_wmanager(treenode *owner, treenode *object, int event, int dsize, void *data);

int create_regular(treenode *temp, char *title);
int create_dialog(treenode *temp, char *title);
int create_mini(treenode *temp, char *title);

void resize_window_regular(treenode *dis);
void resize_window_dialog(treenode *dis);
void resize_window_mini(treenode *dis);

typedef struct regwinprop {
  treenode *titlebar;
  treenode *closebox;
  treenode *zoombox;
  treenode *tborder;
  treenode *bborder;
  treenode *lborder;
  treenode *rborder;
  treenode *ltcorner;
  treenode *trcorner;
  treenode *blcorner;
  treenode *brcorner;
  int destroying;
} regwinprop;

typedef struct miniwinprop {
  treenode *titlebar;
  treenode *closebox;
  treenode *zoombox;
  treenode *tborder;
  treenode *bborder;
  treenode *lborder;
  treenode *rborder;
  treenode *ltcorner;
  treenode *rtcorner;
  treenode *blcorner;
  treenode *brcorner;
  int destroying;
} miniwinprop;

typedef struct dialogprop {
  treenode *titlebar;
} dialogprop;

#endif
