LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := send_test

LOCAL_LDLIBS := \
	-llog \

LOCAL_PRELINK_MODULE := false

LOCAL_CFLAGS := \
	-D_ANDROID_BUILD \

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../../common \


LOCAL_SHARED_LIBRARIES := \

LOCAL_SRC_FILES := \
	main.cpp \


include $(BUILD_EXECUTABLE)
