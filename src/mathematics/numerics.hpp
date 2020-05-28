#ifndef MATHEMATICS_NUMERICS_H
#define MATHEMATICS_NUMERICS_H
#include "core.hpp"

struct Vec {
    // n x 1 column vector.
    int n;
    float *vals;

    float &operator[] (int index) const {
        return vals[index];
    }

    Vec(int _n) : n{_n} {
        vals = new float[n];
        for (int i = 0; i < n; i++) vals[i] = 0;
    }
    Vec(const Vec &copied) {
        n = copied.n;
        vals = new float[n];
        memcpy(vals, copied.vals, sizeof(float)*n);
    }
};
std::ostream &operator<<(std::ostream &os, const Vec &v);

struct Mat {
    // m x n matrix.
    int m; // num rows
    int n; // num columns
    float **vals;

    inline float entry(int index_i, int index_j) {
        return (*this)[index_i][index_j];
    }

    Vec left_multiply_vector(const Vec &v); // Computes Mv.

    Vec operator()(const Vec &v) {
        return left_multiply_vector(v);
    }

    float * &operator[](int index) const {
        return vals[index];
    }

    Mat(int _m, int _n) : m{_m}, n{_n} {
        size_t row_pointers_size = sizeof(float *) * m;
        size_t row_size = sizeof(float) * n;
        char *data = new char[row_pointers_size + row_size * m];
        vals = (float **) data;
        for (int i = 0; i < m; i++) {
            vals[i] = (float *) (data + row_pointers_size + i * row_size);
        }
        // Zero-initialize the matrix.
        memset(vals + row_pointers_size, 0, row_size * m);
    }
    Mat(Mat &copied) : Mat(copied.m, copied.n) {
        memcpy(vals[0], copied.vals[0], sizeof(float)*m*n);
    }
};
std::ostream &operator<<(std::ostream &os, const Mat &M);

Vec Ax_b_solve(Mat &_A, Vec &_b, bool trace = false);

#endif // MATHEMATICS_NUMERICS_H
