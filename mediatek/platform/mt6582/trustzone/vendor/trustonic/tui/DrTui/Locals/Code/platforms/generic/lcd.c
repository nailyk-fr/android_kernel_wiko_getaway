/*
 * Copyright (c) 2013 TRUSTONIC LIMITED
 * All rights reserved
 * 
 * The present software is the confidential and proprietary information of
 * TRUSTONIC LIMITED. You shall not disclose the present software and shall
 * use it only in accordance with the terms of the license agreement you
 * entered into with TRUSTONIC LIMITED. This software may be subject to
 * export or import laws in certain countries.
 */

#include "drStd.h"
#include "DrApi/DrApi.h"
#include "drTuiHal.h"

#define TAG "[TUI HAL DISP] "

/** Protect display controller */
drTuiHalError_t tuiHalDisplayProtectController() {
    drTuiHalError_t ret = E_TUI_HAL_INTERNAL;
    drDbgPrintLnf(TAG "%s", __func__);
    return ret;
}

/** Unprotect display controller */
drTuiHalError_t tuiHalDisplayUnprotectController() {
    drTuiHalError_t ret = E_TUI_HAL_INTERNAL;
    drDbgPrintLnf(TAG "%s", __func__);
    return ret;
}

/**
 * tuiHalDisplayMapController()
 *      Map the video controller register into the driver address space
 */
drTuiHalError_t tuiHalDisplayMapController(void)
{
    drTuiHalError_t ret = E_TUI_HAL_INTERNAL;
    drDbgPrintLnf(TAG "%s", __func__);
    return ret;
}

/**
 * tuiHalDisplayUnmapController()
 *      Unmap the video controller register from the driver address space
 */
drTuiHalError_t tuiHalDisplayUnMapController(void)
{
    drTuiHalError_t ret = E_TUI_HAL_INTERNAL;
    drDbgPrintLnf(TAG "%s", __func__);
    return ret;
}

/**
 * tuiHalDisplayInitialize()
 *      Backup NWd config (in a static struct), protect and reconfig DSS for TUI frame buffer)
 *      Disable all windows but win4 (highest blend priority), disable win4 blending
 */
drTuiHalError_t tuiHalDisplayInitialize(uint32_t tuiFrameBufferPa,
                                        uint32_t tuiFrameBufferSize)
{
    drTuiHalError_t ret = E_TUI_HAL_INTERNAL;
    drDbgPrintLnf(TAG "%s", __func__);
    return ret;
}

/** Restore NWd config and unprotect DSS */
drTuiHalError_t tuiHalDisplayUninitialize(void) {
    drTuiHalError_t ret = E_TUI_HAL_INTERNAL;
    drDbgPrintLnf(TAG "%s", __func__);
    return ret;
}
