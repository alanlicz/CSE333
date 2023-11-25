// Copyright Alan Li, tianyl28@uw.edu

#include <iostream>
#include "Vector.h"

// Function to test pass-by-value or pass-by-reference
void Increment(Vector v) {
    v.x += 1.0f;
    v.y += 1.0f;
    v.z += 1.0f;
}

// Function to verify addresses
bool VerifyAddress(const Vector& v, void* addr) {
    // We are not modifying v
    return static_cast<void*>(const_cast<Vector*>(&v)) == addr;
}

int main() {
    // Create a Vector object for testing
    Vector v1(1, 2, 3);
    std::cout << "Original Vector: " << v1.get_x() << ", " << v1.get_y() << ", "
    << v1.get_z() << std::endl;

    // Call Increment() to test pass-by-value or pass-by-reference
    Increment(v1);
    std::cout << "After Increment(): " << v1.get_x() << ", " << v1.get_y()
    << ", " << v1.get_z() << std::endl;

    // Call VerifyAddress() to test address verification
    void* addr = static_cast<void*>(&v1);
    bool sameAddr = VerifyAddress(v1, addr);
    std::cout << "VerifyAddress(): ";
    if (sameAddr) {
        std::cout << "Ref: same address" << std::endl;
    } else {
        std::cout << "Ref: different address" << std::endl;
    }

    return 0;
}
