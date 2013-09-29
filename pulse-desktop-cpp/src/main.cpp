#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include "EvmGdownIIR.hpp"
#include "Pulse.hpp"
#include "Window.hpp"
#include "profiler/Profiler.h"

using std::cout;
using std::endl;
using cv::VideoCapture;
using cv::VideoWriter;
using cv::waitKey;
using cv::flip;

static void writeVideo(VideoCapture& capture, const Mat& frame);

int main(int argc, const char** argv) {
    const bool shouldWrite = true;
//    const bool shouldFlip = true;
    const bool shouldFlip = false;

//    VideoCapture capture("../../vidmagSIGGRAPH2012/baby2_source.mp4");
//    VideoCapture capture("../../vidmagSIGGRAPH2012/face.wmv");
//    VideoCapture capture("../../vidmagSIGGRAPH2012/face2_source.mp4");
//    VideoCapture capture("../../vidmagSIGGRAPH2012/face_source.wmv");
//    VideoCapture capture("../../videos/eva.mov");
//    VideoCapture capture("../../videos/me-high-pulse.mov");
    VideoCapture capture("../../videos/me2.mov");
//    VideoCapture capture(0);

    const int WIDTH  = capture.get(CV_CAP_PROP_FRAME_WIDTH);
    const int HEIGHT = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
    const double FPS = capture.get(CV_CAP_PROP_FPS);
    cout << "SIZE: " << WIDTH << "x" << HEIGHT << endl;

    Pulse pulse;
    if (FPS != 0) {
        cout << "FPS: " << FPS << endl;
        pulse.fps = FPS;
    }
    pulse.load("res/lbpcascade_frontalface.xml");
    pulse.start(WIDTH, HEIGHT);

    Window window(pulse);

    Mat frame;
    while (true) {
        PROFILE_SCOPED_DESC("loop");

        PROFILE_START_DESC("capture");
        capture >> frame;
        PROFILE_STOP();

        if (frame.empty()) {
            PROFILE_PAUSE_SCOPED(); // loop
            while (waitKey() != 27) {}
            break;
        }

        PROFILE_START_DESC("flip");
        if (shouldFlip)
            flip(frame, frame, 1);
        PROFILE_STOP();

        window.update(frame);

        if (shouldWrite) {
            writeVideo(capture, frame);
        }

        PROFILE_START_DESC("wait key");
        if (waitKey(1) == 27) {
            PROFILE_STOP(); // wait key
            break;
        }
        PROFILE_STOP();
    }

    Profiler::detect(argc, argv);
    Profiler::dumphtml();
    return 0;
}

static void writeVideo(VideoCapture& capture, const Mat& frame) {
    static VideoWriter writer("out.avi",
            CV_FOURCC('X', 'V', 'I', 'D'),
            capture.get(CV_CAP_PROP_FPS),
            Size(capture.get(CV_CAP_PROP_FRAME_WIDTH),
            capture.get(CV_CAP_PROP_FRAME_HEIGHT)));

    writer << frame;
}
