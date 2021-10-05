//
// Created by jc on 10/3/21.
//
#include <opencv4/opencv2/objdetect.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <vector>
#include <iostream>
#include <sstream>
#include <typeinfo>

#ifndef OPENCVTESTING_DETECTION_H
#define OPENCVTESTING_DETECTION_H


class Detection {
public:
    static bool detect(cv::Mat& image, cv::CascadeClassifier& cascClassifier, double scale, int& totSize, int& x, int& y);
    static void parse(std::string& str);
};


#endif //OPENCVTESTING_DETECTION_H
