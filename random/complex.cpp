#include <iostream>
#include <vector>

class complex {
    double re, im;
public:
    complex(double r, double i) : re{r}, im{i} {}
    complex(double r) : re{r}, im{0} {}
    complex() : re{0}, im{0} {}

    inline double real() const { return re; }
    inline double imag() const { return im; }

    inline void real(double d) { re = d; }
    inline void imag(double d) { im = d; }

    inline complex inverse() const {
        double dinv = 1.0 / (re*re + im*im);
        return complex(re * dinv, -im * dinv);
    };
    
    complex& operator+=(complex z) {
        re += z.re;
        im += z.im;
        return *this;
    }
    complex& operator-=(complex z) {
        re -= z.re;
        im -= z.im;
        return *this;
    }
    complex& operator*=(complex z) {
        double rep = re*z.re - im*z.im;
        double imp = re*z.im + im*z.re;
        re = rep;
        im = imp;
        return *this;
    }
    complex& operator/=(complex z) {
        *this *= z.inverse();
        return *this;
    }
};
complex operator * (complex a, complex b) {
    return complex(a.real()*b.real() - a.imag()*b.imag(), a.real()*b.imag() + a.imag()*b.real());
}
complex operator / (complex a, complex b) {
    return a * b.inverse();
}


std::ostream& operator<<(std::ostream &os, const complex &c) {
    os << "(" << c.real() << ", " << c.imag() << ")";
    return os;
}

int main(void)
{
    complex z(5,-2.5);
    std::vector<complex> nums {
        complex(1,2),
        complex(3,4),
        complex(2,-3.5)
    };
    for (const complex &c : nums) {
        std::cout << c << ", ";
    }
    std::cout << "\n";
    for (complex &c : nums) {
        std::cout << c * z << ", ";
    }
    std::cout << "\n";

    for (complex &c : nums) {
        // operator/ is an actual family of functions, which ./. is equivalent to.
        // std::cout << z * operator/(c, z) << ", ";
        std::cout << z * (c / z) << ", ";
    }
    std::cout << "\n";
}
