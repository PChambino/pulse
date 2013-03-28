#include "Pulse.hpp"
#include <sstream>
#include <opencv2/imgproc/imgproc.hpp>
#include "ext/opencv.hpp"
#include "profiler/Profiler.h"

using std::stringstream;
using namespace cv;

Pulse::Pulse() {
    relativeMinFaceSize = 0.3;
    deleteFaceIn = 10;
    fps = 0;
    magnify = true;
}

Pulse::~Pulse() {
}

void Pulse::load(const string& filename) {
    classifier.load(filename);
}

void Pulse::start(int width, int height) {
    t = 0;
    minFaceSize = Size(width * relativeMinFaceSize, height * relativeMinFaceSize);
    faces.clear();
    nextFaceId = 1;
}

void Pulse::onFrame(Mat& frame) {
    PROFILE_SCOPED();
    
    // count frames
    t++;

    PROFILE_START_DESC("detect faces");
    // detect faces
    cvtColor(frame, gray, CV_RGB2GRAY);
    classifier.detectMultiScale(frame, boxes, 1.1, 3, 0, minFaceSize);
    PROFILE_STOP();

    // iterate through faces and boxes
    if (faces.size() <= boxes.size()) {
        PROFILE_SCOPED_DESC("equal or more boxes than faces");
        // match each face to nearest box
        for (size_t i = 0; i < faces.size(); i++) {
            Face& face = faces.at(i);
            int boxIndex = face.nearestBox(boxes);
            face.deleteIn = deleteFaceIn;
            face.updateBox(boxes.at(boxIndex));
            onFace(frame, face, boxes.at(boxIndex));
            boxes.erase(boxes.begin() + boxIndex);
        }
        // remaining boxes are new faces
        for (size_t i = 0; i < boxes.size(); i++) {
            faces.push_back(Face(nextFaceId++, boxes.at(i), deleteFaceIn));
            onFace(frame, faces.back(), boxes.at(i));
        }
    } else {
        PROFILE_SCOPED_DESC("more faces than boxes");
        // match each box to nearest face
        for (size_t i = 0; i < faces.size(); i++) {
            faces.at(i).selected = false;
        }
        for (size_t i = 0; i < boxes.size(); i++) {
            int faceIndex = nearestFace(boxes.at(i));
            Face& face = faces.at(faceIndex);
            face.selected = true;
            face.deleteIn = deleteFaceIn;
            face.updateBox(boxes.at(i));
            onFace(frame, face, boxes.at(i));
        }
        // remaining faces are deleted or marked for deletion
        for (size_t i = 0; i < faces.size(); i++) {
            Face& face = faces.at(i);
            if (!face.selected) {
                if (face.deleteIn <= 0) {
                    faces.erase(faces.begin() + i);
                    i--;
                } else {
                    face.deleteIn--;
                }
            }
        }
    }
}

