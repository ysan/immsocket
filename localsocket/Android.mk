LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := liblocalsocket

LOCAL_LDLIBS := \
	-llog \
	-lcommon \

LOCAL_PRELINK_MODULE := false

LOCAL_CFLAGS := -D_ANDROID_BUILD

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../common \


LOCAL_SHARED_LIBRARIES := \
	libcommon

LOCAL_SRC_FILES := \
	LocalSocketServer.cpp \
	LocalSocketClient.cpp \
	LocalSocketCommon.cpp \


include $(BUILD_SHARED_LIBRARY)
