MY_JNI_PATH := $(call my-dir)
MY_SRC_PATH := $(MY_JNI_PATH)/../../src/

include $(CLEAR_VARS)
LOCAL_PATH := $(MY_JNI_PATH)
LOCAL_MODULE := perflab

LOCAL_C_INCLUDES := $(MY_SRC_PATH)

LOCAL_SRC_FILES := $(wildcard *.cpp)
LOCAL_SRC_FILES += $(wildcard ../../src/*.cpp)
LOCAL_SRC_FILES += $(wildcard ../../src/common/*.cpp)

LOCAL_ARM_NEON := true
LOCAL_CPPFLAGS += -D_M_ARM

include $(BUILD_SHARED_LIBRARY)
