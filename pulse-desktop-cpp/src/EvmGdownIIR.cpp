#include "EvmGdownIIR.hpp"
#include "profiler/Profiler.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/core/core.hpp>

using cv::pyrDown;
using cv::pyrUp;
using cv::resize;

EvmGdownIIR::EvmGdownIIR() {
    first = true;
    blurredSize = Size(10, 10);
    fLow = 70/60./10;
    fHigh = 80/60./10;
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
    resize(srcFloat, blurred, blurredSize, 0, 0, CV_INTER_AREA);
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
        resize(blurred, outFloat, src.size(), 0, 0, CV_INTER_LINEAR);
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

