IMM_PATH := $(call my-dir)

include $(CLEAR_VARS)

IMM_MODULE := libimmsocketservice

IMM_LDLIBS := \
	-llog \
	-limmsocket \
	-lcommon \	

IMM_PRELINK_MODULE := false

IMM_CFLAGS := -D_ANDROID_BUILD

IMM_C_INCLUDES := \
	$(IMM_PATH)/../common \
	$(IMM_PATH)/../immsocket \


IMM_SHARED_LIBRARIES := \
	libcommon \
	libimmsocket \

IMM_SRC_FILES := \
	PacketHandler.cpp \
	Message.cpp \
	ClientHandler.cpp \
	AsyncProcProxy.cpp \
	ImmSocketServiceCommon.cpp \


include $(BUILD_SHARED_LIBRARY)
