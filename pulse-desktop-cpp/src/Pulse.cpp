#include "Pulse.hpp"
#include <sstream>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include "ext/opencv.hpp"
#include "profiler/Profiler.h"

using std::stringstream;
using namespace cv;

Pulse::Pulse() {
    maxSignalSize = 100;
    relativeMinFaceSize = 0.4;
    deleteFaceIn = 1;
    holdPulseFor = 30;
    fps = 0;
    evm.magnify = true;
    evm.alpha = 100;
}

Pulse::~Pulse() {
}

void Pulse::load(const string& filename) {
    classifier.load(filename);
}

void Pulse::start(int width, int height) {
    now = 0;
    lastFaceDetectionTimestamp = 0;
    lastBpmTimestamp = 0;
    minFaceSize = Size(min(width, height) * relativeMinFaceSize, min(width, height) * relativeMinFaceSize);
    faces.clear();
    nextFaceId = 1;
}

void Pulse::onFrame(Mat& frame) {
    PROFILE_SCOPED();

    // count frames
    now = getTickCount();

    // detect faces only every second
    if ((now - lastFaceDetectionTimestamp) * 1000. / getTickFrequency() >= 1000) {
        lastFaceDetectionTimestamp = getTickCount();

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
                        onFace(frame, face, face.box);
                    }
                }
            }
        }
    } else {
        PROFILE_SCOPED_DESC("previously detected faces");
        // use previously detected faces
        for (size_t i = 0; i < faces.size(); i++) {
            Face& face = faces.at(i);
            onFace(frame, face, face.box);
        }
    }
}

int Pulse::nearestFace(const Rect& box) {
    PROFILE_SCOPED();

    int index = -1;
    int min = -1;
    Point p;

    // search for first unselected face
    for (size_t i = 0; i < faces.size(); i++) {
        if (!faces.at(i).selected) {
            index = i;
            p = box.tl() - faces.at(i).box.tl();
            min = p.x * p.x + p.y * p.y;
            break;
        }
    }

    // no unselected face found
    if (index == -1) {
        return -1;
    }

    // compare with remaining unselected faces
    for (size_t i = index + 1; i < faces.size(); i++) {
        if (!faces.at(i).selected) {
            p = box.tl() - faces.at(i).box.tl();
            int d = p.x * p.x + p.y * p.y;
            if (d < min) {
                min = d;
                index = i;
            }
        }
    }

    return index;
}

void Pulse::onFace(Mat& frame, Face& face, const Rect& box) {
    PROFILE_SCOPED();

    // only show magnified face when there is pulse
    Mat roi = !evm.magnify || evm.magnify && face.existsPulse ?
        frame(face.evm.box) : face.evm.out;

    // if magnification is on
    if (evm.magnify) {
        if (face.evm.evm.first || face.evm.evm.alpha != evm.alpha) {
            // reset after changing magnify or alpha value
            face.reset();
        }

        // update face Eulerian video magnification values
        face.evm.evm.alpha = evm.alpha;

        // apply Eulerian video magnification on face box
        face.evm.evm.onFrame(frame(face.evm.box), roi);

    } else if (!face.evm.evm.first) {
        // reset after changing magnify value
        face.reset();
    }

    // capture raw value and timestamp, shifting if necessary
    if (face.raw.rows >= maxSignalSize) {
        PROFILE_SCOPED_DESC("shift raw and timestamp");
        const int total = face.raw.rows;
        face.raw.rowRange(1, total).copyTo(face.raw.rowRange(0, total - 1));
        face.raw.pop_back();
        face.timestamps.rowRange(1, total).copyTo(face.timestamps.rowRange(0, total - 1));
        face.timestamps.pop_back();
    }
    PROFILE_START_DESC("push back raw and timestamp");
    face.raw.push_back<double>(mean(roi)(1)); // grab green channel
    face.timestamps.push_back<double>(getTickCount());
    PROFILE_STOP();

    PROFILE_START_DESC("verify if raw signal is stable enough");
    Scalar rawStdDev;
    meanStdDev(face.raw, Scalar(), rawStdDev);
    const bool stable = rawStdDev(0) <= (evm.magnify ? 1 : 0) * evm.alpha * 0.045 + 1;
    PROFILE_STOP();

    if (stable) {
        // calculate current FPS
        currentFps = this->fps;
        if (currentFps == 0) {
            const double diff = (face.timestamps(face.timestamps.rows - 1) - face.timestamps(0)) * 1000. / getTickFrequency();
            currentFps = face.timestamps.rows * 1000 / diff;
        }

        // process raw signal
        detrend<double>(face.raw, face.pulse, currentFps / 2);
        normalization(face.pulse, face.pulse);
        meanFilter(face.pulse, face.pulse);

        // detects peaks and validates pulse
        peaks(face);
    } else {
        // too noisy to detect pulse
        face.existsPulse = false;

        // reset to improve BPM detection speed
        face.reset();
    }

    if (face.existsPulse) {
        bpm(face);
    }

    if (!face.existsPulse){
        PROFILE_SCOPED_DESC("no pulse");

        if (face.pulse.rows == face.raw.rows) {
            face.pulse = 0;
        } else {
            face.pulse = Mat1d::zeros(face.raw.rows, 1);
        }
        face.peaks.clear();
        face.bpms.pop_back(face.bpms.rows);
        face.bpm = 0;
    }

#ifndef __ANDROID__
    draw(frame, face, box);
#endif
}

