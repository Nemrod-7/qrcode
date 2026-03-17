#include <stdlib.h>
#include <string.h>

enum direction {CLOCKWISE, COUNTER_CLOCKWISE};

void rotate (size_t rows, size_t cols, const int matrix[rows][cols], enum direction direction, int rotated[cols][rows]) {

    memcpy(rotated, matrix, rows * cols * sizeof(int) );

    const int width = rows, height = cols;
  
    if (direction == CLOCKWISE) {

        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                rotated[i][j] = matrix[width - j - 1][i];
            }
        }
    } else {
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                rotated[i][j] = matrix[j][height - i - 1];
            }
        }
    }
}
int main () {

    const int size = 3;
    const int row = 3, col = 3;

    const int mat[3][3] = { {1,2,3}, {4,5,6}, {7,8,9} };


}
