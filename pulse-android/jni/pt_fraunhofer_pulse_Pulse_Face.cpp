#include <pt_fraunhofer_pulse_Pulse_Face.h>
#include <vector>
#include <opencv2/core/core.hpp>

#include "Pulse.hpp"

#include <android/log.h>
#define LOG_TAG "Pulse::Pulse::Face"
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))

using std::vector;
using cv::Mat;
using cv::Mat1d;
using cv::Rect;

/*
 * Class:     pt_fraunhofer_pulse_Pulse_Face
 * Method:    _id
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_pt_fraunhofer_pulse_Pulse_00024Face__1id
  (JNIEnv *jenv, jclass, jlong self)
  {
    LOGD("Java_pt_fraunhofer_pulse_Pulse_00024Face__1id enter");
    jint id = 0;
    try
    {
        if (self)
            id = ((Pulse::Face*)self)->id;
    }
    catch(cv::Exception& e)
    {
        jclass je = jenv->FindClass("org/opencv/core/CvException");
        if(!je) je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, e.what());
    }
    catch (...)
    {
        jclass je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, "Unknown exception in JNI code.");
    }
    LOGD("Java_pt_fraunhofer_pulse_Pulse_00024Face__1id exit");
    return id;
  }

/*
 * Class:     pt_fraunhofer_pulse_Pulse_Face
 * Method:    _box
 * Signature: (JJ)V
 */
JNIEXPORT void JNICALL Java_pt_fraunhofer_pulse_Pulse_00024Face__1box
  (JNIEnv *jenv, jclass, jlong self, jlong mat)
  {
    LOGD("Java_pt_fraunhofer_pulse_Pulse_00024Face__1box enter");
    try
    {
        if (self) {
            vector<Rect> v;
            v.push_back(((Pulse::Face*)self)->evm.box);
            *((Mat*)mat) = Mat(v, true);
        }
    }
    catch(cv::Exception& e)
    {
        jclass je = jenv->FindClass("org/opencv/core/CvException");
        if(!je) je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, e.what());
    }
    catch (...)
    {
        jclass je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, "Unknown exception in JNI code.");
    }
    LOGD("Java_pt_fraunhofer_pulse_Pulse_00024Face__1box exit");
  }

/*
 * Class:     pt_fraunhofer_pulse_Pulse_Face
 * Method:    _bpm
 * Signature: (J)D
 */
JNIEXPORT jdouble JNICALL Java_pt_fraunhofer_pulse_Pulse_00024Face__1bpm
  (JNIEnv *jenv, jclass, jlong self)
  {
    LOGD("Java_pt_fraunhofer_pulse_Pulse_00024Face__1bpm enter");
    jdouble bpm = 0;
    try
    {
        if (self)
            bpm = ((Pulse::Face*)self)->bpm;
    }
    catch(cv::Exception& e)
    {
        jclass je = jenv->FindClass("org/opencv/core/CvException");
        if(!je) je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, e.what());
    }
    catch (...)
    {
        jclass je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, "Unknown exception in JNI code.");
    }
    LOGD("Java_pt_fraunhofer_pulse_Pulse_00024Face__1bpm exit");
    return bpm;
  }

/*
 * Class:     pt_fraunhofer_pulse_Pulse_Face
 * Method:    _pulse
 * Signature: (JJ)V
 */
JNIEXPORT void JNICALL Java_pt_fraunhofer_pulse_Pulse_00024Face__1pulse
  (JNIEnv *jenv, jclass, jlong self, jlong mat)
  {
    LOGD("Java_pt_fraunhofer_pulse_Pulse_00024Face__1pulse enter");
    try
    {
        if (self)
            *((Mat*)mat) = ((Pulse::Face*)self)->pulse;
    }
    catch(cv::Exception& e)
    {
        jclass je = jenv->FindClass("org/opencv/core/CvException");
        if(!je) je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, e.what());
    }
    catch (...)
    {
        jclass je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, "Unknown exception in JNI code.");
    }
    LOGD("Java_pt_fraunhofer_pulse_Pulse_00024Face__1pulse exit");
  }

/*
 * Class:     pt_fraunhofer_pulse_Pulse_Face
 * Method:    _existsPulse
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL Java_pt_fraunhofer_pulse_Pulse_00024Face__1existsPulse
  (JNIEnv *jenv, jclass, jlong self)
  {
    LOGD("Java_pt_fraunhofer_pulse_Pulse_00024Face__1existsPulse enter");
    jboolean result = false;
    try
    {
        if (self)
            result = ((Pulse::Face*)self)->existsPulse;
    }
    catch(cv::Exception& e)
    {
        jclass je = jenv->FindClass("org/opencv/core/CvException");
        if(!je) je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, e.what());
    }
    catch (...)
    {
        jclass je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, "Unknown exception in JNI code.");
    }
    LOGD("Java_pt_fraunhofer_pulse_Pulse_00024Face__1existsPulse exit");
    return result;
  }
