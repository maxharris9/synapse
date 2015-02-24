#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "synapse.h"
#include "tree.h"

typedef struct camera {
   treenode nodedef;
   point location;
   point rotation;
   GLfloat velocity;
   point mouseratio;
} camera;

void display(void);
void reshape(int width, int height);
camera *new_create_camera(treenode *parent, GLfloat x, GLfloat y, GLfloat z,
                         GLfloat rx, GLfloat ry, GLfloat rz, GLfloat v);
camera * current_camera(void);
camera * set_current_camera(camera *curr);
void movecamera(camera *cam, GLfloat amt);
void rotatecamera(camera *cam, GLfloat dx, GLfloat dy, GLfloat dz);
void positionview(camera *cam);

void draw_generic_icon(void);
void draw_folder(void);

// when you change FOV, you need to recalculate the others
// each is equal to the tangent of half of the fov (horizontal or vertical)
// times 2
#define FOV 30.0  // vertical FOV
#define REALSH 0.53589838  // defined by lim(z->0 of x/z)
#define REALSW 0.72794047  // defined by lim(z->0 of y/z)

#endif
