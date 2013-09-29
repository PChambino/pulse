#include <pt_fraunhofer_pulse_Pulse.h>
#include <string>
#include <opencv2/core/core.hpp>

#include "Pulse.hpp"

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
 * Signature: (JJ)V
 */
JNIEXPORT void JNICALL Java_pt_fraunhofer_pulse_Pulse__1onFrame
  (JNIEnv *jenv, jclass, jlong self, jlong frame)
  {
    LOGD("Java_pt_fraunhofer_pulse_Pulse__1onFrame enter");
    try
    {
        if (self) {
            ((Pulse*)self)->onFrame(*((Mat*)frame));
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
    LOGD("Java_pt_fraunhofer_pulse_Pulse__1onFrame exit");
  }

/*
 * Class:     pt_fraunhofer_pulse_Pulse
 * Method:    _facesCount
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_pt_fraunhofer_pulse_Pulse__1facesCount
  (JNIEnv *jenv, jclass, jlong self)
  {
    LOGD("Java_pt_fraunhofer_pulse_Pulse__1facesCount enter");
    try
    {
        if (self)
            return ((Pulse*)self)->faces.size();
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
    LOGD("Java_pt_fraunhofer_pulse_Pulse__1facesCount exit");
    return 0;
  }

/*
 * Class:     pt_fraunhofer_pulse_Pulse
 * Method:    _face
 * Signature: (JI)J
 */
JNIEXPORT jlong JNICALL Java_pt_fraunhofer_pulse_Pulse__1face
  (JNIEnv *jenv, jclass, jlong self, jint i)
  {
    LOGD("Java_pt_fraunhofer_pulse_Pulse__1face enter");
    try
    {
        if (self)
            return (jlong)(&((Pulse*)self)->faces.at(i));
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
    LOGD("Java_pt_fraunhofer_pulse_Pulse__1face exit");
    return 0;
  }

/*
 * Class:     pt_fraunhofer_pulse_Pulse
 * Method:    _relativeMinFaceSize
 * Signature: (J)D
 */
JNIEXPORT jdouble JNICALL Java_pt_fraunhofer_pulse_Pulse__1relativeMinFaceSize
  (JNIEnv *jenv, jclass, jlong self)
{
    LOGD("Java_pt_fraunhofer_pulse_Pulse__1maxSignalSize enter");
    jdouble result = 0;
    try
    {
        if (self)
            result = ((Pulse*)self)->relativeMinFaceSize;
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
    LOGD("Java_pt_fraunhofer_pulse_Pulse__1maxSignalSize exit");
    return result;
  }

/*
 * Class:     pt_fraunhofer_pulse_Pulse
 * Method:    _maxSignalSize
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_pt_fraunhofer_pulse_Pulse__1maxSignalSize
  (JNIEnv *jenv, jclass, jlong self)
  {
    LOGD("Java_pt_fraunhofer_pulse_Pulse__1maxSignalSize enter");
    jint result = 0;
    try
    {
        if (self)
            result = ((Pulse*)self)->maxSignalSize;
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
    LOGD("Java_pt_fraunhofer_pulse_Pulse__1maxSignalSize exit");
    return result;
  }

/*
 * Class:     pt_fraunhofer_pulse_Pulse
 * Method:    _magnification
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL Java_pt_fraunhofer_pulse_Pulse__1magnification__J
  (JNIEnv *jenv, jclass, jlong self)
  {
    LOGD("Java_pt_fraunhofer_pulse_Pulse__1magnification__J enter");
    jboolean result = false;
    try
    {
        if (self)
            result = ((Pulse*)self)->evm.magnify;
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
    LOGD("Java_pt_fraunhofer_pulse_Pulse__1magnification__J exit");
    return result;
  }

/*
 * Class:     pt_fraunhofer_pulse_Pulse
 * Method:    _magnification
 * Signature: (JZ)V
 */
JNIEXPORT void JNICALL Java_pt_fraunhofer_pulse_Pulse__1magnification__JZ
  (JNIEnv *jenv, jclass, jlong self, jboolean m)
  {
    LOGD("Java_pt_fraunhofer_pulse_Pulse__1magnification__JZ enter");
    try
    {
        if (self)
            ((Pulse*)self)->evm.magnify = m;
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
    LOGD("Java_pt_fraunhofer_pulse_Pulse__1magnification__JZ exit");
  }

/*
 * Class:     pt_fraunhofer_pulse_Pulse
 * Method:    _magnificationFactor
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_pt_fraunhofer_pulse_Pulse__1magnificationFactor__J
  (JNIEnv *jenv, jclass, jlong self)
  {
    LOGD("Java_pt_fraunhofer_pulse_Pulse__1maxSignalSize enter");
    jint result = 0;
    try
    {
        if (self)
            result = ((Pulse*)self)->evm.alpha;
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
    LOGD("Java_pt_fraunhofer_pulse_Pulse__1maxSignalSize exit");
    return result;
  }

/*
 * Class:     pt_fraunhofer_pulse_Pulse
 * Method:    _magnificationFactor
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL Java_pt_fraunhofer_pulse_Pulse__1magnificationFactor__JI
  (JNIEnv *jenv, jclass, jlong self, jint m)
  {
    LOGD("Java_pt_fraunhofer_pulse_Pulse__1maxSignalSize enter");
    try
    {
        if (self)
            ((Pulse*)self)->evm.alpha = m;
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
    LOGD("Java_pt_fraunhofer_pulse_Pulse__1maxSignalSize exit");
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
