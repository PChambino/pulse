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
using cv::Mat1i;
using cv::Rect;
using cv::Size;
using cv::CascadeClassifier;

class Pulse {
public:
    Pulse();
    virtual ~Pulse();

    void load(const string& filename);
    void start(int width, int height);
    void onFrame(Mat& frame);

    int maxSignalSize;
    double relativeMinFaceSize;
    double fps;
    struct {
        bool magnify;
        double alpha;
    } evm;

    struct Face {
        int id;
        int deleteIn;
        bool selected;

        Rect box;
        Mat1d timestamps;
        Mat1d raw;
        Mat1d pulse;
        int noPulseIn;
        bool existsPulse;

        Mat1d bpms;
        double bpm;

        struct {
            EvmGdownIIR evm;
            Mat out;
            Rect box;
        } evm;

        struct Peaks {
            Mat1i indices;
            Mat1d timestamps;
            Mat1d values;

            void push(int index, double timestamp, double value);
            void pop();
            void clear();
        } peaks;

        Face(int id, const Rect& box, int deleteIn);
        int nearestBox(const vector<Rect>& boxes);
        void updateBox(const Rect& box);
        void reset();
    };

    vector<Face> faces;

private:
    int nearestFace(const Rect& box);
    void onFace(Mat& frame, Face& face, const Rect& box);
    void peaks(Face& face);
    void bpm(Face& face);
    void draw(Mat& frame, const Face& face, const Rect& box);

    double now;
    double lastFaceDetectionTimestamp;
    double lastBpmTimestamp;
    Size minFaceSize;
    CascadeClassifier classifier;
    Mat gray;
    vector<Rect> boxes;
    Mat1d powerSpectrum;
    int nextFaceId;
    int deleteFaceIn;
    int holdPulseFor;
    double currentFps;

};

#endif	/* PULSE_HPP */

