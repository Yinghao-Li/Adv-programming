//
// Created by brian on 11/20/18.
//

#include "complex.h"

#include <cmath>

using namespace std;

const float PI = 3.14159265358979f;

Complex::Complex() : real(0.0f), imag(0.0f) {}

Complex::Complex(float r) : real(r), imag(0.0f) {}

Complex::Complex(float r, float i) : real(r), imag(i) {}

Complex Complex::operator+(const Complex &b) const {
    Complex temp(real + b.real, imag + b.imag);
    return temp;
}

Complex Complex::operator-(const Complex &b) const {
    Complex temp(real - b.real, imag - b.imag);
    return temp;
}

Complex Complex::operator*(const Complex &b) const {
    Complex temp(real * b.real - imag * b.imag, real * b.imag + imag * b.real);
    return temp;
}

Complex Complex::mag() const {
    Complex temp(sqrt(real * real + imag * imag));
    return temp;
}

Complex Complex::angle() const {
    Complex temp(atan2(imag, real) * 360 / (2 * PI));
    return temp;
}

Complex Complex::conj() const {
    Complex temp(real, -imag);
    return temp;
}

std::ostream& operator << (std::ostream& os, const Complex& rhs) {
    Complex c(rhs);
    if(fabsf(rhs.imag) < 1e-10) c.imag = 0.0f;
    if(fabsf(rhs.real) < 1e-10) c.real = 0.0f;

    if(c.imag == 0) {
        os << c.real;
    }
    else {
        os << "(" << c.real << "," << c.imag << ")";
    }
    return os;
}

