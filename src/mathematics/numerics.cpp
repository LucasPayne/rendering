#include <iostream>
#include "mathematics/numerics.hpp"

Vec Ax_b_solve(Mat &_A, Vec &_b)
{
    #define err(STRING)\
        std::cerr << "Ax_b_solve error: " << ( STRING ) << "\n";
    if (A.n != A.m) err("A must be a square matrix.");
    if (b.n != A.n) err("b must match the dimensions of A.");
    
    // Copy the inputs.
    Mat A(_A);
    Vec b(_b);
    int n = A.n;

    // Start with the first row. If A[0][0] is 0, swap row A[0] with
    // the next row with A[ii][0] != 0. Then, divide the row by A[0][0], so A[0][0] is 1.
    // Next, subtract row A[i][0]*A[0] from rows A[i], i>0, so that the first column of A is 1,0,...,0.
    // Repeat this process for the next rows, considering entry A[1][1], etc.
    //
    // All of these row operations are mirrored in arithmetic on the entries of b.
    for (int i = 0; i < n; i++) {
        if (A[i][i] == 0) {
            // If this entry is zero, find the next row with a non-zero entry in this column.
            int swap_with_row;
            for (int ii = i+1; ii <= n; ii++) {
                if (ii == n) err("System is not solvable.");
                if (A[ii][i] != 0) {
                    swap_with_row = ii;
                    break;
                }
            }
            // Swap the rows.
            for (int j = 0; j < n; j++) {
                float temp = A[i][j];
                A[i][j] = A[swap_with_row][j];
                A[swap_with_row][j] = temp;
            }
            float temp = B[i];
            B[i] = B[swap_with_row];
            B[swap_with_row] = temp;
        }
        // Make this diagonal entry 1.
        float inv_diagonal_entry = 1.0 / A[i][i];
        A[i][i] = 1.0;
        for (int j = 1; j < n; j++) {
            A[i][j] *= inv_diagonal_entry;
        }
        B[i] *= inv_diagonal_entry;
        // Nullify the entries in this column of each of the following rows.
        for (int ii = i + 1; ii < n; ii++) {
            float val = A[ii][i];
            A[ii][i] = 0;
            for (int j = ii + 1; j < n; j++) {
                A[ii][j] -= val * A[i][j];
            }
            b[ii] -= val * b[i];
        }
    }
    //--^ Probably didn't check enough for invertibility!

    // The system should now be known invertible, upper triangular,
    // and with ones on the diagonal. This can be solved with repeated substitution.
    for (int j = n-1; j >= 0; --j) {
        for (int jj = j-1; jj >= 0; --jj) {
            b[j] -= b[jj] * A[j][jj];
        }
    }

    //------ Destroy the copied matrix!
    return b;
}
