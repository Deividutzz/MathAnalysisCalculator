#include <iostream>
#include <vector>

using namespace std;
using Matrix = vector<vector<long long>>;

class Determinant {
public:
    Determinant() = default;

    long long det(const Matrix& matrix) {
        this->size = matrix.size();
        return determinant(matrix,size);
    }
private:
    //Matrix matrix;
    int size = 0;
    static Matrix getMinor(int excludedColumn, int size, const Matrix& matrix)
    {
        Matrix minor(size - 1, vector<long long>(size - 1));

        int minorColumn = 0;
        for (int column = 0; column < size; ++column)
        {
            if (column == excludedColumn)
                continue;

            int minorRow = 0;
            for (int row = 1; row < size; ++row)
            {
                minor[minorRow++][minorColumn] = matrix[row][column];
            }
            ++minorColumn;
        }

        return minor;
    }
    long long determinant(const Matrix& matrix, int size)
    {
        if (size == 1)
            return matrix[0][0];

        long long detResult = 0;

        for (int column = 0; column < size; ++column)
        {
            const long long multiplier = matrix[0][column];
            const int sign = column % 2 == 0 ? 1 : -1;
            const Matrix minor = getMinor(column, size, matrix);

            detResult += sign * multiplier * determinant(minor, size - 1);
        }

        return detResult;
    }

};

int main()
{
    int n;
    if (!(cin >> n) || n <= 0)
    {
        cerr << "Enter a positive matrix dimension.\n";
        return 1;
    }

    Matrix matrix(static_cast<size_t>(n),
                  vector<long long>(static_cast<size_t>(n)));
    for (int row = 0; row < n; ++row)
    {
        for (int column = 0; column < n; ++column)
        {
            if (!(cin >> matrix[row][column]))
            {
                cerr << "Not enough valid matrix entries.\n";
                return 1;
            }
        }
    }

    Determinant detCalc;

    cout << detCalc.det(matrix);
    return 0;
}
