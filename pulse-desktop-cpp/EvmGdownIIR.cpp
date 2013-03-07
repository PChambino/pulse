#include "EvmGdownIIR.hpp"
#include "Window.hpp"
#include "ext_opencv.hpp"

EvmGdownIIR::EvmGdownIIR() {
    t = 0;
    blurLevel = 4;
    fLow = 50/60./10;
    fHigh = 60/60./10;
    alpha = 200;
    minFaceSize = 0.3;
}

EvmGdownIIR::~EvmGdownIIR() {
}

void EvmGdownIIR::load(const string& filename) {
    classifier.load(filename);
}

void EvmGdownIIR::start(int width, int height) {
    t = 0;
    srcSize = Size(width, height);
    _minFaceSize = Size(width * minFaceSize, height * minFaceSize);
}

void EvmGdownIIR::onFrame(const Mat& src, Mat& out) {
    // count frames
    t++;

    // detect faces
    cvtColor(src, gray, CV_RGB2GRAY);
    classifier.detectMultiScale(src, _faces, 1.1, 3, 0, _minFaceSize);

    // convert to float
    src.convertTo(srcFloat, CV_32F);

    // apply spatial filter: blur and downsample
    srcFloat.copyTo(blurred);
    for (int i = 0; i < blurLevel; i++) {
        pyrDown(blurred, blurred);
    }

    if (isFirstFrame()) {
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
    for (int i = 0; i < _faces.size(); i++) {
        rectangle(out, _faces.at(i), BLUE);
    }
    // TODO support multiple faces
    if (!_faces.empty()) {
        onFace(out, _faces.front());
    }
}

void EvmGdownIIR::onFace(Mat& frame, const Rect& box) {
    if (isFirstFrame()) {
        face.box = box;
    }
    face.updateBox(box);

    const int total = face.raw.total();
    if (total >= 100) { // TODO extract constant to class?
        timestamps.rowRange(1, total).copyTo(timestamps.rowRange(0, total-1));
        face.raw.rowRange(1, total).copyTo(face.raw.rowRange(0, total-1));
        timestamps.pop_back();
        face.raw.pop_back();
    }
    timestamps.push_back<double>(getTickCount());
    face.raw.push_back<double>(mean(frame(face.roi))[1]);

    detrend(face.raw, face.pulse);
    normalization(face.pulse, face.pulse);
    meanFilter(face.pulse, face.pulse);

    if (t % 30 == 0) { // TODO extract constant to class?
        const double diff = (timestamps(total-1) - timestamps(0)) * 1000. / getTickFrequency();
        const double fps = total * 1000 / diff;
        // TODO extract constants to class?
        const int low = total * 45./60./fps + 1;
        const int high = total * 240./60./fps + 1;

        dft(face.pulse, powerSpectrum);

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
            face.bpm = bpm;
        }
    }

    // draw some stuff
    rectangle(frame, face.box, BLUE, 2);
    rectangle(frame, face.roi, RED);

    Point bl = face.box.tl() + Point(0, face.box.height);
    Point g;
    for (int i = 0; i < total; i++) {
        g = bl + Point(i, -face.raw(i) + 50);
        line(frame, g, g, GREEN);
        g = bl + Point(i, -face.pulse(i) * 10 - 50);
        line(frame, g, g, RED);
    }

    stringstream ss;
    ss.precision(3);
    ss << face.bpm;
    putText(frame, ss.str(), bl, FONT_HERSHEY_PLAIN, 1, RED);
}

void EvmGdownIIR::Face::updateBox(const Rect& _box) {
    interpolate(box, _box, box, 0.05);
    Point c = box.tl() + Point(box.size().width * .5, box.size().height * 0.15);
    Point r(box.width * 0.05, box.height * 0.025);
    roi = Rect(c - r, c + r);
}
