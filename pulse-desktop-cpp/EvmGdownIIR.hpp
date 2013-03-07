#ifndef EVMGDOWNIIR_HPP
#define	EVMGDOWNIIR_HPP

#include <string>
#include <vector>
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
    double minFaceSize;

    struct Face {
        int id;
        Rect box;
        Rect roi;
        Mat1d raw;
        Mat1d pulse;
        double bpm;

        void updateBox(const Rect& box);
    };

private:
    bool isFirstFrame() { return t == 1; }
    void onFace(Mat& frame, const Rect& face);

    uint64 t;
    Size srcSize;
    Size _minFaceSize;
    CascadeClassifier classifier;
    Mat gray;
    Mat srcFloat;
    Mat blurred;
    Mat lowpassHigh;
    Mat lowpassLow;
    Mat1d timestamps;
    Mat1d powerSpectrum;
    vector<Rect> _faces;
    Face face;

};

#endif	/* EVMGDOWNIIR_HPP */