// Algorithm based on: Pulse onset detection
void Pulse::peaks(Face& face) {
    PROFILE_SCOPED();

    // remove old peaks
    face.peaks.clear();

    int lastIndex = 0;
    int lastPeakIndex = 0;
    int lastPeakTimestamp = face.timestamps(0);
    int lastPeakValue = face.pulse(0);
    double peakValueThreshold = 0;

    for (int i = 1; i < face.raw.rows; i++) {

        // if more than X milliseconds passed search for peak in this segment
        const double diff = (face.timestamps(i) - face.timestamps(lastIndex)) * 1000. / getTickFrequency();
        if (diff >= 200) {

            // find max in this segment
            int relativePeakIndex[2];
            double peakValue;
            minMaxIdx(face.pulse.rowRange(lastIndex, i+1), 0, &peakValue, 0, &relativePeakIndex[0]);
            const int peakIndex = lastIndex + relativePeakIndex[0];

            // if max is not at the boundaries and is higher than the threshold
            if (peakValue > peakValueThreshold && lastIndex < peakIndex && peakIndex < i) {
                const double peakTimestamp = face.timestamps(peakIndex);
                const double peakDiff = (peakTimestamp - lastPeakTimestamp) * 1000. / getTickFrequency();

                // if peak is too close to last peak and has an higher value
                if (peakDiff <= 200 && peakValue > lastPeakValue) {
                    // then pop last peak
                    face.peaks.pop();
                }

                // if peak is far away enough from last peak or has an higher value
                if (peakDiff > 200 || peakValue > lastPeakValue) {
                    // then store current peak
                    face.peaks.push(peakIndex, peakTimestamp, peakValue);

                    lastPeakIndex = peakIndex;
                    lastPeakTimestamp = peakTimestamp;
                    lastPeakValue = peakValue;

                    peakValueThreshold = 0.6 * mean(face.peaks.values)(0);
                }
            }

            lastIndex = i;
        }
    }

    // verify if peaks describe a valid pulse signal
    Scalar peakValuesStdDev;
    meanStdDev(face.peaks.values, Scalar(), peakValuesStdDev);
    const double diff = (face.timestamps(face.raw.rows - 1) - face.timestamps(0)) / getTickFrequency();

    Scalar peakTimestampsStdDev;
    if (face.peaks.indices.rows >= 3) {
        meanStdDev((face.peaks.timestamps.rowRange(1, face.peaks.timestamps.rows) - face.peaks.timestamps.rowRange(0, face.peaks.timestamps.rows - 1)) / getTickFrequency(), Scalar(), peakTimestampsStdDev);
    }

    // TODO extract constants to class?
    bool validPulse =
            2 <= face.peaks.indices.rows &&
            40/60 * diff <= face.peaks.indices.rows &&
            face.peaks.indices.rows <= 240/60 * diff &&
            peakValuesStdDev(0) <= 0.5 &&
            peakTimestampsStdDev(0) <= 0.5;

    if (!face.existsPulse && validPulse) {
        // pulse become valid
        face.noPulseIn = holdPulseFor;
        face.existsPulse = true;
    } else if (face.existsPulse && !validPulse) {
        // pulse become invalid
        if (face.noPulseIn > 0) face.noPulseIn--; // keep pulse for a few frames
        else face.existsPulse = false; // pulse has been invalid for too long
    }
}

