LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
  
##
## VERSIONING
##
AROMA_NAME    := AROMA Filemanager
AROMA_VERSION := 1.91
AROMA_BUILD   := $(shell date +%y%m%d%H)
AROMA_CN      := Degung Gamelan

## LOCAL PATH COPY
AROMAFM_LOCALPATH := $(LOCAL_PATH)

## AROMA CONTROLS SOURCE FILES
LOCAL_SRC_FILES += \
    src/aroma_openpty.c \
    src/controls/aroma_controls.c \
    src/controls/aroma_control_button.c \
    src/controls/aroma_control_check.c \
    src/controls/aroma_control_checkbox.c \
    src/controls/aroma_control_console.c \
    src/controls/aroma_control_edit.c \
    src/controls/aroma_control_filebox.c \
    src/controls/aroma_control_ime.c \
    src/controls/aroma_control_ime2.c \
    src/controls/aroma_control_imgbutton.c \
    src/controls/aroma_control_label.c \
    src/controls/aroma_control_menubox.c \
    src/controls/aroma_control_optbox.c \
    src/controls/aroma_control_progress.c \
    src/controls/aroma_control_textbox.c \
    src/controls/aroma_control_threads.c

## AROMA LIBRARIES SOURCE FILES
LOCAL_SRC_FILES += \
    src/libs/aroma_array.c \
    src/libs/aroma_freetype.c \
    src/libs/aroma_graph.c \
    src/libs/aroma_input.c \
    src/libs/aroma_languages.c \
    src/libs/aroma_libs.c \
    src/libs/aroma_memory.c \
    src/libs/aroma_png.c \
    src/libs/aroma_zip.c

## AROMA FILEMANAGER SOURCE FILES
LOCAL_SRC_FILES += \
    src/main/aroma.c \
    src/main/aroma_ui.c

## MODULE SETTINGS
LOCAL_MODULE                  := aroma_recovery
LOCAL_MODULE_TAGS             := eng
LOCAL_FORCE_STATIC_EXECUTABLE := true

## INCLUDES & OUTPUT PATH
LOCAL_C_INCLUDES              := $(AROMAFM_LOCALPATH)/include
LOCAL_C_INCLUDES              += external/freetype/include external/libpng external/zlib
LOCAL_MODULE_PATH             := $(TARGET_RECOVERY_ROOT_OUT)/sbin

## COMPILER FLAGS
LOCAL_CFLAGS                  += -DFT2_BUILD_LIBRARY=1 -DDARWIN_NO_CARBON 
LOCAL_CFLAGS                  += -D_AROMA_NODEBUG

## SET VERSION
LOCAL_CFLAGS += -DAROMA_NAME="\"$(AROMA_NAME)\""
LOCAL_CFLAGS += -DAROMA_VERSION="\"$(AROMA_VERSION)\""
LOCAL_CFLAGS += -DAROMA_BUILD="\"$(AROMA_BUILD)\""
LOCAL_CFLAGS += -DAROMA_BUILD_CN="\"$(AROMA_CN)\""

## INCLUDED LIBRARIES
LOCAL_STATIC_LIBRARIES        := libminutf8 libminzip.aroma libpng libft2 libm libc libz

ALL_DEFAULT_INSTALLED_MODULES += $(AROMA_ZIP_FILE)

include $(BUILD_EXECUTABLE)

include $(AROMAFM_LOCALPATH)/libs/minutf8/Android.mk
include $(AROMAFM_LOCALPATH)/libs/minzip/Android.mk
