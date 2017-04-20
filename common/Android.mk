LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libcommon

LOCAL_LDLIBS := -llog -rdynamic -lunwind -ldl

LOCAL_PRELINK_MODULE := false

LOCAL_CFLAGS := -D_ANDROID_BUILD


LOCAL_SHARED_LIBRARIES :=

LOCAL_SRC_FILES := \
	WorkerThread.cpp \
	SigwaitThread.cpp \
	Utils.cpp \


include $(BUILD_SHARED_LIBRARY)
