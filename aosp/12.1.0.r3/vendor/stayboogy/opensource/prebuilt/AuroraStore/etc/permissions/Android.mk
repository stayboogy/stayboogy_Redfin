LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := privapp-permissions-AuroraServices.xml
LOCAL_MODULE_STEM := privapp-permissions-AuroraServices.xml
LOCAL_SRC_FILES := privapp-permissions-AuroraServices.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/permissions/privapp-permissions-AuroraServices.xml

include $(BUILD_PREBUILT)


