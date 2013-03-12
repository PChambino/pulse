LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#OPENCV_CAMERA_MODULES:=off
#OPENCV_INSTALL_MODULES:=off
#OPENCV_LIB_TYPE:=SHARED
include libs/OpenCV-2.4.4-android-sdk/sdk/native/jni/OpenCV.mk

LOCAL_SRC_FILES  := pt_fraunhofer_pulse_Pulse.cpp
LOCAL_SRC_FILES  += Pulse.cpp
LOCAL_SRC_FILES  += EvmGdownIIR.cpp
LOCAL_SRC_FILES  += ext/opencv.cpp
LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_LDLIBS     += -llog -ldl

LOCAL_MODULE     := pulse

include $(BUILD_SHARED_LIBRARY)