void Pulse::Face::Peaks::push(int index, double timestamp, double value) {
    indices.push_back<int>(index);
    timestamps.push_back<double>(timestamp);
    values.push_back<double>(value);
}

void Pulse::Face::Peaks::pop() {
    indices.pop_back(min(indices.rows, 1));
    timestamps.pop_back(min(timestamps.rows, 1));
    values.pop_back(min(values.rows, 1));
}

void Pulse::Face::Peaks::clear() {
    indices.pop_back(indices.rows);
    timestamps.pop_back(timestamps.rows);
    values.pop_back(values.rows);
}

void Pulse::bpm(Face& face) {
    PROFILE_SCOPED();

    dft(face.pulse, powerSpectrum);

    const int total = face.raw.rows;

    // band limit
    const int low = total * 40./60./currentFps + 1;
    const int high = total * 240./60./currentFps + 1;
    powerSpectrum.rowRange(0, min((size_t)low, (size_t)total)) = ZERO;
    powerSpectrum.pop_back(min((size_t)(total - high), (size_t)total));

    // power spectrum
    pow(powerSpectrum, 2, powerSpectrum);

    if (!powerSpectrum.empty()) {
        // grab index of max power spectrum
        int idx[2];
        minMaxIdx(powerSpectrum, 0, 0, 0, &idx[0]);

        // calculate BPM
        face.bpms.push_back<double>(idx[0] * currentFps * 30. / total); // constant 30 = 60 BPM / 2
    }

    // update BPM when none available or after one second
    if (face.bpm == 0 || (now - lastBpmTimestamp) * 1000. / getTickFrequency() >= 1000) {
        lastBpmTimestamp = getTickCount();

        // average calculated BPMs since last time
        face.bpm = mean(face.bpms)(0);
        face.bpms.pop_back(face.bpms.rows);

        // mark as no pulse when BPM is too low
        if (face.bpm <= 40) {
            face.existsPulse = false;
        }
    }
}

void Pulse::draw(Mat& frame, const Face& face, const Rect& box) {
    PROFILE_SCOPED();

    rectangle(frame, box, BLUE);
    rectangle(frame, face.box, BLUE, 2);
    rectangle(frame, face.evm.box, GREEN);

    // bottom left point of face box
    Point bl = face.box.tl() + Point(0, face.box.height);
    Point g;
    for (int i = 0; i < face.raw.rows; i++) {
        g = bl + Point(i, -face.raw(i) + 50);
        line(frame, g, g, GREEN);
        g = bl + Point(i, -face.pulse(i) * 10 - 50);
        line(frame, g, g, RED, face.existsPulse ? 2 : 1);
    }

    // peaks
    for (int i = 0; i < face.peaks.indices.rows; i++) {
        const int index = face.peaks.indices(i);
        g = bl + Point(index, -face.pulse(index) * 10 - 50);
        circle(frame, g, 1, BLUE, 2);
    }

    stringstream ss;

    // id
    ss << face.id;
    putText(frame, ss.str(), face.box.tl(), FONT_HERSHEY_PLAIN, 2, BLUE, 2);
    ss.str("");

    // bpm
    ss.precision(3);
    ss << face.bpm;
    putText(frame, ss.str(), bl, FONT_HERSHEY_PLAIN, 2, RED, 2);
}

Pulse::Face::Face(int id, const Rect& box, int deleteIn) {
    this->id = id;
    this->box = box;
    this->deleteIn = deleteIn;
    this->updateBox(this->box);
    this->existsPulse = false;
    this->noPulseIn = 0;
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

    // update box position and size
    Point p = box.tl() - a.tl();
    double d = (p.x * p.x + p.y * p.y) / pow(box.width / 3., 2.);
    interpolate(box, a, box, min(1., d));

    // update EVM box
    Point c = box.tl() + Point(box.size().width * .5, box.size().height * .5);
    Point r(box.width * .275, box.height * .425);
    evm.box = Rect(c - r, c + r);
}

void Pulse::Face::reset() {
    PROFILE_SCOPED();

    // restarts Eulerian video magnification
    evm.evm.first = true;

    // clear raw signal
    raw.pop_back(raw.rows);
    timestamps.pop_back(timestamps.rows);
}
