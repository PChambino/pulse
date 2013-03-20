#include "Window.hpp"
#include <sstream>
#include <opencv2/highgui/highgui.hpp>
#include "ext/opencv.hpp"
#include "Pulse.hpp"
#include "profiler/Profiler.h"

using std::stringstream;
using namespace cv;

Window::Window(Pulse& pulse) :
    pulse(pulse),
    WINDOW_NAME("EVM"),
    TRACKBAR_BLUR_NAME("Blur level"),
    TRACKBAR_F_HIGH_NAME("High cut-off"),
    TRACKBAR_F_LOW_NAME("Low cut-off"),
    TRACKBAR_ALPHA_NAME("Amplification")
{
//    trackbarBlur  = pulse.evm.blurLevel;
//    trackbarFHigh = pulse.evm.fHigh * 600;
//    trackbarFLow  = pulse.evm.fLow * 600;
//    trackbarAlpha = pulse.evm.alpha;
    
    namedWindow(WINDOW_NAME);
//    createTrackbar(TRACKBAR_BLUR_NAME, WINDOW_NAME, &trackbarBlur, 8);
//    createTrackbar(TRACKBAR_F_HIGH_NAME, WINDOW_NAME, &trackbarFHigh, 240);
//    createTrackbar(TRACKBAR_F_LOW_NAME, WINDOW_NAME, &trackbarFLow, 240);
//    createTrackbar(TRACKBAR_ALPHA_NAME, WINDOW_NAME, &trackbarAlpha, 1000);
    
    fpsPoint = Point(10, 15);
}

Window::~Window() {
}

void Window::update(Mat& frame) {
    PROFILE_SCOPED();
    
//    if (pulse.evm.blurLevel != trackbarBlur) {
//        pulse.evm.first = true;
//    }
//    pulse.evm.blurLevel = trackbarBlur;
//    pulse.evm.fHigh = trackbarFHigh / 600.;
//    pulse.evm.fLow = trackbarFLow / 600.;
//    pulse.evm.alpha = trackbarAlpha;

    pulse.onFrame(frame);

//    drawTrackbarValues(frame);
    drawFps(frame);

    PROFILE_START_DESC("imshow");
    imshow(WINDOW_NAME, frame);
    PROFILE_STOP();
}

void Window::drawTrackbarValues(Mat& frame) {
    PROFILE_SCOPED();
    
    stringstream ss;

    ss << TRACKBAR_BLUR_NAME << ": " << trackbarBlur;
    putText(frame, ss.str(), Point(10, 30), FONT_HERSHEY_PLAIN, 1, BLUE);
    ss.str("");

    ss << TRACKBAR_F_HIGH_NAME << ": " << trackbarFHigh;
    putText(frame, ss.str(), Point(10, 45), FONT_HERSHEY_PLAIN, 1, BLUE);
    ss.str("");

    ss << TRACKBAR_F_LOW_NAME << ": " << trackbarFLow;
    putText(frame, ss.str(), Point(10, 60), FONT_HERSHEY_PLAIN, 1, BLUE);
    ss.str("");

    ss << TRACKBAR_ALPHA_NAME << ": " << trackbarAlpha;
    putText(frame, ss.str(), Point(10, 75), FONT_HERSHEY_PLAIN, 1, BLUE);
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
