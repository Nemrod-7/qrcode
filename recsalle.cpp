#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

#include <png.h>

using namespace std;


int main() {

    const char *fileName = "picture/QRcode_Wikipedia_FRA.png";

    // We try to open the image file
    FILE * inputImageFile;

    if((inputImageFile = fopen(fileName, "rb")) == NULL) {
        throw std::runtime_error("Error : can't open this file");
    }

    // We start the decompression
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop imageInfo = png_create_info_struct(png);
    png_init_io(png, inputImageFile);
    png_read_info(png, imageInfo);
    // We store the image informations into privates variables
    unsigned int width = png_get_image_width(png, imageInfo);
    unsigned int height = png_get_image_height(png, imageInfo);
    unsigned int colorType = png_get_color_type(png, imageInfo);
    unsigned int bitDepth = png_get_bit_depth(png, imageInfo);

    // We continue to read the image
    png_read_update_info(png, imageInfo);

    // We create a table to store the pixels values
    png_bytep * rowPointers;
    rowPointers = (png_bytep*)malloc(sizeof(png_bytep) * height);

    // We allocate memory
    for (unsigned int i = 0; i < height; i++) {
        rowPointers[i] = (png_byte*)malloc(png_get_rowbytes(png, imageInfo));
    }

    // We finish the decompression
    png_read_image(png, rowPointers);
    png_destroy_read_struct(&png, &imageInfo, NULL);

    std::cout << "Image Dimensions : " << width << "x" << height <<"\n";

    // Now you can get the rgb values like this :
    int x = 4;
    int y = 7;

    png_bytep pixel = &(rowPointers[y][x * 4]); // 4 for R, G, B, and the alpha value (the transparance)
                                                // The "+" is here to print the value as a number
    std::cout << "Image Pixel (x = 4, y = 7) : RGB(" << +pixel[0] << ", " << +pixel[1] << ", " << +pixel[2] << ")" << "\n";
    std::cout << "Image Pixel (x = 4, y = 7) : transparance : " << +pixel[3] << "\n";
    // std::cout << Infos::grid(qr);
    //
    std::cout << "\nend\n";
}
