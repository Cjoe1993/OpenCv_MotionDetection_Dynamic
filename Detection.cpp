//
// Created by jc on 10/3/21.
//

#include "Detection.h"

bool Detection::detect(cv::Mat& image, cv::CascadeClassifier& cascClassifier, double scale) {
    // boolean, true if face detected
    bool b;
    std::vector<cv::Rect> detectedFace;
//    cv::Mat gray, secondImg;
//
//    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    cv::Mat gray, smallImg;
    cvtColor( image, gray, cv::COLOR_BGR2GRAY );
    double fx = 1 / scale;
    resize( gray, smallImg, cv::Size(), fx, fx, cv::INTER_LINEAR_EXACT );
    equalizeHist( smallImg, smallImg );

    cascClassifier.detectMultiScale( smallImg, detectedFace,
                                     1.1, 2, 0|cv::CASCADE_SCALE_IMAGE,
                                     cv::Size(30, 30) );

//    std::cout << detectedFace.size() << std::endl;

    if (!detectedFace.empty()) {
        b = true;
    } else {
        b = false;
    }
    return b;
}