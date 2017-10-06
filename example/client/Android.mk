LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := client

LOCAL_LDLIBS := \
	-llog \
	-limmsocketservice \
	-limmsocket \
	-lcommon \

LOCAL_PRELINK_MODULE := false

LOCAL_CFLAGS := \
	-D_ANDROID_BUILD \
	-D_DEBUG_TCP \

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../../common \
	$(LOCAL_PATH)/../../immsocket \
	$(LOCAL_PATH)/../../immsocketservice \


LOCAL_SHARED_LIBRARIES := \
	libcommon \
	libimmsocket \
	libimmsocketservice \

LOCAL_SRC_FILES := \
	ClMessageHandler.cpp \
	main.cpp \


include $(BUILD_EXECUTABLE)