#include <iostream>
#include <vector>

#include <set>
#include "include/image.hpp"
#include "include/qr.hpp"

using namespace std;

vector<int> filter(const vector<int> &ve) {
    vector<int> res;

    for (int i = 1; i < ve.size(); i++) {
        // int diff = ve[i] - ve[i - 1];
        // if (diff != 1) {
            res.push_back(ve[i]);
        // }
    }

    return res;
}


Image crop (const Image &pic) {

    int minx = 0, miny = 0;
    int maxx = pic.width - 1, maxy = pic.height - 1;

    do {
        int cell = 0;

        for (int x = 0; x < pic.width; x++) {
            if (pic.pixels[miny * pic.width + x] == black) {
                cell++;
            }
        }

        if (cell > 6) break;
    } while (miny++ < pic.height);

    do {
        int cell = 0;

        for (int y = 0; y < pic.height; y++) {
            if (pic.pixels[y * pic.width + minx] == black) {
                cell++;
            }
        }
        if (cell > 6) break;
    } while (minx++ < pic.width);

    do {
        int cell = 0;
        for (int x = 0; x < pic.width; x++) {
            if (pic.pixels[maxy * pic.width + x] == black) {
                cell++;
            }
        }
        if (cell > 6) break;
    } while (maxy-->0);

    do {
        int cell = 0;
        for (int y = 0; y < pic.height; y++) {
            if (pic.pixels[y * pic.width + maxx] == black) {
                cell++;
            }
        }
        if (cell > 6) break;
    } while (maxx-->0);


    // printf("%i %i\n", minx, miny);
    int nh = maxy - miny, nw = maxx - minx;
    Image next;

    for (int y = miny; y < maxy; y++) {
        for (int x = minx; x < maxx; x++) {
            next.pixels.push_back(pic.pixels[y * pic.width + x]);
        }
    }
    next.width = nw, next.height = nw;

    return next;
}

void ttt(const Image &pic) {
    set<int> histx = {0}, histy = {0};

    for (int y = 0; y < pic.height; y++) {
        int cell = 0;

        for (int x = 1; x < pic.width; x++) {
            int ix = y * pic.width + x;

            if (pic.pixels[ix] != pic.pixels[ix - 1] ) {
                cell++;
                histx.insert(x);
            }
        }

        printf("%i ", cell);


    }

    // for (int x = 0; x < pic.width; x++) {
    //     for (int y = 1; y < pic.height; y++) {
    //         int ix = y * pic.width + x;
    //         int px = (y - 1) * pic.width + x;
    //
    //         if (pic.pixels[ix] != pic.pixels[px]) {
    //             histy.insert(y);
    //         } 
    //     }
    // }
    //
    // vector<int> coordx = {histx.begin(), histx.end()};
    // vector<int> coordy = {histy.begin(), histy.end()};
    //
    // for (int i = 1; i < coordy.size(); i++) {
    //     int y = coordy[i];
    //
    //     printf("%i ", coordy[i] - coordy[i-1]);
    //     for (int j = 0; j < coordx.size(); j++) {
    //         int x = coordx[j];
    //         int ix = y * pic.width + x;
    //
    //         int bit = pic.pixels[ix] == black ? '#' : ' ';
    //         // printf("%c ", bit );
    //     }
    //     // printf("\n");
    // }

}

int main() {

    Image pic;
    pic = Image::from_file ("pictures/Micro_QR_Example.pnm");
    pic = simpl_thresh(pic);
    pic = crop(pic);


    int scale = 3;
    int nw = pic.width / scale;
    int nh = pic.height / scale;
    std::vector<std::vector<int>> qr (nh, vector<int>(nw)) ;

    // for (int y = 0; y < nh; y++) {
    //     for (int x = 0; x < nw; x++) {
    //         const int sx = x * scale, sy = y * scale;
    //         const int nx = (x + 1) * scale, ny = (y + 1) * scale;
    //
    //         int kernel = 0;
    //         for (int i = 0; i < scale; i++) {
    //             for (int j = 0; j < scale; j++) {
    //                 int pix = pic.pixels[(sy + i) * pic.width + (sx + j) ];
    //                 kernel += pix;
    //             }
    //         }
    //
    //         int bit = kernel / (scale * scale);
    //
    //         qr[y][x] = (bit < 255 / 2) ? 1 : 0;
    //
    //     }
    // }
    //
    // cout << Infos::grid(qr);

    // for (int y = 0; y < nh; y++) {
    //     for (int x = 0; x < nw; x++) {
    //
    //     }
    // }

    // cout << "[" << txt << "]\n";
    // std::cout << "\nend\n";
}
