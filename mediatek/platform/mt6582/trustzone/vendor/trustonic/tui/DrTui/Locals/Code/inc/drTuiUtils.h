/** @addtogroup DRIVER_TUI
 * @{
 * @file   drUtils.h
 * @brief  Contains utility API declerations
 *
 * <Copyright goes here>
 */

#ifndef __DRUTILS_H__
#define __DRUTILS_H__

#include "DrApi/DrApiThread.h"
#include "DrApi/DrApiMm.h"

#include "drTuiCommon.h"


/* Syscall definitions */
#define SYSCALL_THREAD_EX_REGS      12

/**
 * Restarts thread after setting sp (with clean stack) and ip accordingly
 *
 * @retval E_OK    no error.
 */
drApiResult_t drUtilsRestartThread(
    threadno_t threadno,
    addr_t ip,
    addr_t sp );


/**
 * Makes request to RTM to update notification thread
 *
 * @retval E_OK    no error.
 */
drApiResult_t drUtilsUpdateNotificationThread( threadno_t threadno );


#endif // __DRUTILS_H__
