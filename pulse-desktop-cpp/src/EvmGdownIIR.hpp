#ifndef EVMGDOWNIIR_HPP
#define	EVMGDOWNIIR_HPP

#include <opencv2/core/core.hpp>

using cv::Mat;

class EvmGdownIIR {
public:
    EvmGdownIIR();
    virtual ~EvmGdownIIR();

    void onFrame(const Mat& src, Mat& out);

    bool first;
    int blurLevel;
    double fHigh;
    double fLow;
    int alpha;

private:
    Mat srcFloat;
    Mat blurred;
    Mat lowpassHigh;
    Mat lowpassLow;

};

#endif	/* EVMGDOWNIIR_HPP */

