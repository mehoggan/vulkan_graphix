#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>
#include <stdio.h>

class Vector {
public:
    Vector();
    Vector(float new_compo_x, float new_compo_y, float new_compo_z);
    ~Vector();
    float compo_x;
    float compo_y;
    float compo_z;
};

#endif /*	VECTOR_H	*/