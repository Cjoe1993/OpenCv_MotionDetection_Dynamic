//
// Created by jc on 10/3/21.
//

#include "Detection.h"

bool Detection::detect(cv::Mat& image, cv::CascadeClassifier& cascClassifier, double scale, int& totSize, int& x, int& y) {
    // boolean, true if face detected
    bool b = false;
    std::vector<cv::Rect> detectedFace;
    double fx = 1 / scale;
    cv::Mat gray, smallImg;

    if (!image.empty()) {
        cvtColor(image, gray, cv::COLOR_BGR2GRAY);
        resize( gray, smallImg, cv::Size(), fx, fx, cv::INTER_LINEAR_EXACT );
        equalizeHist( smallImg, smallImg );
        cascClassifier.detectMultiScale( smallImg, detectedFace,
                                         1.1, 2, 0|cv::CASCADE_SCALE_IMAGE,
                                         cv::Size(30, 30) );
    } else {
        ;;
//        std::cout << "Empty image detected." << std::endl;
    }



    if (!detectedFace.empty()) {
        // get position and size of the vector object, which is a cv::Rect
        cv::Rect s;
        s = detectedFace.at(0);
        totSize = s.width;
        x = s.x;
        y = s.y;
        // face detected
        b = true;
    }

    return b;
}

void Detection::parse(std::string& str){

}