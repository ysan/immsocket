LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := server_test

LOCAL_LDLIBS := \
	-llog \
	-limmsocketservice \
	-limmsocket \
	-limmsocketcommon \

LOCAL_PRELINK_MODULE := false

LOCAL_CFLAGS := \
	-D_ANDROID_BUILD \
	-D_DEBUG_TCP \

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../../immsocketcommon \
	$(LOCAL_PATH)/../../immsocket \
	$(LOCAL_PATH)/../../immsocketservice \


LOCAL_SHARED_LIBRARIES := \
	libimmsocketcommon \
	libimmsocket \
	libimmsocketservice \

LOCAL_SRC_FILES := \
	SvrMessageHandler.cpp \
	main.cpp \


include $(BUILD_EXECUTABLE)
