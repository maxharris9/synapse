#ifndef _COORD_H_
#define _COORD_H_

void grow_node_bbox(treenode *thing, point *origin, point *dimension);
void draw_bbox(treenode *thing);
void make_bbox(treenode *thing);
void rotate_point_forward(double tri[3], double rotx, double roty, double rotz);
void rotate_point_backward(double tri[3], double rotx, double roty, double rotz);
void rotate_tri(double tri[9], double rotx, double roty, double rotz);
void childxy(treenode *child, point *pt);
void parentxy(treenode *child, point *pt);

#endif
