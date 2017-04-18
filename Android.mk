LOCAL_PATH := $(call my-dir)
#TMP_LOCAL_PATH := $(LOCAL_PATH)

#include $(TMP_LOCAL_PATH)/common/Android.mk
#include $(TMP_LOCAL_PATH)/localsocket/Android.mk
#include $(TMP_LOCAL_PATH)/localsocketservice/Android.mk
#include $(TMP_LOCAL_PATH)/client_sample/Android.mk
#include $(TMP_LOCAL_PATH)/server_sample/Android.mk

include $(call all-makefiles-under,$(LOCAL_PATH))
