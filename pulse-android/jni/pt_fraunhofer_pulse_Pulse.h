/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class pt_fraunhofer_pulse_Pulse */

#ifndef _Included_pt_fraunhofer_pulse_Pulse
#define _Included_pt_fraunhofer_pulse_Pulse
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     pt_fraunhofer_pulse_Pulse
 * Method:    _initialize
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_pt_fraunhofer_pulse_Pulse__1initialize
  (JNIEnv *, jclass);

/*
 * Class:     pt_fraunhofer_pulse_Pulse
 * Method:    _load
 * Signature: (JLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_pt_fraunhofer_pulse_Pulse__1load
  (JNIEnv *, jclass, jlong, jstring);

/*
 * Class:     pt_fraunhofer_pulse_Pulse
 * Method:    _start
 * Signature: (JII)V
 */
JNIEXPORT void JNICALL Java_pt_fraunhofer_pulse_Pulse__1start
  (JNIEnv *, jclass, jlong, jint, jint);

/*
 * Class:     pt_fraunhofer_pulse_Pulse
 * Method:    _onFrame
 * Signature: (JJ)V
 */
JNIEXPORT void JNICALL Java_pt_fraunhofer_pulse_Pulse__1onFrame
  (JNIEnv *, jclass, jlong, jlong);

/*
 * Class:     pt_fraunhofer_pulse_Pulse
 * Method:    _facesCount
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_pt_fraunhofer_pulse_Pulse__1facesCount
  (JNIEnv *, jclass, jlong);

/*
 * Class:     pt_fraunhofer_pulse_Pulse
 * Method:    _face
 * Signature: (JI)J
 */
JNIEXPORT jlong JNICALL Java_pt_fraunhofer_pulse_Pulse__1face
  (JNIEnv *, jclass, jlong, jint);

/*
 * Class:     pt_fraunhofer_pulse_Pulse
 * Method:    _maxSignalSize
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_pt_fraunhofer_pulse_Pulse__1maxSignalSize
  (JNIEnv *, jclass, jlong);

/*
 * Class:     pt_fraunhofer_pulse_Pulse
 * Method:    _magnification
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL Java_pt_fraunhofer_pulse_Pulse__1magnification__J
  (JNIEnv *, jclass, jlong);

/*
 * Class:     pt_fraunhofer_pulse_Pulse
 * Method:    _magnification
 * Signature: (JZ)V
 */
JNIEXPORT void JNICALL Java_pt_fraunhofer_pulse_Pulse__1magnification__JZ
  (JNIEnv *, jclass, jlong, jboolean);

/*
 * Class:     pt_fraunhofer_pulse_Pulse
 * Method:    _magnificationFactor
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_pt_fraunhofer_pulse_Pulse__1magnificationFactor__J
  (JNIEnv *, jclass, jlong);

/*
 * Class:     pt_fraunhofer_pulse_Pulse
 * Method:    _magnificationFactor
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL Java_pt_fraunhofer_pulse_Pulse__1magnificationFactor__JI
  (JNIEnv *, jclass, jlong, jint);

/*
 * Class:     pt_fraunhofer_pulse_Pulse
 * Method:    _destroy
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_pt_fraunhofer_pulse_Pulse__1destroy
  (JNIEnv *, jclass, jlong);

#ifdef __cplusplus
}
#endif
#endif
