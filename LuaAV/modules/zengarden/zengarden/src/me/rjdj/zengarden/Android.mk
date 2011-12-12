LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := libjnizengarden
LOCAL_SRC_FILES := jnizengarden.cpp

LOCAL_STATIC_LIBRARIES := zengarden sndfile
#LOCAL_SHARED_LIBRARIES := sndfile

LOCAL_CFLAGS := -I$(LOCAL_PATH)/../../../

include $(BUILD_SHARED_LIBRARY)
