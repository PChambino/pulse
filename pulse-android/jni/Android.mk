LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

include /home/smite/dev/opencv/platforms/build_android_arm/install/sdk/native/jni/OpenCV.mk

LOCAL_SRC_FILES  := pt_fraunhofer_pulse_Pulse_Face.cpp
LOCAL_SRC_FILES  += pt_fraunhofer_pulse_Pulse.cpp
LOCAL_SRC_FILES  += Pulse.cpp
LOCAL_SRC_FILES  += EvmGdownIIR.cpp
LOCAL_SRC_FILES  += ext/opencv.cpp
LOCAL_SRC_FILES  += profiler/Profiler.cpp
LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_LDLIBS     += -llog -ldl

LOCAL_MODULE     := pulse

include $(BUILD_SHARED_LIBRARY)
