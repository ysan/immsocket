LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := liblocalsocketservice

LOCAL_LDLIBS := \
	-llog \
	-llocalsocket \
	-lcommon \	

LOCAL_PRELINK_MODULE := false

LOCAL_CFLAGS := -D_ANDROID_BUILD

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../common \
	$(LOCAL_PATH)/../localsocket \


LOCAL_SHARED_LIBRARIES := \
	libcommon \
	liblocalsocket \

LOCAL_SRC_FILES := \
	PacketHandler.cpp \
	Message.cpp \
	ClientHandler.cpp \
	AsyncProcProxy.cpp \
	LocalSocketServiceCommon.cpp \


include $(BUILD_SHARED_LIBRARY)
