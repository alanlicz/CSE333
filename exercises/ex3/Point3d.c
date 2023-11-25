#include "Point3d.h"
#include <stdlib.h>

Point3d* Point3d_Allocate(int32_t x, int32_t y, int32_t z) {
    Point3d* point_ptr = (Point3d*) malloc(sizeof(Point3d));

    point_ptr->x = x;
    point_ptr->y = y;
    point_ptr->z = z;

    return point_ptr;
}

void Point3d_Scale(Point3d* point_ptr, int32_t multiplier) {
    point_ptr->x *= multiplier;
    point_ptr->y *= multiplier;
    point_ptr->z *= multiplier; 
}

Point3d Point3d_GetOrigin() {
    Point3d point;
    point.x = 0;
    point.y = 0;
    point.z = 0;

    return point;
}