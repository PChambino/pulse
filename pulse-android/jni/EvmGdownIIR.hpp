#ifndef EVMGDOWNIIR_HPP
#define	EVMGDOWNIIR_HPP

#include <opencv2/core/core.hpp>

using cv::Mat;
using cv::Size;

class EvmGdownIIR {
public:
    EvmGdownIIR();
    virtual ~EvmGdownIIR();

    void onFrame(const Mat& src, Mat& out);

    bool first;
    Size blurredSize;
    double fHigh;
    double fLow;
    int alpha;

private:
    Mat srcFloat;
    Mat blurred;
    Mat lowpassHigh;
    Mat lowpassLow;
    Mat outFloat;

};

#endif	/* EVMGDOWNIIR_HPP */

