#include "EvmGdownIIR.h"
#include "Window.h"
#include "colors.h"

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
    classifier.detectMultiScale(src, faces, 1.1, 3, 0, minFaceSize);

    for (int i = 0; i < faces.size(); i++) {
        const Rect& face = faces.at(i);
        rectangle(out, face, BLUE, 2);
        point(out, face.tl() + Point(face.size().width * .5, face.size().height * 0.15));
    }

    t++;
}

template<typename T>
void printMat(const string& name, InputArray _a) {
    Mat a = _a.getMat();
    cout << name << ": " << a.rows << "x" << a.cols << "x" << a.channels() << endl;
    for (int y = 0; y < a.rows; y++) {
        cout << "[";
        for (int x = 0; x < a.cols; x++) {
            T* e = &a.at<T>(y, x);
            cout << "(" << e[0];
            for (int c = 1; c < a.channels(); c++) {
                cout << ", " << e[c];
            }
            cout << ")";
        }
        cout << "]" << endl;
    }
    cout << endl;
}

void detrend(InputArray _z, OutputArray _r, int lambda = 10) {
    CV_Assert((_z.type() == CV_32F || _z.type() == CV_64F)
            && _z.total() == max(_z.size().width, _z.size().height));

    Mat z = _z.getMat();
    if (z.total() < 3) {
        z.copyTo(_r);
    } else {
        int t = z.total();
        Mat i = Mat::eye(t, t, z.type());
        Mat d = _z.type() == CV_32F ? Mat(Matx13f(1, -2, 1)) : Mat(Matx13d(1, -2, 1));
        Mat d2Aux = Mat::ones(t-2, 1, z.type()) * d;
        Mat d2 = Mat::zeros(t-2, t, z.type());
        for (int k = 0; k < 3; k++) {
            d2Aux.col(k).copyTo(d2.diag(k));
        }
        Mat r = (i - (i + pow((double)lambda, 2) * d2.t() * d2).inv()) * z.t();
        r.copyTo(_r);
    }
}

void normalization(InputArray _a, OutputArray _b) {
    _a.getMat().copyTo(_b);
    Mat b = _b.getMat();
    Scalar mean, stdDev;
    meanStdDev(b, mean, stdDev);
    b = (b - mean[0]) / stdDev[0];
}

void meanFilter(InputArray _a, OutputArray _b, Size s = Size(5, 5)) {
    _a.getMat().copyTo(_b);
    Mat b = _b.getMat();
    for (int i = 0 ; i < 3; i++) {
        blur(b, b, s);
    }
}

template<typename T>
int countZeros(InputArray _a) {
    CV_Assert(_a.total() == max(_a.size().width, _a.size().height));

    Mat a = _a.getMat();
    int count = 0;
    for (int i = 1; i < a.total(); i++) {
        if ((a.at<T>(i-1) < 0 && a.at<T>(i) >= 0) || (a.at<T>(i-1) > 0 && a.at<T>(i) <= 0)) {
            count++;
        }
    }
    return count;
}

void EvmGdownIIR::point(Mat& frame, const Point& p) {
    // TODO extract static variables to class
    static vector<double> timestamps;
    static vector<double> green;
    static vector<double> detrended;
    
    static vector<double> frequency;
    static vector<double> amplitude;
    static vector<double> power;

    timestamps.push_back(getTickCount());
    green.push_back(frame.at<Vec3b>(p)[1]);
    if (green.size() > 100) { // TODO extract constant to class?
        timestamps.erase(timestamps.begin());
        green.erase(green.begin()); // TODO improve performance
    }
    detrend(green, detrended);
    normalization(detrended, detrended);
    meanFilter(detrended, detrended);

//    if (t % 30 == 0) { // TODO extract constant to class?
//        dft(green, amplitude);
//        amplitude.erase(amplitude.begin() + amplitude / 2, amplitude.end());
//        pow(amplitude, 2, power);
//        for (int i = 0; i < amplitude; i++) {
//            // merge frequency and power for sorting?
//        }
//        printMat<double>("dft", amplitude);
//    }

    Point g;
    int w = green.size() - frame.cols;
    for (int i = max(0, w); i < green.size(); i++) {
        g.x = i - w;
        g.y = green[i];
        line(frame, g, g, GREEN);
        g.y = detrended[i] * 10 + 100; // scaling
        line(frame, g, g, RED);
    }

    int count = countZeros<double>(detrended);
    double diff = (timestamps.back() - timestamps.front()) * 1000. / getTickFrequency();
    double bpm = count * 60000 / diff;
    stringstream ss;
    ss.precision(4);
    ss << bpm;
    putText(frame, ss.str(), Point(frame.cols - 50, 15), FONT_HERSHEY_PLAIN, 1, RED);
    circle(frame, p, 2, RED, 4);
}
