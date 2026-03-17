#include <iostream>
#include <fstream>
#include <vector>

#include "include/image.hpp"

using namespace std;
// using u8 = unsigned char;

int main() {

    const string name = "pictures/QRcode_Wikipedia_FRA.pnm";

    Image pnm = Image::from_file(name);




    std::cout << "\nend\n";
}
