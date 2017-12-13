################################################################################

LOCAL_PATH := $(call my-dir)

################################################################################
#
################################################################################
include $(CLEAR_VARS)
#-----------------------------------------------------------
#LOCAL_MODULE_TAGS := optional
#LOCAL_MODULE := libaccess_skin_beauty
LOCAL_PREBUILT_LIBS := libaccess_skin_beauty.so
LOCAL_LDLIBS := -ldl

#-----------------------------------------------------------
include $(BUILD_MULTI_PREBUILT)
#include $(BUILD_SHARED_LIBRARY)

################################################################################
#
################################################################################
include $(CLEAR_VARS)
LOCAL_PREBUILT_LIBS := libaccess_beautifier_c.so
LOCAL_LDLIBS := -ldl
include $(BUILD_MULTI_PREBUILT)


include $(CLEAR_VARS)
include $(call all-makefiles-under,$(LOCAL_PATH))