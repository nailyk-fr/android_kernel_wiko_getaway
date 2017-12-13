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
 * @file   drSmgmt.c
 * @brief  Implements session management
 *
 */


#include "drStd.h"

#include "drCommon.h"
#include "drSmgmt.h"


/* Session context */
drSessionCtx_t drSessionCtx;


/**
 * Open session and initializes data
 *
 * @param threadid  caller thread identifier
 *
 * @retval session id or DR_SID_INVALID
 */
uint32_t drSmgmtOpenSession(threadid_t threadid)
{
    uint32_t sid = DR_SID_INVALID;
    drSessionReg_ptr  pRegEntry;

    do
    {
        for (uint32_t i = 0; i < MAX_DR_SESSIONS; i++)
        {
            pRegEntry = &(drSessionCtx.sessionReg[i]);
            if (pRegEntry->state == SESSION_STATE_UNUSED)
            {
                /* use available session sid */
                sid = i;
                /* Update state and threadid*/
                pRegEntry->state = SESSION_STATE_ACTIVE;
                pRegEntry->threadid = threadid;
                break;
            }
        }
    }while(0);

    return sid;
}


/**
 * Close session
 *
 * @param sid  session id
 *
 * @retval none
 */
void drSmgmtCloseSession( uint32_t sid )
{
    if(sid < MAX_DR_SESSIONS)
    {
        memset(&(drSessionCtx.sessionReg[sid]), 0, sizeof(drSessionReg_t));
    }
}


/**
 * Close session opened by specific thread
 *
 * @param threadid  caller thread identifier
 *
 * @retval none
 */
void drSmgmtCloseSessionForThread( threadid_t threadid )
{
    drSessionReg_ptr  pRegEntry;

    for (uint32_t i = 0; i < MAX_DR_SESSIONS; i++)
    {
        pRegEntry = &(drSessionCtx.sessionReg[i]);
        if (pRegEntry->threadid == threadid)
        {
            drSmgmtCloseSession(i);
        }
    }
}


/**
 * Retrieves session data based on thread id
 *
 * @param sid  session id
 *
 * @retval pointer to the session data
 */
drSessionReg_ptr drSmgmtGetSessionData( uint32_t sid )
{
    if (sid >= MAX_DR_SESSIONS)
    {
        return NULL;
    }

    return &(drSessionCtx.sessionReg[sid]);
}


/**
 * Set session data
 *
 * @param sid  session id
 * @param threadid  caller thread identifier
 * @param pData  pointer to data
 *
 * @retval E_DR_SMGMT_OK or E_DR_SMGMT_INVALID
 */
drSmgmtError_t drSmgmtSetSessionData(
        uint32_t sid,
        threadid_t threadid,
        drMarshalingData_ptr pData)
{
    drSmgmtError_t ret = E_DR_SMGMT_INVALID;
    drSessionReg_ptr  pRegEntry;

    if (!pData)
    {
        return ret;
    }

    for (uint32_t i = 0; i < MAX_DR_SESSIONS; i++)
    {
        pRegEntry = &(drSessionCtx.sessionReg[i]);
        /* If session id and thread id match update session data */
        if ((sid == i) && (pRegEntry->threadid == threadid))
        {
            /* Copy data */
            memcpy(&(pRegEntry->mData), pData, sizeof(drMarshalingData_t));
            ret = E_DR_SMGMT_OK;
            break;
        }
    }

    return ret;
}

/**
 * Init session management
 *
 * @param none
 *
 * @retval none
 */
void drSmgmtInit()
{
    for (uint32_t i = 0; i < MAX_DR_SESSIONS; i++)
    {
        memset(&(drSessionCtx.sessionReg[i]), 0, sizeof(drSessionReg_t));
    }
}


/**
 * Closes all active session
 *
 * @param none
 *
 * @retval none
 */
void drSmgmtCloseAllSessions()
{
    for (uint32_t i = 0; i < MAX_DR_SESSIONS; i++)
    {
        drSmgmtCloseSession(i);
    }
}

