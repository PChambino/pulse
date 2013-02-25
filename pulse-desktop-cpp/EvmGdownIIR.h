#ifndef EVMGDOWNIIR_H
#define	EVMGDOWNIIR_H

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class EvmGdownIIR {
public:
    EvmGdownIIR();
    virtual ~EvmGdownIIR();

    void load(const string& filename);
    void start(int width, int height);
    void onFrame(const Mat& src, Mat& out);

    int blurLevel;
    double fHigh;
    double fLow;
    int alpha;
    double minFaceSizePercentage;

private:
    bool first;
    Size srcSize;
    Mat srcFloat;
    Mat blurred;
    Mat lowpassHigh;
    Mat lowpassLow;
    Mat gray;
    CascadeClassifier classifier;
    Size minFaceSize;
    vector<Rect> faces;

};

#endif	/* EVMGDOWNIIR_H */

