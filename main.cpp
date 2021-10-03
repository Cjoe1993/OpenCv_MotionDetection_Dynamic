#include <iostream>
#include <opencv4/opencv2/videoio.hpp>
#include <opencv4/opencv2/core/mat.hpp>
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <unistd.h>

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
void capImage(Mat& imgCap, int& num);

int main(int argc, char** argv) {
    // add in platform specific pathing for saved images later ---
    cout << "Platform identified as " << PLATFORM_NAME << endl;
//    String DIR;
    Mat frame, temp, gray, frameDelta, thresh, firstFrame, imgCap;
    int num = 0;
    vector<vector<Point> > cnts;

    //Start VideoCapture
    VideoCapture cap;
    // Open default camera
    cap.open(0);
    int deviceID = 0;
    int apiID = CAP_ANY;
    // Open camera with deviceID
    cap.open(deviceID + apiID);
    sleep(3);
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
        for (auto & cnt : cnts) {
            if (contourArea(cnt) < 500) {
                continue;
            }
        }
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
            rectangle(frame, ptx, pty, Scalar(0,255,0), 1);
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
//        uncomment to display fps
//        double fps = cap.get(CAP_PROP_FPS);
//        String str = to_string(fps);
//        Point point(30, 30);
//        putText(frame, str, point,
//                FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(200, 200, 250), 1, 8, false);


//draw a box/crosshair at center of screen

        // top left corner
        Point pt1(150, 100);
        Point pt2(190, 100);
        Point pt3(150, 140);
        line(frame, pt1, pt2, Scalar(125, 125, 125), 1, 8, 0);
        line(frame, pt1, pt3, Scalar(125, 125, 125), 1, 8, 0);
        // top right corner
        Point pt4(490, 100);
        Point pt5(450, 100);
        Point pt6(490, 140);
        line(frame, pt4, pt5, Scalar(125, 125, 125), 1, 8, 0);
        line(frame, pt4, pt6, Scalar(125, 125, 125), 1, 8, 0);
        // bottom left corner
        Point pt7(150, 400);
        Point pt8(190, 400);
        Point pt9(150, 360);
        line(frame, pt7, pt8, Scalar(125, 125, 125), 1, 8, 0);
        line(frame, pt7, pt9, Scalar(125, 125, 125), 1, 8, 0);
        // bottom right corner
        Point pt10(490, 400);
        Point pt11(450, 400);
        Point pt12(490, 360);
        line(frame, pt10, pt11, Scalar(125, 125, 125), 1, 8, 0);
        line(frame, pt10, pt12, Scalar(125, 125, 125), 1, 8, 0);
        // crosshair
        Point x1(300, 250);
        Point x2(340, 250);
        Point x3(320, 270);
        Point x4(320, 230);
        line(frame, x1, x2, Scalar(125, 125, 125), 1, 8, 0);
        line(frame, x3, x4, Scalar(125, 125, 125), 1, 8, 0);

        // show the feed, timeout if key pressed
        imshow(name1, frame);
// Uncomment to show the frame that will be the base comparison frame in addition to the live feed
//        imshow("ff", firstFrame);

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
            capImage(frame, num);
        }

    }
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
void capImage(Mat& imgCap, int& num){
    // need to set up proper directory for images, as of now each run inits num to 0.
    imwrite("/home/jc/image_capture_" + to_string(num) + ".jpg", imgCap);
    num++;
}
