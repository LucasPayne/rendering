#ifndef MATHEMATICS_NUMERICS_H
#define MATHEMATICS_NUMERICS_H
#include <string.h>

struct Vec {
    int n;
    float *vals;
    Vec(int _n) : n{_n} {
        vals = new float[n];
        for (int i = 0; i < n; i++) vals[i] = 0;
    }
    Vec(Vec &copied) {
        n = copied.n;
        vals = new float[n];
        memcpy(vals, copied.vals, sizeof(float)*n);
    }
};

struct Mat {
    int n; // num rows
    int m; // num columns
    float **vals;

    Mat(int _n, int _m) : n{_n}, m{_m} {
        size_t row_pointers_size = sizeof(float *) * n;
        size_t row_size = sizeof(float) * m;
        char *data = new char[row_pointers_size + row_size * n];
        vals = (float *) data;
        for (int i = 0; i < n; i++) {
            vals[i] = (float *) (data + row_pointers_size + i * row_size);
        }
        // Zero-initialize the matrix.
        memset(vals + row_pointers_size, 0, row_size * n);
    }
    Mat(Mat &copied) {
        n = copied.n;
        m = copied.m;
        memcpy(vals[0], copied.vals[0], sizeof(float)*n*m);
    }
};

Vec Ax_b_solve(Mat &A, Vec &b);

#endif // MATHEMATICS_NUMERICS_H
