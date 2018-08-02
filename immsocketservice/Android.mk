LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libimmsocketservice

LOCAL_LDLIBS := \
	-llog \
	-limmsocket \
	-limmsocketcommon \	

LOCAL_PRELINK_MODULE := false

LOCAL_CFLAGS := -D_ANDROID_BUILD

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../immsocketcommon \
	$(LOCAL_PATH)/../immsocket \


LOCAL_SHARED_LIBRARIES := \
	libimmsocketcommon \
	libimmsocket \

LOCAL_SRC_FILES := \
	PacketHandler.cpp \
	Message.cpp \
	Common.cpp \
	MessageId.cpp \
	SyncRequestManager.cpp \


include $(BUILD_SHARED_LIBRARY)
