IMM_PATH := $(call my-dir)

include $(CLEAR_VARS)

IMM_MODULE := server_sample

IMM_LDLIBS := \
	-llog \
	-limmsocketservice \
	-limmsocket \
	-lcommon \

IMM_PRELINK_MODULE := false

IMM_CFLAGS := \
	-D_ANDROID_BUILD \
	-D_DEBUG_TCP \

IMM_C_INCLUDES := \
	$(IMM_PATH)/../common \
	$(IMM_PATH)/../immsocket \
	$(IMM_PATH)/../immsocketservice \


IMM_SHARED_LIBRARIES := \
	libcommon \
	libimmsocket \
	libimmsocketservice \

IMM_SRC_FILES := \
	SvrClientHandler.cpp \
	SvrMessageHandler.cpp \
	main.cpp \


include $(BUILD_EXECUTABLE)
