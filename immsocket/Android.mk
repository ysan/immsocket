LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libimmsocket

LOCAL_LDLIBS := \
	-llog \
	-limmsocketcommon \

LOCAL_PRELINK_MODULE := false

LOCAL_CFLAGS := -D_ANDROID_BUILD

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../immsocketcommon \


LOCAL_SHARED_LIBRARIES := \
	libimmsocketcommon

LOCAL_SRC_FILES := \
	ImmSocketServer.cpp \
	ImmSocketClient.cpp \
	ImmSocketCommon.cpp \


include $(BUILD_SHARED_LIBRARY)
