// Copyright Alan Li, tianyl28@uw.edu
#include "Vector.h"

Vector::Vector() : x(0), y(0), z(0) {}

Vector::Vector(float x, float y, float z) : x(x), y(y), z(z) {}

Vector::Vector(const Vector& v) : x(v.x), y(v.y), z(v.z) {}

Vector::~Vector() {}

float Vector::get_x() const { return x; }

float Vector::get_y() const { return y; }

float Vector::get_z() const { return z; }

Vector& Vector::operator=(const Vector& v) {
    x = v.x;
    y = v.y;
    z = v.z;
    return *this;
}

Vector& Vector::operator+=(const Vector& v) {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

Vector& Vector::operator-=(const Vector& v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

float Vector::operator*(const Vector& v) const {
    return x * v.x + y * v.y + z * v.z;
}
