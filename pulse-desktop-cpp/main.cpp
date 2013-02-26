#include <iostream>
#include <sstream>
#include <opencv2/opencv.hpp>

#include "EvmGdownIIR.h"

using namespace std;
using namespace cv;

static void writeVideo(VideoCapture capture, const Mat& frame);
static void showFPS(Mat& frame);

int main(int argc, char** argv) {
    const string WINDOW_NAME = "EVM";
    namedWindow(WINDOW_NAME);

    VideoCapture capture;
//    capture.open("../../vidmagSIGGRAPH2012/face_source_timecode.wmv");
    capture.open(0);

    const double FPS = capture.get(CV_CAP_PROP_FPS);
    cout << "FPS: " << FPS << endl;
    const int DELAY = 1;//FPS > 0 ? 1000 / FPS : 50;

    const int WIDTH = capture.get(CV_CAP_PROP_FRAME_WIDTH);
    const int HEIGHT = capture.get(CV_CAP_PROP_FRAME_HEIGHT);

    cout << "WIDTH: " << WIDTH << endl;
    cout << "HEIGHT: " << HEIGHT << endl;

    EvmGdownIIR evm;
    evm.load("lbpcascade_frontalface.xml");
    evm.start(WIDTH, HEIGHT);

    const string TRACKBAR_BLUR_NAME = "Blur level";
    const string TRACKBAR_F_HIGH_NAME = "High cut-off";
    const string TRACKBAR_F_LOW_NAME = "Low cut-off";
    const string TRACKBAR_ALPHA_NAME = "Amplification";
    
    int TRACKBAR_BLUR = evm.blurLevel;
    int TRACKBAR_F_HIGH = evm.fHigh * 600;
    int TRACKBAR_F_LOW = evm.fLow * 600;
    int TRACKBAR_ALPHA = evm.alpha;

    createTrackbar(TRACKBAR_BLUR_NAME, WINDOW_NAME, &TRACKBAR_BLUR, 8);
    createTrackbar(TRACKBAR_F_HIGH_NAME, WINDOW_NAME, &TRACKBAR_F_HIGH, 240);
    createTrackbar(TRACKBAR_F_LOW_NAME, WINDOW_NAME, &TRACKBAR_F_LOW, 240);
    createTrackbar(TRACKBAR_ALPHA_NAME, WINDOW_NAME, &TRACKBAR_ALPHA, 400);


    Mat frame, output;
    while (true) {
        capture >> frame;
        if (frame.empty()) {
            break;
        }

        if (evm.blurLevel != TRACKBAR_BLUR) {
            evm.first = true;
        }
        evm.blurLevel = TRACKBAR_BLUR;
        evm.fHigh = TRACKBAR_F_HIGH / 600.;
        evm.fLow = TRACKBAR_F_LOW / 600.;
        evm.alpha = TRACKBAR_ALPHA;

        evm.onFrame(frame, output);

        static stringstream ss;
        ss << TRACKBAR_BLUR_NAME << ": " << TRACKBAR_BLUR;
        putText(output, ss.str(), Point(10, 30), FONT_HERSHEY_PLAIN, 1, Scalar(255, 0, 0));
        ss.str("");
        ss << TRACKBAR_F_HIGH_NAME << ": " << TRACKBAR_F_HIGH;
        putText(output, ss.str(), Point(10, 45), FONT_HERSHEY_PLAIN, 1, Scalar(255, 0, 0));
        ss.str("");
        ss << TRACKBAR_F_LOW_NAME << ": " << TRACKBAR_F_LOW;
        putText(output, ss.str(), Point(10, 60), FONT_HERSHEY_PLAIN, 1, Scalar(255, 0, 0));
        ss.str("");
        ss << TRACKBAR_ALPHA_NAME << ": " << TRACKBAR_ALPHA;
        putText(output, ss.str(), Point(10, 75), FONT_HERSHEY_PLAIN, 1, Scalar(255, 0, 0));
        ss.str("");

        showFPS(output);

//        writeVideo(capture, output);

        imshow(WINDOW_NAME, output);
        
        if (waitKey(DELAY) == 27) {
            break;
        }
    }

    return 0;
}

static void writeVideo(VideoCapture capture, const Mat& frame) {
    static VideoWriter writer("out.avi",
            CV_FOURCC('X', 'V', 'I', 'D'),
            capture.get(CV_CAP_PROP_FPS),
            Size(capture.get(CV_CAP_PROP_FRAME_WIDTH), 
            capture.get(CV_CAP_PROP_FRAME_HEIGHT)));

    writer << frame;
}

static void showFPS(Mat& frame) {
    static int frameCounter = 0;
    static int lastFrameCounter = 0;
    static double lastFpsTime = (double)getTickCount();
    static string fpsStr;
    static Point point(10, 15);
    static Scalar color(255, 0, 0);

    frameCounter++;

    if ((frameCounter % 30) != 0) {
        putText(frame, fpsStr, point, FONT_HERSHEY_PLAIN, 1, color);
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

    static stringstream ss;
    ss.str("");
    ss.precision(3);
    ss << "FPS: " << fps;
    fpsStr = ss.str();
    
    cout << fpsStr << endl;
    putText(frame, fpsStr, point, FONT_HERSHEY_PLAIN, 1, color);
}
