#include "EvmGdownIIR.h"
#include "Window.h"
#include "ext_opencv.h"

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
    // count frames
    t++;

    // detect faces
    cvtColor(src, gray, CV_RGB2GRAY);
    classifier.detectMultiScale(src, faces, 1.1, 3, 0, minFaceSize);

    // convert to float
    src.convertTo(srcFloat, CV_32F);

    // apply spatial filter: blur and downsample
    srcFloat.copyTo(blurred);
    for (int i = 0; i < blurLevel; i++) {
        pyrDown(blurred, blurred);
    }

    if (first) {
        first = false;
        blurred.copyTo(lowpassHigh);
        blurred.copyTo(lowpassLow);
        src.copyTo(out);
    } else {
        // apply temporal filter: subtraction of two IIR lowpass filters
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

    // iterate through faces
    for (int i = 0; i < faces.size(); i++) {
        rectangle(out, faces.at(i), BLUE);
        stringstream ss;
        ss << i;
        putText(out, ss.str(), faces.at(i).tl() + Point(5, 15), FONT_HERSHEY_PLAIN, 1, BLUE);
    }
    // TODO support multiple faces
    face(out, faces.front());
}

void EvmGdownIIR::face(Mat& frame, const Rect& face) {
    // TODO extract static variables to class
    static Rect f = face;
    static Rect roi;
    static Mat1d timestamps;
    static Mat1d green;
    static Mat1d detrended;
    static Mat1d powerSpectrum;
    static Mat1d pulse;
    static string bpmStr;

    {
        interpolate(f, face, f, 0.05);
        Point p = f.tl() + Point(f.size().width * .5, f.size().height * 0.15);
        Point s(f.width * 0.05, f.height * 0.025);
        roi = Rect(p - s, p + s);
    }

    const int total = green.total();
    if (total >= 100) { // TODO extract constant to class?
        timestamps.rowRange(1, total).copyTo(timestamps.rowRange(0, total-1));
        green.rowRange(1, total).copyTo(green.rowRange(0, total-1));
        timestamps.pop_back();
        green.pop_back();
    }
    timestamps.push_back<double>(getTickCount());
    green.push_back<double>(mean(frame(roi))[1]);

    detrend(green, detrended);
    normalization(detrended, detrended);
    meanFilter(detrended, detrended);

    if (t % 30 == 0) { // TODO extract constant to class?
        const double diff = (timestamps(total-1) - timestamps(0)) * 1000. / getTickFrequency();
        const double fps = total * 1000 / diff;
        // TODO extract constants to class?
        const int low = total * 45./60./fps + 1;
        const int high = total * 240./60./fps + 1;

        dft(green, powerSpectrum);

        // band limit
        powerSpectrum.rowRange(0, low) = ZERO;
        powerSpectrum.pop_back(min(total - high, total));

        // power spectrum
        pow(powerSpectrum, 2, powerSpectrum);

        if (!powerSpectrum.empty()) {
            // grab index of max power spectrum
            int idx[2];
            minMaxIdx(powerSpectrum, 0, 0, 0, &idx[0]);

            // calculate BPM
            const double bpm = idx[0] * fps * 60. / total;
            pulse.push_back(bpm);

            stringstream ss;
            ss.precision(3);
            ss << "BPM:  " << bpm;
            bpmStr = ss.str();
        }
    }

    // draw some stuff
    rectangle(frame, f, BLUE, 2);
    rectangle(frame, roi, RED);

    Point bl = f.tl() + Point(0, f.height);
    Point g;
    for (int i = 0; i < total; i++) {
        g = bl + Point(i, -green(i) + 50);
        line(frame, g, g, GREEN);
        g = bl + Point(i, -detrended(i) * 10 - 50);
        line(frame, g, g, RED);
    }

    putText(frame, bpmStr, bl, FONT_HERSHEY_PLAIN, 1, RED);
}
