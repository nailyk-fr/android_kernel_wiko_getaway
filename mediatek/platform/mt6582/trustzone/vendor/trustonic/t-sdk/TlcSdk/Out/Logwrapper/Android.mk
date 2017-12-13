# =============================================================================
#
# MobiCore log wrapper to be included by Android components / products
#
# =============================================================================

# This is not a separate module.
# Only for inclusion by other modules.

LOCAL_LDLIBS += -llog

# Enable logging to logcat per default
LOCAL_CFLAGS += -DLOG_ANDROID

LOCAL_C_INCLUDES += $(call my-dir)
