#include "Window.hpp"
#include <sstream>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "ext/opencv.hpp"
#include "Pulse.hpp"
#include "profiler/Profiler.h"

using std::stringstream;
using namespace cv;

Window::Window(Pulse& pulse) :
    pulse(pulse),
    WINDOW_NAME("EVM"),
    TRACKBAR_MAGNIFY_NAME("Magnify      "),
    TRACKBAR_ALPHA_NAME  ("Amplification")
{
    trackbarMagnify = pulse.evm.magnify;
    trackbarAlpha = pulse.evm.alpha;

    namedWindow(WINDOW_NAME);
    createTrackbar(TRACKBAR_MAGNIFY_NAME, WINDOW_NAME, &trackbarMagnify, 1);
    createTrackbar(TRACKBAR_ALPHA_NAME, WINDOW_NAME, &trackbarAlpha, 500);

    fpsPoint = Point(10, 15);
}

Window::~Window() {
}

void Window::update(Mat& frame) {
    PROFILE_SCOPED();

    // update pulse values for Eulerian video magnification
    pulse.evm.magnify = trackbarMagnify == 1;
    pulse.evm.alpha = trackbarAlpha;

    PROFILE_START_DESC("bgr2rgb");
    cvtColor(frame, frame, CV_BGR2RGB);
    PROFILE_STOP();

    // process frame
    pulse.onFrame(frame);

    drawTrackbarValues(frame);
    drawFps(frame);

    PROFILE_START_DESC("rgb2bgr");
    cvtColor(frame, frame, CV_RGB2BGR);
    PROFILE_STOP();

    PROFILE_START_DESC("imshow");
    imshow(WINDOW_NAME, frame);
    PROFILE_STOP();
}

void Window::drawTrackbarValues(Mat& frame) {
    PROFILE_SCOPED();

    const int namesX = 10;
    const int valuesX = 150;
    const int spaceY = 15;

    stringstream ss;

    putText(frame, TRACKBAR_MAGNIFY_NAME,                 Point( namesX, spaceY * 2), FONT_HERSHEY_PLAIN, 1, BLUE);
    putText(frame, (trackbarMagnify == 1 ? "ON" : "OFF"), Point(valuesX, spaceY * 2), FONT_HERSHEY_PLAIN, 1, BLUE);

    ss.str("");
    ss << trackbarAlpha;
    putText(frame, TRACKBAR_ALPHA_NAME, Point( namesX, spaceY * 3), FONT_HERSHEY_PLAIN, 1, BLUE);
    putText(frame, ss.str(),            Point(valuesX, spaceY * 3), FONT_HERSHEY_PLAIN, 1, BLUE);
}

void Window::drawFps(Mat& frame) {
    PROFILE_SCOPED();

    if (fps.update()) {
        PROFILE_SCOPED_DESC("fps string");
        stringstream ss;
        ss.precision(3);
        ss << "FPS: " << fps.fps;
        fpsString = ss.str();
    }

    PROFILE_START_DESC("fps drawing");
    putText(frame, fpsString, fpsPoint, FONT_HERSHEY_PLAIN, 1, BLUE);
    PROFILE_STOP();
}

Window::Fps::Fps() {
    deltaFrames = 30;
    currentFrame = 0;
    lastFrame = 0;
    lastFpsTime = 0;
    fps = 0;
}

bool Window::Fps::update() {
    PROFILE_SCOPED();

    if (currentFrame == 0) {
        lastFpsTime = (double)getTickCount();
    }

    currentFrame++;

    if (currentFrame % 30 == 0) {
        PROFILE_SCOPED_DESC("fps tick");
        double now = (double)getTickCount();
        double diff = (now - lastFpsTime) * 1000. / getTickFrequency();

        if (diff > 0) {
            fps = (currentFrame - lastFrame) * 1000 / diff;
            lastFrame = currentFrame;
            lastFpsTime = now;
            return true;
        }
    }

    return false;
}