void Pulse::onFace(Mat& frame, Face& face, const Rect& box) {
    PROFILE_SCOPED();
    
    // apply Eulerian video magnification on face box
    if (magnify) {
        PROFILE_START_DESC("resize face box");
        resize(frame(face.evmBox), face.evmMat, face.evmSize, 0, 0, CV_INTER_NN);
        PROFILE_STOP();
        face.evm.onFrame(face.evmMat, face.evmMat);
        PROFILE_START_DESC("resize and draw face box back to frame");
        resize(face.evmMat, face.evmMat, face.evmBox.size(), 0, 0, CV_INTER_NN);
        face.evmMat.copyTo(frame(face.evmBox));
        PROFILE_STOP();
    }

    const int total = face.raw.total();
    if (total >= 100) { // TODO extract constant to class?
        PROFILE_SCOPED_DESC("shift raw and timestamp");
        face.raw.rowRange(1, total).copyTo(face.raw.rowRange(0, total-1));
        face.raw.pop_back();
        face.timestamps.rowRange(1, total).copyTo(face.timestamps.rowRange(0, total-1));
        face.timestamps.pop_back();
    }
    PROFILE_START_DESC("push back raw and timestamp");
    face.raw.push_back<double>(mean(frame(face.roi))[1]);
    face.timestamps.push_back<double>(getTickCount());
    PROFILE_STOP();

    detrend(face.raw, face.pulse);
    normalization(face.pulse, face.pulse);
    meanFilter(face.pulse, face.pulse);

    if (t % 30 == 0) { // TODO extract constant to class?
        PROFILE_SCOPED_DESC("bpm");
        
        double fps = this->fps;
        if (fps == 0) {
            const double diff = (face.timestamps(total-1) - face.timestamps(0)) * 1000. / getTickFrequency();
            fps = total * 1000 / diff;
        }
        // TODO extract constants to class?
        const int low = total * 40./60./fps + 1;
        const int high = total * 240./60./fps + 1;

        dft(face.pulse, powerSpectrum);

        // band limit
        powerSpectrum.rowRange(0, min((size_t)low, (size_t)total)) = ZERO;
        powerSpectrum.pop_back(min((size_t)(total - high), (size_t)total));

        // power spectrum
        pow(powerSpectrum, 2, powerSpectrum);

        if (!powerSpectrum.empty()) {
            // grab index of max power spectrum
            int idx[2];
            minMaxIdx(powerSpectrum, 0, 0, 0, &idx[0]);

            // calculate BPM
            face.bpm = idx[0] * fps * 60. / (2 * total);
            face.bpms.push_back(face.bpm);
        }
    }

    // draw some stuff
    PROFILE_START_DESC("drawing");
    rectangle(frame, box, BLUE);
    rectangle(frame, face.box, BLUE, 2);
    rectangle(frame, face.evmBox, GREEN);
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
    ss << face.id;
    putText(frame, ss.str(), face.box.tl(), FONT_HERSHEY_PLAIN, 2, BLUE, 2);
    ss.str("");
    
    ss.precision(3);
    ss << face.bpm;
    putText(frame, ss.str(), bl, FONT_HERSHEY_PLAIN, 2, RED, 2);
    PROFILE_STOP();
}

int Pulse::nearestFace(const Rect& box) {
    PROFILE_SCOPED();
    
    int index = -1;
    int min = -1;
    Point p;
    for (size_t i = 0; i < faces.size(); i++) {
        if (!faces.at(i).selected) {
            index = i;
            p = box.tl() - faces.at(i).box.tl();
            min = p.x * p.x + p.y * p.y;
            break;
        }
    }
    if (index == -1) {
        return -1;
    }
    for (size_t i = index; i < faces.size(); i++) {
        p = box.tl() - faces.at(i).box.tl();
        int d = p.x * p.x + p.y * p.y;
        if (d < min) {
            min = d;
            index = i;
        }
    }
    return index;
}

Pulse::Face::Face(int id, const Rect& box, int deleteIn) {
    this->id = id;
    this->box = box;
    this->deleteIn = deleteIn;
    this->updateBox(this->box);
    this->evmSize = this->evmBox.size();
    this->bpm = 0;
}

Pulse::Face::~Face() {
    if (!bpms.empty()) {
        cout << "Face " << id << " mean(bpms) = " << mean(bpms)[0] << endl;
    }
}

int Pulse::Face::nearestBox(const vector<Rect>& boxes) {
    PROFILE_SCOPED();
    
    if (boxes.empty()) {
        return -1;
    }
    int index = 0;
    Point p = box.tl() - boxes.at(0).tl();
    int min = p.x * p.x + p.y * p.y;
    for (size_t i = 1; i < boxes.size(); i++) {
        p = box.tl() - boxes.at(i).tl();
        int d = p.x * p.x + p.y * p.y;
        if (d < min) {
            min = d;
            index = i;
        }
    }
    return index;
}

void Pulse::Face::updateBox(const Rect& a) {
    PROFILE_SCOPED();
    
    interpolate(box, a, box, 0.05);

    Point c = box.tl() + Point(box.size().width * .5, box.size().height * 0.5);
    Point r(box.width * 0.35, box.height * 0.45);
    evmBox = Rect(c - r, c + r);

    c = box.tl() + Point(box.size().width * .5, box.size().height * 0.55);
    r = Point(box.width * 0.3, box.height * 0.075);
    roi = Rect(c - r, c + r);
}
