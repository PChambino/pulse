#include <iostream>
#include <sstream>
#include <opencv2/opencv.hpp>

#include "EvmGdownIIR.h"

using namespace std;
using namespace cv;

void showFPS(Mat& frame);

int main(int argc, char** argv) {
    const string WINDOW_NAME = "EVM";
    namedWindow(WINDOW_NAME);

    VideoCapture capture;
//    capture.open("../../vidmagSIGGRAPH2012/face_source_timecode.wmv");
    capture.open(0);

    const int FPS = capture.get(CV_CAP_PROP_FPS);
    cout << "FPS: " << FPS << endl;
    const int DELAY = 1;//FPS > 0 ? 1000 / FPS : 50;

    const int WIDTH = capture.get(CV_CAP_PROP_FRAME_WIDTH);
    const int HEIGHT = capture.get(CV_CAP_PROP_FRAME_HEIGHT);

    cout << "WIDTH: " << WIDTH << endl;
    cout << "HEIGHT: " << HEIGHT << endl;

    EvmGdownIIR evm;
    evm.load("lbpcascade_frontalface.xml");
    evm.start(WIDTH, HEIGHT);

    Mat frame, output;
    while (true) {
        capture >> frame;
        if (frame.empty()) {
            break;
        }

        evm.onFrame(frame, output);

        showFPS(output);

        imshow(WINDOW_NAME, output);
        
        if (waitKey(DELAY) == 27) {
            break;
        }
    }

    return 0;
}

void showFPS(Mat& frame) {
    static int frameCounter = 0;
    static int lastFrameCounter = 0;
    static double lastFpsTime = (double)getTickCount();
    static string fpsStr;
    static Point point(10, 15);
    static Scalar color(255, 0, 0);

    frameCounter++;

    if ((frameCounter % 30) != 0) {
        cv::putText(frame, fpsStr, point, FONT_HERSHEY_PLAIN, 1, color);
        return;
    }

    double now = (double)getTickCount();
    double diff = (now - lastFpsTime) * 1000. / getTickFrequency();

    if (diff == 0) {
        return;
    }

    double fps = (frameCounter - lastFrameCounter) * 1000 / diff;
    lastFrameCounter = frameCounter;
    lastFpsTime = now;

    stringstream ss;
    ss.precision(3);
    ss << "FPS: " << fps;
    fpsStr = ss.str();
    
    cout << fpsStr << endl;
    cv::putText(frame, fpsStr, point, FONT_HERSHEY_PLAIN, 1, color);
}
