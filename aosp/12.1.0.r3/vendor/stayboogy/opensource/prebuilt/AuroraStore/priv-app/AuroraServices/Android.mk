LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := AuroraServices
LOCAL_MODULE_STEM := AuroraServices.apk
LOCAL_SRC_FILES := AuroraServices.apk
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT)/priv-app/AuroraServices

include $(BUILD_PREBUILT)

