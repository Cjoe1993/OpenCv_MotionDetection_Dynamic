#include <iostream>
#include <opencv4/opencv2/videoio.hpp>
#include <opencv4/opencv2/core/mat.hpp>
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <unistd.h>
#include <filesystem>
#include "Detection.h"

#if defined(_WIN32)
#define PLATFORM_NAME "windows" // Windows
#elif defined(_WIN64)
#define PLATFORM_NAME "windows" // Windows
#elif defined(__CYGWIN__) && !defined(_WIN32)
#define PLATFORM_NAME "windows" // Windows (Cygwin POSIX under Microsoft Window)
#elif defined(__ANDROID__)
#define PLATFORM_NAME "android" // Android (implies Linux, so it must come first)
#elif defined(__linux__)
#define PLATFORM_NAME "linux" // Debian, Ubuntu, Gentoo, Fedora, openSUSE, RedHat, Centos and other
#elif defined(__unix__) || !defined(__APPLE__) && defined(__MACH__)
#endif

using namespace cv;
using namespace std;

void zoom(Mat& frame, Mat& frame2, const string& name);
void capMotion(Mat& imgCap, int& num, const string& location);
void capFace(Mat& imgCap, int num, const string& location, int& totSize, int& totSizeX, int& totSizeY);

int main(int argc, char** argv) {

    // number of files to check for face
    int numFiles = 0;
    filesystem::path pathCaptures { "/home/jc/CLionProjects/OpenCVTesting/captures" };

    // add in platform specific pathing for saved images later ---
    cout << "Platform identified as " << PLATFORM_NAME << endl;
    Mat frame, temp, gray, frameDelta, thresh, firstFrame, imgCap;
    // where your captures will be stored
    string captures = "/home/jc/CLionProjects/OpenCVTesting/captures";
    // where your detected faces will be stored
    string detected_faces = "/home/jc/CLionProjects/OpenCVTesting/captures/detected_faces";
//    double scale = 1.2;

    // crop positioning for face shots
    int totSize, totSizeX, totSizeY;

    int num = 0;
    vector<vector<Point> > cnts;

    CascadeClassifier cascadeClassifier;
    cascadeClassifier.load("/usr/share/opencv4/haarcascades/haarcascade_frontalcatface.xml");

    //Start VideoCapture
    VideoCapture cap;
    int deviceID = 0;
    int apiID = CAP_ANY;
    // Open camera with deviceID
    cap.open(deviceID + apiID);
    sleep(1);
    cap.read(frame);

    //convert to grayscale and set the first frame
    cvtColor(frame, firstFrame, COLOR_BGR2GRAY);
    GaussianBlur(firstFrame, firstFrame, Size(21, 21), 0);

    // window names
    const string name1 = "Security Camera";
    const string name2 = "Zoomed";

    //error if videocapture is not opened.
    if (!cap.isOpened()) {
        cerr << "Unable to open camera!\n";
        return -1;
    }
    cout << "Press q to terminate\n" << endl;

    /**
     * @var count
     * every 15 frames, or about once a second with the computational speed on this computer,
     * the firstFrame will reset to the current frame which compensates for lighting and background changes by using the count variable.
     * Count represents frames (slower than actual fps due to computation time, every 15 frames equals roughly 1 second on this machine)
     * and allows us to reset the base comparison frame after the maximum count threshold is reached
     * */

    int count = 0;

    while (cap.read(frame)) {
        // increment counter
        count++;
        //convert to grayscale
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        GaussianBlur(gray, gray, Size(21, 21), 0);

        //compute difference between first frame and current frame
        absdiff(firstFrame, gray, frameDelta);
        // thresh is the minimum difference before motion is detected, higher number is less sensitive. Base = 25
        threshold(frameDelta, thresh, 25, 255, THRESH_BINARY);

        dilate(thresh, thresh, Mat(), Point(-1,-1), 2);
        findContours(thresh, cnts, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        int area = 0;
        int index;
        Rect rect;

        for (int i = 0; i < cnts.size(); i++){
            if (area < cnts[i].size()){
                index = i;
            }
            rect = boundingRect(cnts[index]);
        }

        Point ptx, pty;
        ptx.x = rect.x;
        ptx.y = rect.y;
        pty.x = rect.x + rect.width;
        pty.y = rect.y + rect.height;

        // do not draw rect if area is too small
        if (rect.width < 50 || rect.height < 50){
            ;;
        } else {
            // Uncomment to display rectangle on largest moving mass
//            rectangle(frame, ptx, pty, Scalar(0,255,0), 1);
            // take a picture every 2 frames
            if (count % 2 == 0) {
                capMotion(frame, num, captures);
            }
        }

        //if frame does not initialize, break
        if (frame.empty()) {
            cerr << "ERROR! blank frame grabbed\n";
            break;
        }

        if (count >= 15){
            // reset firstFrame to current frame, and convert to gray
            // reset counter to zero
            firstFrame = frame;
            cvtColor(frame, firstFrame, COLOR_BGR2GRAY);
            GaussianBlur(firstFrame, firstFrame, Size(21, 21), 0);
            count = 0;
        }
        // draw a box/crosshair at center of screen

        // top left corner
        Point pt1(150, 100);Point pt2(190, 100);Point pt3(150, 140);
        line(frame, pt1, pt2, Scalar(125, 125, 125), 1, 8, 0);
        line(frame, pt1, pt3, Scalar(125, 125, 125), 1, 8, 0);
        // top right corner
        Point pt4(490, 100);Point pt5(450, 100);Point pt6(490, 140);
        line(frame, pt4, pt5, Scalar(125, 125, 125), 1, 8, 0);
        line(frame, pt4, pt6, Scalar(125, 125, 125), 1, 8, 0);
        // bottom left corner
        Point pt7(150, 400);Point pt8(190, 400);Point pt9(150, 360);
        line(frame, pt7, pt8, Scalar(125, 125, 125), 1, 8, 0);
        line(frame, pt7, pt9, Scalar(125, 125, 125), 1, 8, 0);
        // bottom right corner
        Point pt10(490, 400);Point pt11(450, 400);Point pt12(490, 360);
        line(frame, pt10, pt11, Scalar(125, 125, 125), 1, 8, 0);
        line(frame, pt10, pt12, Scalar(125, 125, 125), 1, 8, 0);
        // crosshair
        Point x1(300, 250);Point x2(340, 250);Point x3(320, 270);Point x4(320, 230);
        line(frame, x1, x2, Scalar(125, 125, 125), 1, 8, 0);
        line(frame, x3, x4, Scalar(125, 125, 125), 1, 8, 0);

        // show the feed, timeout if q key pressed
        imshow(name1, frame);

        int key = (waitKey(1) & 0xFF);
        if (key == 'q') {
            break;
            // zoom
        } else if (key == 'e') {
            zoom(frame, temp, name2);
            // zoom more
        } else if (key == 'r'){
            zoom(temp, frame, name2);
            // destroy zoom windows
        } else if (key == 'w'){
            destroyWindow(name2);
        } else if (key == 'c'){
            // image capture
//            capImage(frame, num, captures);
        }

    }
    for (auto& p : filesystem::directory_iterator(pathCaptures))
    {
        numFiles++;
    }
    cout << "Number of files " << numFiles << endl;
    // integer below needs to be dynamic
    int facesSpotted = 0;
    Mat image;
    for (int i = 0; i < numFiles; i++) {
        image = imread(captures + "/image_capture_" + to_string(i) + ".jpg");

        if (Detection::detect(image, cascadeClassifier, 1, totSize, totSizeX, totSizeY)) {
            facesSpotted++;
            capFace(image, i, detected_faces, totSize, totSizeX, totSizeY);
            cout << "Spotted @ " << i << endl;
        }
    }
    cout << "Found " << facesSpotted << " faces." << endl;
    cap.release();
    destroyAllWindows();
    return 0;
}
// zoom function
void zoom(Mat& frame, Mat& frame2, const string& name){
    // box coordinates = 150-490x, 100-400y
    Rect roi(150, 100, 340, 300);
    frame2 = frame(roi);
    resize(frame2, frame2, frame.size());
    imshow(name, frame2);
}
// screenshot function
void capMotion(Mat& imgCap, int& num, const string& location){
    Mat img;
    img = imgCap;
    imwrite(location + "/image_capture_" + to_string(num) + ".jpg", img);
    // need to set up proper directory for images, as of now each run inits num to 0.
    num++;
}
// iterate through all motion caps and check if any faces are present; get a close up mugshot
void capFace(Mat& imgCap, int num, const string& location, int& totSize, int& totSizeX, int& totSizeY){
    Mat img;
    Rect closeUp(totSizeX, totSizeY, totSize, totSize);
    img = imgCap(closeUp);
    imwrite(location + "/image_capture_" + to_string(num) + ".jpg", img);
}

