LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := AuroraStore
LOCAL_MODULE_STEM := AuroraStore.apk
LOCAL_SRC_FILES := AuroraStore.apk
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT)/priv-app/AuroraStore

include $(BUILD_PREBUILT)

