#include <iostream>
#include <opencv4/opencv2/videoio.hpp>
#include <opencv4/opencv2/core/mat.hpp>
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <unistd.h>

using namespace cv;
using namespace std;

void zoom(Mat& frame, Mat& frame2, const string& name);
int main(int argc, char** argv) {
    // frames
    Mat frame, temp, gray, frameDelta, thresh, firstFrame;
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

    // count, representing frames (slower than actual fps due to computation time, every 15 frames equals roughly 1 second here,
    // even though the default fps is 30)
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

        for(auto & cnt : cnts) {
            if(contourArea(cnt) < 500) {
                continue;
            }
            putText(frame, "Motion Detected", Point(10, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0,0,255),2);
        }
        //if frame does not initialize, break
        if (frame.empty()) {
            cerr << "ERROR! blank frame grabbed\n";
            break;
        }
        // every 15 frames, or about once a second with the computational speed on this computer,
        // the firstFrame will reset to the current frame which compensates for lighting and background changes.
        // aka rudimentary dynamic background sensor.
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