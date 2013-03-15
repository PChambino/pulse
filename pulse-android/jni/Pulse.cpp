#include "Pulse.hpp"
#include <sstream>
#include <opencv2/imgproc/imgproc.hpp>
#include "ext/opencv.hpp"

using std::stringstream;
using namespace cv;

Pulse::Pulse() {
    relativeMinFaceSize = 0.3;
    deleteFaceIn = 10;
    fps = 0;
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

void Pulse::onFrame(const Mat& src, Mat& out) {
    // count frames
    t++;

    // detect faces
    cvtColor(src, gray, CV_RGB2GRAY);
    classifier.detectMultiScale(src, boxes, 1.1, 3, 0, minFaceSize);

    // iterate through faces and boxes
    if (faces.size() <= boxes.size()) {
        // match each face to nearest box
        for (int i = 0; i < faces.size(); i++) {
            Face& face = faces.at(i);
            int boxIndex = face.nearestBox(boxes);
            face.deleteIn = deleteFaceIn;
            face.updateBox(boxes.at(boxIndex));
            onFace(out, face, boxes.at(boxIndex));
            boxes.erase(boxes.begin() + boxIndex);
        }
        // remaining boxes are new faces
        for (int i = 0; i < boxes.size(); i++) {
            faces.push_back(Face(nextFaceId++, boxes.at(i), deleteFaceIn));
            onFace(out, faces.back(), boxes.at(i));
        }
    } else {
        // match each box to nearest face
        for (int i = 0; i < faces.size(); i++) {
            faces.at(i).selected = false;
        }
        for (int i = 0; i < boxes.size(); i++) {
            int faceIndex = nearestFace(boxes.at(i));
            Face& face = faces.at(faceIndex);
            face.selected = true;
            face.deleteIn = deleteFaceIn;
            face.updateBox(boxes.at(i));
            onFace(out, face, boxes.at(i));
        }
        // remaining faces are deleted or marked for deletion
        for (int i = 0; i < faces.size(); i++) {
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
    resize(frame(face.roi), face.roiMat, face.evmSize);
    face.evm.onFrame(face.roiMat, face.roiMat);

    const int total = face.raw.total();
    if (total >= 100) { // TODO extract constant to class?
        face.raw.rowRange(1, total).copyTo(face.raw.rowRange(0, total-1));
        face.raw.pop_back();
        face.timestamps.rowRange(1, total).copyTo(face.timestamps.rowRange(0, total-1));
        face.timestamps.pop_back();
    }
    face.raw.push_back<double>(mean(face.roiMat)[1]);
    face.timestamps.push_back<double>(getTickCount());

    detrend(face.raw, face.pulse);
    normalization(face.pulse, face.pulse);
    meanFilter(face.pulse, face.pulse);

    if (t % 30 == 0) { // TODO extract constant to class?
        double fps = this->fps;
        if (fps == 0) {
            const double diff = (face.timestamps(total-1) - face.timestamps(0)) * 1000. / getTickFrequency();
            fps = total * 1000 / diff;
        }
        // TODO extract constants to class?
        const int low = total * 45./60./fps + 1;
        const int high = total * 240./60./fps + 1;

        dft(face.pulse, powerSpectrum);

        // band limit
        powerSpectrum.rowRange(0, low) = ZERO;
        powerSpectrum.pop_back(min((size_t)(total - high), (size_t)total));

        // power spectrum
        pow(powerSpectrum, 2, powerSpectrum);

        if (!powerSpectrum.empty()) {
            // grab index of max power spectrum
            int idx[2];
            minMaxIdx(powerSpectrum, 0, 0, 0, &idx[0]);

            // calculate BPM
            const double bpm = idx[0] * fps * 60. / total;
            face.bpm = bpm;
        }
    }

    // draw some stuff
    resize(face.roiMat, face.roiMat, face.roi.size());
    face.roiMat.copyTo(frame(face.roi));

    rectangle(frame, box, BLUE);
    rectangle(frame, face.box, BLUE, 2);
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
}

int Pulse::nearestFace(const Rect& box) {
    int index = -1;
    int min = -1;
    Point p;
    for (int i = 0; i < faces.size(); i++) {
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
    for (int i = index; i < faces.size(); i++) {
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
    this->evmSize = this->roi.size();
}

int Pulse::Face::nearestBox(const vector<Rect>& boxes) {
    if (boxes.empty()) {
        return -1;
    }
    int index = 0;
    Point p = box.tl() - boxes.at(0).tl();
    int min = p.x * p.x + p.y * p.y;
    for (int i = 1; i < boxes.size(); i++) {
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
    interpolate(box, a, box, 0.05);
    Point c = box.tl() + Point(box.size().width * .5, box.size().height * 0.15);
    Point r(box.width * 0.10, box.height * 0.05);
    roi = Rect(c - r, c + r);
}
