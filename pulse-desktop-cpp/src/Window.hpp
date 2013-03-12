#ifndef WINDOW_HPP
#define	WINDOW_HPP

#include <string>
#include <opencv2/core/core.hpp>
#include "Pulse.hpp"

using std::string;
using cv::Mat;
using cv::Point;

class Window {
public:
    Window(Pulse& pulse);
    virtual ~Window();

    void update(Mat& frame);

private:
    void drawTrackbarValues(Mat& frame);
    void drawFps(Mat& frame);

    Pulse& pulse;

    const string WINDOW_NAME;
    const string TRACKBAR_BLUR_NAME;
    const string TRACKBAR_F_HIGH_NAME;
    const string TRACKBAR_F_LOW_NAME;
    const string TRACKBAR_ALPHA_NAME;

    int trackbarBlur;
    int trackbarFHigh;
    int trackbarFLow;
    int trackbarAlpha;
        
    class Fps {
    public:
        int deltaFrames;
        uint64 currentFrame;
        uint64 lastFrame;
        double lastFpsTime;
        double fps;

        Fps();
        bool update();
    };

    Fps fps;
    string fpsString;
    Point fpsPoint;
    
};

#endif	/* WINDOW_HPP */

