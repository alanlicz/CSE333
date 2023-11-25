//Alan Li tianyl28@uw.edu

#include "Point3d.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    // Test Point3d_Allocate
    Point3d* p1 = Point3d_Allocate(1, 2, 3);
    if (p1 == NULL) {
        printf("Error: Memory allocation failed!\n");
        return 1;
    }
    printf("p1 = (%d, %d, %d)\n", p1->x, p1->y, p1->z);
    free(p1);

    // Test Point3d_Scale
    Point3d p2 = {4, 5, 6};
    Point3d_Scale(&p2, 2);
    printf("p2 = (%d, %d, %d)\n", p2.x, p2.y, p2.z);

    // Test Point3d_GetOrigin
    Point3d origin = Point3d_GetOrigin();
    printf("origin = (%d, %d, %d)\n", origin.x, origin.y, origin.z);

    return 0;
}
