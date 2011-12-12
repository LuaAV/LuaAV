LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := libzengarden
include $(LOCAL_PATH)/Makefile.sourcefiles

LOCAL_STATIC_LIBRARIES := sndfile

LOCAL_CFLAGS := -I$(LOCAL_PATH)/../../libsndfile-1.0.20/src

include $(BUILD_STATIC_LIBRARY)
