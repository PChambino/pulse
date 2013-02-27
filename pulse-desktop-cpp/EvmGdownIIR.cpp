#include "EvmGdownIIR.h"
#include "Window.h"
#include "colors.h"
#include <sstream>

EvmGdownIIR::EvmGdownIIR() {
    first = true;
    blurLevel = 4;
    fLow = 50/60./10;
    fHigh = 60/60./10;
    alpha = 200;
    minFaceSizePercentage = 0.3;
    t = 0;
}

EvmGdownIIR::~EvmGdownIIR() {
}

void EvmGdownIIR::load(const string& filename) {
    classifier.load(filename);
}

void EvmGdownIIR::start(int width, int height) {
    srcSize = Size(width, height);
    minFaceSize = Size(width * minFaceSizePercentage, height * minFaceSizePercentage);
}

void EvmGdownIIR::onFrame(const Mat& src, Mat& out) {
    // convert to float
    src.convertTo(srcFloat, CV_32F);

    // apply spatial filter: blur and downsample
    srcFloat.copyTo(blurred);
    for (int i = 0; i < blurLevel; i++) {
        pyrDown(blurred, blurred);
    }

    // apply temporal filter: subtraction of two IIR lowpass filters
    if (first) {
        first = false;
        blurred.copyTo(lowpassHigh);
        blurred.copyTo(lowpassLow);
        src.copyTo(out);
    } else {
        lowpassHigh = lowpassHigh * (1-fHigh) + fHigh * blurred;
        lowpassLow = lowpassLow * (1-fLow) + fLow * blurred;

        blurred = lowpassHigh - lowpassLow;

        // amplify
        blurred *= alpha;

        // resize back to original size
        for (int i = 0; i < blurLevel; i++) {
            pyrUp(blurred, blurred);
        }
        resize(blurred, blurred, srcSize);

        // add back to original frame
        blurred += srcFloat;

        // convert to 8 bit
        blurred.convertTo(out, CV_8U);
    }

    // detect face
    cvtColor(src, gray, CV_RGB2GRAY);
    classifier.detectMultiScale(src, faces, 1.1, 2, 0, minFaceSize);

    for (int i = 0; i < faces.size(); i++) {
        const Rect& face = faces.at(i);
        rectangle(out, face, BLUE, 2);
        point(out, face.tl() + Point(face.size().width * .5, face.size().height * 0.2));
    }

    t++;
}

void EvmGdownIIR::point(Mat& frame, const Point& p) {
    static vector<Vec3b> pulse;
    static Point g;

    pulse.push_back(frame.at<Vec3b>(p));

    int w = pulse.size() - frame.cols;
    for (int i = max(0, w); i < pulse.size(); i++) {
        g.x = i - w;
        g.y = pulse[i][1];
        line(frame, g, g, GREEN);
    }

    circle(frame, p, 2, RED, 4);
}
