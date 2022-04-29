LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_SRC_FILES:= QmiWwanCM.c GobiNetCM.c main.c MPQMUX.c QMIThread.c util.c qmap_bridge_mode.c mbim-cm.c device.c udhcpc.c
LOCAL_CFLAGS += -pie -fPIE -Wall -DUSE_ANDROID
LOCAL_LDFLAGS += -pie -fPIE
LOCAL_MODULE_TAGS:= optional
LOCAL_MODULE:= QFirehose
include $(BUILD_EXECUTABLE)
