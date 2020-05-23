#include "mathematics/numerics.hpp"
#include <iostream>
#include <stdlib.h>
#define frand() ((1.0 / (RAND_MAX + 1.0)) * rand())

void rand_int_matrix(Mat &M, int min, int max)
{
    for (int i = 0; i < M.n; i++) {
        for (int j = 0; j < M.m; j++) {
            M[i][j] = min + rand()%(max-min+1);
        }
    }
}
void rand_int_vec(Vec &v, int min, int max)
{
    for (int i = 0; i < v.n; i++) v[i] = min + rand()%(max-min+1);
}
void rand_float_vec(Vec &v, float min, float max)
{
    for (int i = 0; i < v.n; i++) v[i] = min + frand()*(max-min);
}
void rand_float_matrix(Mat &M, float min, float max)
{
    for (int i = 0; i < M.n; i++) {
        for (int j = 0; j < M.m; j++) {
            M[i][j] = min + frand()*(max-min);
        }
    }
}

int main(void)
{
    srand(time(NULL));

    int n = 12;

    Mat A(n, n);
    //rand_int_matrix(A, 0, 8);
    rand_float_matrix(A, 0, 8);
    std::cout << "A = " << A;
    Vec b(n);
    //rand_int_vec(b, 0, 8);
    rand_float_vec(b, 0, 8);
    std::cout << "b = " << b;

    std::cout << "Solving Ax = b\n";
    Vec x = Ax_b_solve(A, b, true);
    std::cout << "x = " << x;
    std::cout << "Ax = " << A(x);

}


