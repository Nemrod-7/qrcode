#include <vector>

const std::vector<std::vector<int>> placement = {
{}, {},
{6,18}, {6,22}, {6,26}, {6,30}, {6,34},
{6,22,38}, {6,24,42}, {6,26,46}, {6,28,50}, {6,30,54}, {6,32,58}, {6,34,62},
{6,26,46,66}, {6,26,48,70}, {6,26,50,74}, {6,30,54,78}, {6,30,56,82}, {6,30,58,86}, {6,34,62,90},
{6,28,50,72,94}, {6,26,50,74,98}, {6,30,54,78,102}, {6,28,54,80,106}, {6,32,58,84,110}, {6,30,58,86,114}, {6,34,62,90,118},
{6,26,50,74,98,122}, {6,30,54,78,102,126}, {6,26,52,78,104,130}, {6,30,56,82,108,134}, {6,34,60,86,112,138}, {6,30,58,86,114,142}, {6,34,62,90,118,146},
{6,30,54,78,102,126,150}, {6,24,50,76,102,128,154}, {6,28,54,80,106,132,158}, {6,32,58,84,110,136,162}, {6,26,54,82,110,138,166}, {6,30,58,86,114,142,170}
};

const std::vector<int> versinfo = {
  0,0,0,0,0,0,0,31892,34236,39577,42195,48118,51042,55367,
  58893,63784,68472,70749,76311,79154,84390,87683,92361,96236,
  102084,102881,110507,110734,117786,119615,126325,127568,133589,
  136944,141498,145311,150283,152622,158308,161089,167017
};

void finder( std::vector<std::vector<int>>&grid, int x, int y) {
    for (int i = -3; i <= 3; i++) {
        for (int j = -3; j <= 3; j++) {
            const int dist = std::max(std::abs(i), std::abs(j)); // Chebyshev distance
            grid[i + y][j + x] = (dist != 2);
        }
    }
}
void align ( std::vector<std::vector<int>>&grid, int x, int y) {
    for (int i = -2; i <= 2; i++) {
        for (int j = -2; j <= 2; j++) {
            const int dist = std::max(std::abs(i), std::abs(j)); // Chebyshev distance
            grid[i + y][j + x] = (dist != 1);
        }
    }
}
std::vector<std::vector<int>> mk_grid (int version) {
    const int size = 17 + version * 4;
    std::vector<std::vector<int>> grid(size, std::vector<int>(size, 2));

    for (int i = 0; i < 8; i++) {
        grid[i][8] = grid[8][i] = 0; // informations for the first eye
        grid[size - i - 1][8] = 0;   // informations for the second eye
        grid[8][size - i - 1] = 0;   // informations for the third eye

        for (int j = 0; j < 8; j++) { // make blank zones
            grid[i][j] = 0;            // first eye
            grid[size - i - 1][j] = 0; // second eye
            grid[j][size - i - 1] = 0; // third eye
        }
    }

    finder(grid, 3, 3);
    finder(grid, 3, size - 4);
    finder(grid, size - 4, 3);

    // if (version > 1)
    for (const auto &x : placement[version]) {
        for (const auto &y : placement[version]) {
            if ((x == 6 && y == 6) || (x == 6 && y == size - 7) || (x == size - 7 && y == 6) ) {
                continue;
            }
            align(grid, x, y);
        }
    }

    if (version >= 7) {
        int byte = versinfo[version];

        // write version information
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 6; j++) {
                bool bit = byte >> (i + j * 3)&1;
                grid[j][size - 11 + i] = bit;
                grid[size - 11 + i][j] = bit;
            }
        }
    }

    // make timing pattern
    for (int i = 7; i < size - 7; i++) {
        grid[6][i] = grid[i][6] = i % 2 == 0;
    }

    grid[8][8] = 0;
    grid[4 * version + 9][8] = 1;  // dark module
    return grid;
}

std::vector<std::pair<int,int>> grid_pos (const std::vector<std::vector<int>> &grid) {
    const int size = grid.size();
    int index = size - 1, cycle = 0;
    std::vector<std::pair<int,int>> zone;

    while (index > 0) {
        if (index == 6) index = 5;

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
std::vector<std::pair<int,int>> info_pos(int size) {
    std::vector<std::pair<int,int>> position;

    for (int i = 0; i < 6; i++) {
        position.push_back({8, i});
        position.push_back({size - i - 1, 8});
    }
    position.push_back({8, 7});
    position.push_back({size - 7, 8});

    position.push_back({8, 8});
    position.push_back({8, size - 8});

    position.push_back({7, 8});
    position.push_back({8, size - 7});

    for (int i = 9; i < 15; i++) {
        position.push_back({14 - i, 8});
        position.push_back({8, size - (14 - i) - 1});
    }
    return position;
}
