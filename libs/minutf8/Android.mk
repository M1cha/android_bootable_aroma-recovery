LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	minutf8.c

LOCAL_MODULE := libminutf8

LOCAL_CFLAGS += -Wall

include $(BUILD_STATIC_LIBRARY)
