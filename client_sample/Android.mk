LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := client_sample

LOCAL_LDLIBS := \
	-llog \
	-llocalsocketservice \
	-llocalsocket \
	-lcommon \

LOCAL_PRELINK_MODULE := false

LOCAL_CFLAGS := \
	-D_ANDROID_BUILD \
	-D_DEBUG_TCP \

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../common \
	$(LOCAL_PATH)/../localsocket \
	$(LOCAL_PATH)/../localsocketservice \


LOCAL_SHARED_LIBRARIES := \
	libcommon \
	liblocalsocket \
	liblocalsocketservice \

LOCAL_SRC_FILES := \
	ClMessageHandler.cpp \
	main.cpp \


include $(BUILD_EXECUTABLE)
