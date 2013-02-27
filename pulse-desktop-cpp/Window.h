#ifndef WINDOW_H
#define	WINDOW_H

#include <string>
#include <opencv2/opencv.hpp>
#include "EvmGdownIIR.h"

using namespace std;
using namespace cv;

class Window {
public:
    Window(EvmGdownIIR& evm);
    virtual ~Window();

    void update(Mat& frame);

private:
    void showTrackbarValues(Mat& frame);
    void showFPS(Mat& frame);

    EvmGdownIIR& evm;

    const string WINDOW_NAME;
    const string TRACKBAR_BLUR_NAME;
    const string TRACKBAR_F_HIGH_NAME;
    const string TRACKBAR_F_LOW_NAME;
    const string TRACKBAR_ALPHA_NAME;

    int TRACKBAR_BLUR;
    int TRACKBAR_F_HIGH;
    int TRACKBAR_F_LOW;
    int TRACKBAR_ALPHA;
    
};

#endif	/* WINDOW_H */

