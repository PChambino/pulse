#ifndef PULSE_HPP
#define	PULSE_HPP

#include <string>
#include <vector>
#include <opencv2/objdetect/objdetect.hpp>
#include "EvmGdownIIR.hpp"

using std::string;
using std::vector;
using cv::Mat;
using cv::Mat1d;
using cv::Rect;
using cv::Size;
using cv::CascadeClassifier;

class Pulse {
public:
    Pulse();
    virtual ~Pulse();
    
    void load(const string& filename);
    void start(int width, int height);
    void onFrame(const Mat& src, Mat& out);

    double relativeMinFaceSize;
    double fps;
    EvmGdownIIR evm;

    struct Face {
        int id;
        int deleteIn;
        bool selected;
        Rect box;
        Rect roi;
        Mat1d timestamps;
        Mat1d raw;
        Mat1d pulse;
        double bpm;

        Face(int id, const Rect& box, int deleteIn);
        int nearestBox(const vector<Rect>& boxes);
        void updateBox(const Rect& box);
    };
    
private:
    void onFace(Mat& frame, Face& face, const Rect& box);
    int nearestFace(const Rect& box);
    
    uint64 t;
    Size minFaceSize;
    CascadeClassifier classifier;
    Mat gray;
    vector<Rect> boxes;
    Mat1d powerSpectrum;
    vector<Face> faces;
    int nextFaceId;
    int deleteFaceIn;


};

#endif	/* PULSE_HPP */

