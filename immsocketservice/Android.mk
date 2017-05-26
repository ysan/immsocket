LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libimmsocketservice

LOCAL_LDLIBS := \
	-llog \
	-limmsocket \
	-lcommon \	

LOCAL_PRELINK_MODULE := false

LOCAL_CFLAGS := -D_ANDROID_BUILD

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../common \
	$(LOCAL_PATH)/../immsocket \


LOCAL_SHARED_LIBRARIES := \
	libcommon \
	libimmsocket \

LOCAL_SRC_FILES := \
	PacketHandler.cpp \
	Message.cpp \
	ClientHandler.cpp \
	AsyncProcProxy.cpp \
	ImmSocketServiceCommon.cpp \


include $(BUILD_SHARED_LIBRARY)
