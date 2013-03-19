#include "opencv.hpp"
#include <opencv2/imgproc/imgproc.hpp>

namespace cv {

void detrend(InputArray _z, OutputArray _r, int lambda) {
    CV_DbgAssert((_z.type() == CV_32F || _z.type() == CV_64F)
            && _z.total() == max(_z.size().width, _z.size().height));


    Mat z = _z.total() == (size_t)_z.size().height ? _z.getMat() : _z.getMat().t();
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
        Mat r = (i - (i + pow((double)lambda, 2) * d2.t() * d2).inv()) * z;
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

void meanFilter(InputArray _a, OutputArray _b, Size s) {
    _a.getMat().copyTo(_b);
    Mat b = _b.getMat();
    for (int i = 0 ; i < 3; i++) {
        blur(b, b, s);
    }
}

void interpolate(const Rect& a, const Rect& b, Rect& c, double p) {
    double np = 1 - p;
    c.x = a.x * np + b.x * p + 0.5;
    c.y = a.y * np + b.y * p + 0.5;
    c.width = a.width * np + b.width * p + 0.5;
    c.height = a.height * np + b.height * p + 0.5;
}

void printMatInfo(const string& name, InputArray _a) {
    Mat a = _a.getMat();
    cout << name << ": " << a.rows << "x" << a.cols
            << "x" << a.channels() << "x" << a.depth()
            << " isContinuous=" << (a.isContinuous() ? "true" : "false")
            << " isSubmatrix=" << (a.isSubmatrix() ? "true" : "false") << endl;
}

}

