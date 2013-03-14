#include <pt_fraunhofer_pulse_Pulse.h>
#include <string>
#include <opencv2/core/core.hpp>

#include "pulse.hpp"

#include <android/log.h>
#define LOG_TAG "Pulse::Pulse"
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))

using std::string;
using cv::Mat;

/*
 * Class:     pt_fraunhofer_pulse_Pulse
 * Method:    _initialize
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_pt_fraunhofer_pulse_Pulse__1initialize
  (JNIEnv *jenv, jclass)
  {
    LOGD("Java_pt_fraunhofer_pulse_Pulse__1initialize enter");
    jlong result = 0;
    try
    {
        result = (jlong)new Pulse();
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
    LOGD("Java_pt_fraunhofer_pulse_Pulse__1initialize exit");
    return result;
  }

/*
 * Class:     pt_fraunhofer_pulse_Pulse
 * Method:    _load
 * Signature: (JLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_pt_fraunhofer_pulse_Pulse__1load
  (JNIEnv *jenv, jclass, jlong self, jstring jfilename)
  {
    LOGD("Java_pt_fraunhofer_pulse_Pulse__1load enter");
    string filename(jenv->GetStringUTFChars(jfilename, NULL));
    try
    {
        if (self)
            ((Pulse*)self)->load(filename);
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
    LOGD("Java_pt_fraunhofer_pulse_Pulse__1load exit");
  }

/*
 * Class:     pt_fraunhofer_pulse_Pulse
 * Method:    _start
 * Signature: (JII)V
 */
JNIEXPORT void JNICALL Java_pt_fraunhofer_pulse_Pulse__1start
  (JNIEnv *jenv, jclass, jlong self, jint width, jint height)
  {
    LOGD("Java_pt_fraunhofer_pulse_Pulse__1start enter");
    try
    {
        if (self)
            ((Pulse*)self)->start(width, height);
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
    LOGD("Java_pt_fraunhofer_pulse_Pulse__1start exit");
  }

/*
 * Class:     pt_fraunhofer_pulse_Pulse
 * Method:    _onFrame
 * Signature: (JJJ)V
 */
JNIEXPORT void JNICALL Java_pt_fraunhofer_pulse_Pulse__1onFrame
  (JNIEnv *jenv, jclass, jlong self, jlong frame, jlong out)
  {
    LOGD("Java_pt_fraunhofer_pulse_Pulse__1onFrame enter");
    try
    {
        if (self)
            ((Pulse*)self)->onFrame(*((Mat*)frame), *((Mat*)out));
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
    LOGD("Java_pt_fraunhofer_pulse_Pulse__1onFrame exit");
  }

/*
 * Class:     pt_fraunhofer_pulse_Pulse
 * Method:    _destroy
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_pt_fraunhofer_pulse_Pulse__1destroy
  (JNIEnv *jenv, jclass, jlong self)
  {
    LOGD("Java_pt_fraunhofer_pulse_Pulse__1destroy enter");
    try
    {
        if (self)
            delete (Pulse*)self;
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
    LOGD("Java_pt_fraunhofer_pulse_Pulse__1destroy exit");
  }
