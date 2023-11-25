#ifndef POINT3D_H
#define POINT3D_H
#include <stdint.h>

typedef struct point3d_st {
   int32_t x;
   int32_t y;
   int32_t z;
} Point3d;

Point3d* Point3d_Allocate(int32_t x, int32_t y, int32_t z);
void Point3d_Scale(Point3d* point_ptr, int32_t multiplier);
Point3d Point3d_GetOrigin();

#endif