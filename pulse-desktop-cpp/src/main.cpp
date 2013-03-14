#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include "EvmGdownIIR.hpp"
#include "Pulse.hpp"
#include "Window.hpp"

using std::cout;
using std::endl;
using cv::VideoCapture;
using cv::VideoWriter;
using cv::waitKey;

static void writeVideo(VideoCapture& capture, const Mat& frame);

int main(int argc, char** argv) {
    const bool shouldWrite = false;

//    VideoCapture capture("../../vidmagSIGGRAPH2012/face.wmv");
//    VideoCapture capture("../../vidmagSIGGRAPH2012/face2_source.mp4");
//    VideoCapture capture("../../vidmagSIGGRAPH2012/face_source_timecode.wmv");
    VideoCapture capture(0);

    const int WIDTH  = capture.get(CV_CAP_PROP_FRAME_WIDTH);
    const int HEIGHT = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
    const double FPS = capture.get(CV_CAP_PROP_FPS);
//    cout << "SIZE: " << WIDTH << "x" << HEIGHT << endl;

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
        capture >> frame;
        if (frame.empty()) {
            while (waitKey() != 27) {}
            break;
        }

        window.update(frame);

        if (shouldWrite) {
            writeVideo(capture, frame);
        }

        if (waitKey(1) == 27) {
            break;
        }
    }

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
