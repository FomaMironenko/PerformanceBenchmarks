#include "matrix.h"

Matrix multiply(const Matrix& a, const Matrix& b)
{
    int n = a.n;

    const std::vector<int>& a_dat = a.data;
    std::vector<int> b_t_dat(n * n);
    std::vector<int> res(n * n);

    // Transpose in order to prevent multiple cache misses
    for (int row = 0; row < n; ++row) {
        for (int col = 0; col < n; ++col) {
            b_t_dat[col * n + row] = b.data[row * n + col];
        }
    }

    int val = 0;
    int a_row = 0, b_t_row = 0;
    for (int row = 0; row < n; ++row) {
        b_t_row = 0;
        for (int col = 0; col < n; ++col) {
            val = 0;
            for (int k = 0; k < n; ++k) {
                val += a_dat[a_row + k] * b_t_dat[b_t_row + k];
            }
            res[a_row + col] = val;
            b_t_row += n;
        }
        a_row += n;
    }
    return Matrix{n, std::move(res)};
}
