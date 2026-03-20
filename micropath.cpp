#include <iostream>
#include <vector>

using namespace std;


void finder( std::vector<std::vector<int>> &grid, int x, int y) {
    for (int i = -3; i <= 3; i++) {
        for (int j = -3; j <= 3; j++) {
            const int dist = std::max(std::abs(i), std::abs(j)); // Chebyshev distance
            grid[i + y][j + x] = (dist != 2);
        }
    }
}
std::vector<std::vector<int>> make (int version) { // M1, M2, M3, M4
    const int size = 2 * version + 11;
    std::vector<std::vector<int>> grid(size, std::vector<int>(size, 2));
    finder(grid, 3, 3);

    for (int i = 0; i < 8; i++) {
        grid[7][i] = grid[i][7] = 0; // quiet zone
        grid[8][i + 1] = grid[i + 1][8] = 0; // format information zone
    }

    for (int i = 8; i < size; i++) { // timing patttern
        grid[0][i] = grid[i][0] = (i % 2 == 0);
    }

    return grid;
}
std::vector<std::pair<int,int>> grid_pos (const std::vector<std::vector<int>> &grid, bool micro = false) {
    const int size = grid.size();
    int index = size - 1, cycle = 0;
    std::vector<std::pair<int,int>> zone;

    while (index > 0) {
        if (!micro && index == 6) index = 5;

        for (int i = 0; i < size; i++) {
            int y = cycle % 2 ? i : (size - 1 - i);

            for (int x = index; x > (index-2); x--) {
                if (grid[y][x] == 2) {
                    zone.push_back({x,y});
                }
            }
        }
        index -= 2, cycle++;
    }

    return zone;
}


int main () {

    int i = 0;
    auto grid = make(0);

    for (auto &[x,y] : grid_pos(grid, true)) {
        grid[y][x] = i++  ;
    }
    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid.size(); j++) {
            printf("%2i ", grid[i][j]);
        }

        printf("\n");
    }

}
