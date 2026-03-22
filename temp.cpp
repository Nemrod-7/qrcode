#include <iostream>
#include <vector>

namespace QR {
    namespace info {
        const std::string ecc[4] = {"Medium", "Low", "Quartile", "High"};
        const std::string mode[4] = {"NUMERIC", "ALPHANUM", "BYTE", "KANJI"};
    };
};

std::string grid (const std::vector<std::vector<int>> &grid) {
    std::string os;

    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid.size(); j++) {
            os += (grid[i][j] == 0) ? ". " : "# ";
        }
        os += '\n';
    }
    return os;
}

int main () {

  const std::vector<std::vector<int>> wiki = {
      {1,1,1,1,1,1,1,0,1,0,1,0,1,0,1},
      {1,0,0,0,0,0,1,0,0,0,0,0,1,0,1},
      {1,0,1,1,1,0,1,0,0,1,1,1,1,0,1},
      {1,0,1,1,1,0,1,0,0,0,0,1,1,1,0},
      {1,0,1,1,1,0,1,0,1,1,1,1,0,1,0},
      {1,0,0,0,0,0,1,0,1,0,0,1,0,1,1},
      {1,1,1,1,1,1,1,0,1,0,1,1,0,1,1},
      {0,0,0,0,0,0,0,0,1,1,1,0,1,1,0},
      {1,1,1,1,0,1,1,0,0,1,1,0,1,1,0},
      {0,0,0,1,1,0,1,1,0,1,0,1,0,1,0},
      {1,1,1,0,0,1,1,1,1,0,1,1,0,0,0},
      {0,0,0,1,1,1,1,1,0,0,0,1,1,0,1},
      {1,1,0,0,0,0,0,0,1,0,1,1,1,0,1},
      {0,0,1,1,1,0,0,0,0,0,1,1,1,1,1},
      {1,0,0,1,0,1,1,0,1,0,1,0,0,0,1}
  };

  std::string os;

  for (int i = 0; i < grid.size(); i++) {
      for (int j = 0; j < grid.size(); j++) {
          os += (grid[i][j] == 0) ? ". " : "# ";
      }
      os += '\n';
  }




}
