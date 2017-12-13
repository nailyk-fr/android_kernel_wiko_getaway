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

/**
 * @file   drMain.c
 * @brief  Implements the entry point of the driver.
 *
 */


#include "drStd.h"
#include "DrApi/DrApi.h"

#include "drCommon.h"


DECLARE_DRIVER_MAIN_STACK(2048);


/* External functions */
#ifdef DR_FEATURE_DCI_HANDLER
extern void drDcihInit( const addr_t dciBuffer, const uint32_t dciBufferLen );
#endif // DR_FEATURE_DCI_HANDLER
//------------------------------------------------------------------------------
extern void drIpchInit( const addr_t dciBuffer, const uint32_t dciBufferLen );
extern _NORETURN void drExchLoop( const addr_t dciBuffer, const uint32_t  dciBufferLen );


/**
 * Initialization function
 */
static drApiResult_t doInitialization( void )
{
    drApiResult_t ret = DRAPI_OK;

    /**
     * TODO: This is the function where certain initialization
     * can be done before proceeding further. Such as HW related
     * initialization. Update the return code accordingly
     */

    return ret;
}

/**
 * Main routine.
 * Starts the required threads.
 */
_DRAPI_ENTRY void drMain(
    const addr_t    dciBuffer,
    const uint32_t  dciBufferLen
){
    drApiLogPrintf("[<t Driver DrTemplate] %d.%d, Build " __DATE__ ", " __TIME__ EOL,
                        DRIVER_VERSION_MAJOR, DRIVER_VERSION_MINOR);
    /* Initialization */
    if (DRAPI_OK != doInitialization())
    {
        drDbgPrintLnf("[Driver DrTemplate] drMain(): Initialization failed");

        /* No need to proceed further. Shutdown the main thread */
        if (DRAPI_OK != drApiStopThread(NILTHREAD))
        {
            drDbgPrintLnf("[Driver DrTemplate] drMain(): Unable to stop main thread");
        }
    }

    /* Start IPC handler */
    drIpchInit(dciBuffer, dciBufferLen);

#ifdef DR_FEATURE_DCI_HANDLER
    /* Start DCI handler */
    drDcihInit(dciBuffer, dciBufferLen);
#endif // DR_FEATURE_DCI_HANDLER

    /* Start exception handler */
    drExchLoop(dciBuffer, dciBufferLen);
}
