#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

// -std=c++20 -O3
// -I/usr/include/opencv4 -lopencv_core -lopencv_imgproc -lopencv_imgcodecs  -lopencv_highgui
using namespace cv;
using namespace std;

int main() {
    // Step 1: Importing Libraries. ...
    // Step 2: Access Webcam. ...
    // Step 3: Reading Frames. ...
    // Step 4: Reading Data From Barcode. ...
    // Step 5: Drawing Rectangle Around QR Code and Displaying Data. ...
    // Step 6: Rotate image
    // Step 7: Rescale image

    const std::string path = samples::findFile("QRcode_Wikipedia_FRA.png");
    Mat img = imread(path, IMREAD_COLOR);

    if (img.empty()) {
        std::cout << "Could not read the image: " << path << std::endl;
        return 1;
    }
    printf("width %i height %i\n", img.cols, img.rows);


    ret,thresh = cv.threshold(img,127,255,0)
    im2,contours,hierarchy = cv.findContours(thresh, 1, 2)
    cnt = contours[0]
    M = cv.moments(cnt)
    print( M )
    // get contour
    // rotate if necessary
    // rescale

    // for (int y = 0; y < img.rows; y++) {
    //   for (int x = 0; x < img.cols; x++) {
    //     int p = img.at<uchar>(y,x);
    //
    //     if (p < 255) {
    //         cout << p << " ";
    //     }
    //   }
    // }
    // imshow("Display window", img);
    int key = waitKey(0); // Wait for a keystroke in the window


    return 0;
}
