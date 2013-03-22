#include "EvmGdownIIR.hpp"
#include "profiler/Profiler.h"
#include <opencv2/imgproc/imgproc.hpp>

using cv::pyrDown;
using cv::pyrUp;
using cv::resize;

EvmGdownIIR::EvmGdownIIR() {
    first = true;
    blurLevel = 4;
    fLow = 50/60./10;
    fHigh = 60/60./10;
    alpha = 200;
}

EvmGdownIIR::~EvmGdownIIR() {
}

void EvmGdownIIR::onFrame(const Mat& src, Mat& out) {
    PROFILE_SCOPED();
    
    // convert to float
    PROFILE_START_DESC("convert to float");
    src.convertTo(srcFloat, CV_32F);
    PROFILE_STOP();

    // apply spatial filter: blur and downsample
    PROFILE_START_DESC("pyrDown");
    srcFloat.copyTo(blurred);
    for (int i = 0; i < blurLevel; i++) {
        pyrDown(blurred, blurred);
    }
    PROFILE_STOP();

    if (first) {
        PROFILE_SCOPED_DESC("first");
        first = false;
        blurred.copyTo(lowpassHigh);
        blurred.copyTo(lowpassLow);
        src.copyTo(out);
    } else {
        // apply temporal filter: subtraction of two IIR lowpass filters
        PROFILE_START_DESC("temporal filter");
        lowpassHigh = lowpassHigh * (1-fHigh) + fHigh * blurred;
        lowpassLow = lowpassLow * (1-fLow) + fLow * blurred;
        blurred = lowpassHigh - lowpassLow;
        PROFILE_STOP();

        // amplify
        PROFILE_START_DESC("amplify");
        blurred *= alpha;
        PROFILE_STOP();
        
        // resize back to original size
        PROFILE_START_DESC("pyrUp");
        for (int i = 0; i < blurLevel; i++) {
            pyrUp(blurred, blurred);
        }
        resize(blurred, outFloat, src.size());
        PROFILE_STOP();

        // add back to original frame
        PROFILE_START_DESC("add back to original frame");
        outFloat += srcFloat;
        PROFILE_STOP();

        // convert to 8 bit
        PROFILE_START_DESC("convert to 8 bit");
        outFloat.convertTo(out, CV_8U);
        PROFILE_STOP();
    }
}

