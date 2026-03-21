#include "include/qr.hpp"


int main() {

    auto grid = QR::make(1);
    int cycle = 0;

    for (auto [x,y] : grid_pos(grid)) {
        grid[y][x] = cycle++ % 8;
    }


    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid.size(); j++) {
            printf("%2i ", grid[i][j]);
        }

        printf("\n");
    }
}
