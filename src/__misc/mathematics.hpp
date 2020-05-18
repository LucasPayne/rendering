#ifndef MATHEMATICS_H
#define MATHEMATICS_H
#include <math.h>

/*--------------------------------------------------------------------------------
    vec3
    Vector with three float components.
--------------------------------------------------------------------------------*/
class vec3 {
private:
    float vals[3];
public:
    vec3() {}
    vec3(float _x, float _y, float _z) {
        vals[0] = _x;
        vals[1] = _y;
        vals[2] = _z;
    }
    inline float x() const { return vals[0]; }
    inline float y() const { return vals[1]; }
    inline float z() const { return vals[2]; }
    inline float operator[](int index) const { return vals[index]; }

    inline float square_length() const {
        return x()*x() + y()*y() + z()*z();
    }
    inline float length() const {
        return sqrt(square_length());
    }
    // v.dot(u) == dot(v, u).
    inline float dot(const vec3 &other_v) const {
        return x()*other_v.x() + y()*other_v.y() + z()*other_v.z();
    }
    // v.cross(u) == cross(v, u).
    inline vec3 cross(const vec3 &other_v) const {
        return vec3(y()*other_v.z() - z()*other_v.y(),
                    z()*other_v.x() - x()*other_v.z(),
                    x()*other_v.y() - y()*other_v.x());
    }
    inline vec3 normalize() {
        float dinv = 1.0 / length();
        vals[0] *= dinv;
        vals[1] *= dinv;
        vals[2] *= dinv;
    }
    inline vec3 normalized() const {
        float dinv = 1.0 / length();
        return vec3(x()*dinv, y()*dinv, z()*dinv);
    }
    // Unary operations.
    inline vec3 operator-() const {
        return vec3(-x(), -y(), -z());
    }
};

// Vector-vector operations.
inline float dot(const vec3 &a, const vec3 &b) {
    return a.x()*b.x() + a.y()*b.y() + a.z()*b.z();
}
inline vec3 cross(const vec3 &a, const vec3 &b) {
    return vec3(a.y()*b.z() - a.z()*b.y(),
                a.z()*b.x() - a.x()*b.z(),
                a.x()*b.y() - a.y()*b.x());
}
// Scalar-vector operations.
inline vec3 operator*(const vec3 &v, float t) {
    return vec3(v.x() * t, v.y() * t, v.z() * t);
}
inline vec3 operator*(float t, const vec3 &v) {
    return vec3(v.x() * t, v.y() * t, v.z() * t);
}
// Component-wise vector operations.
inline vec3 operator*(const vec3 &a, const vec3 &b) {
    return vec3(a.x()*b.x(), a.y()*b.y(), a.z()*b.z());
}
inline vec3 operator+(const vec3 &a, const vec3 &b) {
    return vec3(a.x()+b.x(), a.y()+b.y(), a.z()+b.z());
}
inline vec3 operator-(const vec3 &a, const vec3 &b) {
    return vec3(a.x()-b.x(), a.y()-b.y(), a.z()-b.z());
}
inline vec3 operator/(const vec3 &a, const vec3 &b) {
    return vec3(a.x()/b.x(), a.y()/b.y(), a.z()/b.z());
}

/*--------------------------------------------------------------------------------
    mat4x4
    A 4-by-4 float matrix, defined in column-major order.
--------------------------------------------------------------------------------*/
class mat4x4 {
private:
    float vals[4][4]; // column-major.
public:
    mat4x4() {}
    mat4x4(float m00, float m01, float m02, float m03,
           float m10, float m11, float m12, float m13,
           float m20, float m21, float m22, float m23,
           float m30, float m31, float m32, float m33) {
        vals[0] = m00;
    }
};

#endif // MATHEMATICS_H
