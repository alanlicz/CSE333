// COpyright Alan Li, tianyl28@uw.edu
#ifndef VECTOR_H_
#define VECTOR_H_
class Vector {
 public:
    Vector();  // default contructor with all parameter set to 0
    // constructor with 3 floats as parameters
    Vector(float x, float y, float z);
    Vector(const Vector& v);
    ~Vector();  // destructor

    Vector& operator=(const Vector& v);  // assignment operator

    float get_x() const;  // getter for x component
    float get_y() const;  // getter for y component
    float get_z() const;  // getter for z component

    Vector& operator+=(const Vector& v);  // updating addition operator
    Vector& operator-=(const Vector& v);  // updating subtraction operator

    float operator*(const Vector& v) const;  // dot product operator

 private:
    float x, y, z;  // components of the Vector
    friend void Increment(Vector v);  // friend function
};
#endif  // VECTOR_H_
