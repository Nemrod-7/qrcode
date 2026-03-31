#include <iostream>

#include <opencv2/opencv.hpp>
// #include <opencv2/core.hpp>
// #include <opencv2/imgcodecs.hpp>
// #include <opencv2/highgui.hpp>
//
// -std=c++20 -O2
// -I/usr/include/opencv4 -lopencv_core -lopencv_imgproc -lopencv_imgcodecs  -lopencv_highgui
//
// using namespace cv;
using namespace std;

int main() {
    // Step 1: Importing Libraries. ...
    // Step 2: Access Webcam. ...
    // Step 3: Reading Frames. ...
    // Step 4: Reading Data From Barcode. ...:ls
    //
    // Step 5: Drawing Rectangle Around QR Code and Displaying Data. ...
    // Step 6: Rotate image
    // Step 7: Rescale image

    // Step 7: Rescale image
    const std::string path = cv::samples::findFile("pictures/Micro_QR_Example.pnm");
    cv::Mat img = cv::imread(path, cv::IMREAD_GRAYSCALE);

    if (img.empty()) {
        std::cout << "Could not read the image: " << path << std::endl;
        return 1;
    }

    // printf("width %i height %i\n", img.cols, img.rows);
    // cv::Mat resizedAbsolute;
    // cv::Size newSize(50, 50); // width=300, height=200
    // cv::resize(img, resizedAbsolute, newSize, 0, 0, cv::INTER_LINEAR);

    // Option 2: Resize by scale factor (relative size)
    // cv::Mat resizedScale;
    // double scaleX = 1.0 / 6.0;
    // double scaleY = 1.0 / 6.0;
    // cv::resize(img, resizedScale, cv::Size(), scaleX, scaleY, cv::INTER_LINEAR);

    // Define line color (B, G, R) and thickness
    cv::Scalar lineColor(0, 255, 0); // Green
    int thickness = 1;

    for (int i = 0; i < img.cols; i++) {
      cv::Point startPoint(i, 0);
      cv::Point endPoint(i, img.rows);
      // Draw the line on the image
      cv::line(img, startPoint, endPoint, lineColor, thickness, cv::LINE_AA);
    }



    // Display results
    cv::imshow("Original Image", img);
    // cv::imshow("Resized (Absolute)", resizedAbsolute);
    // cv::imshow("Resized (Scale Factor)", resizedScale);
    //
    // // Save resized images
    // cv::imwrite("resized_absolute.jpg", resizedAbsolute);
    // cv::imwrite("resized_scale.jpg", resizedScale);

    // Wait for a key press indefinitely
    cv::waitKey(0);

    return 0;}
