#include <vector>

struct Matrix
{
    int n;
    std::vector<int> data;

    int get(int i, int j) const {
        return data[i*n + j];
    }
};

Matrix multiply(const Matrix& a, const Matrix& b);