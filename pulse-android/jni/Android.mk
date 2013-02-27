LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#OPENCV_CAMERA_MODULES:=off
#OPENCV_INSTALL_MODULES:=off
#OPENCV_LIB_TYPE:=SHARED
include libs/OpenCV-2.4.4-android-sdk/sdk/native/jni/OpenCV.mk

LOCAL_SRC_FILES  := Evm_jni.cpp
LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_LDLIBS     += -llog -ldl

LOCAL_MODULE     := evm

include $(BUILD_SHARED_LIBRARY)
