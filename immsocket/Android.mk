IMM_PATH := $(call my-dir)

include $(CLEAR_VARS)

IMM_MODULE := libimmsocket

IMM_LDLIBS := \
	-llog \
	-lcommon \

IMM_PRELINK_MODULE := false

IMM_CFLAGS := -D_ANDROID_BUILD

IMM_C_INCLUDES := \
	$(IMM_PATH)/../common \


IMM_SHARED_LIBRARIES := \
	libcommon

IMM_SRC_FILES := \
	ImmSocketServer.cpp \
	ImmSocketClient.cpp \
	ImmSocketCommon.cpp \


include $(BUILD_SHARED_LIBRARY)
