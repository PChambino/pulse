#ifndef COLORS_H
#define	COLORS_H

#include <opencv2/opencv.hpp>

using namespace std;

namespace cv {

const Scalar BLACK    (  0,   0,   0);
const Scalar RED      (  0,   0, 255);
const Scalar GREEN    (  0, 255,   0);
const Scalar BLUE     (255,   0,   0);
const Scalar WHITE    (255, 255, 255);

const Scalar ZERO     (0);

inline void printMatInfo(const string& name, InputArray _a) {
    Mat a = _a.getMat();
    cout << name << ": " << a.rows << "x" << a.cols
            << "x" << a.channels() << "x" << a.depth()
            << " isContinuous=" << (a.isContinuous() ? "true" : "false")
            << " isSubmatrix=" << (a.isSubmatrix() ? "true" : "false") << endl;
}

template<typename T>
void printMat(const string& name, InputArray _a,
        int rows = -1,
        int cols = -1,
        int channels = -1)
{
    printMatInfo(name, _a);

    Mat a = _a.getMat();
    if (-1 == rows) rows = a.rows;
    if (-1 == cols) cols = a.cols;
    if (-1 == channels) channels = a.channels();

    for (int y = 0; y < rows; y++) {
        cout << "[";
        for (int x = 0; x < cols; x++) {
            T* e = &a.at<T>(y, x);
            cout << "(" << e[0];
            for (int c = 1; c < channels; c++) {
                cout << ", " << e[c];
            }
            cout << ")";
        }
        cout << "]" << endl;
    }
    cout << endl;
}

inline void detrend(InputArray _z, OutputArray _r, int lambda = 10) {
    CV_Assert((_z.type() == CV_32F || _z.type() == CV_64F)
            && _z.total() == max(_z.size().width, _z.size().height));


    Mat z = _z.total() == _z.size().height ? _z.getMat() : _z.getMat().t();
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

inline void normalization(InputArray _a, OutputArray _b) {
    _a.getMat().copyTo(_b);
    Mat b = _b.getMat();
    Scalar mean, stdDev;
    meanStdDev(b, mean, stdDev);
    b = (b - mean[0]) / stdDev[0];
}

inline void meanFilter(InputArray _a, OutputArray _b, Size s = Size(5, 5)) {
    _a.getMat().copyTo(_b);
    Mat b = _b.getMat();
    for (int i = 0 ; i < 3; i++) {
        blur(b, b, s);
    }
}

template<typename T>
int countZeros(InputArray _a) {
    CV_Assert(_a.channels() == 1
            && _a.total() == max(_a.size().width, _a.size().height));

    int count = 0;
    if (_a.total() > 0) {
        Mat a = _a.getMat();
        T last = a.at<T>(0);
        for (int i = 1; i < a.total(); i++) {
            T current = a.at<T>(i);
            if ((last < 0 && current >= 0) || (last > 0 && current <= 0)) {
                count++;
            }
            last = current;
        }
    }
    return count;
}

}

#endif	/* COLORS_H */

