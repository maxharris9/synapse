#ifndef _RAYTRI_H_
#define _RAYTRI_H_

int
intersect_triangle(double orig[3], double dir[3],
                   double vert0[3], double vert1[3], double vert2[3],
                   double *t, double *u, double *v);

#endif
