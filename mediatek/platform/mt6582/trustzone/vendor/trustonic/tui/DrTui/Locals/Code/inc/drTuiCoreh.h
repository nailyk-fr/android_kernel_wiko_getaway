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

#ifndef __DRTUICOREH_H__
#define __DRTUICOREH_H__

#include "dciTui.h"

#include "TlApi/TlApiTui.h"
#include "drTuiHal.h"

/**
 * Return codes of the core tui driver.
 */
#define TUI_CORE_OK                     0x00040000
#define TUI_CORE_ERR_UNKNOWN_CMD        0x00040001
#define TUI_CORE_ERR_NOT_SUPPORTED      0x00040002
#define TUI_CORE_ERR_INTERNAL_ERROR     0x00040003
#define TUI_CORE_ERR_NO_RESPONSE        0x00040004
#define TUI_CORE_ERR_BAD_PARAMETERS     0x00040005
#define TUI_CORE_ERR_BUSY               0x00040006
#define TUI_CORE_ERR_NO_EVENT           0x00040007
#define TUI_CORE_ERR_NO_SESSION         0x00040008
#define TUI_CORE_ERR_OUT_OF_DISPLAY     0x00040009
#define TUI_CORE_ERR_IMG_BAD_FORMAT     0x00040010
//#define TUI_CORE_ERR_IGNORE             9

// Internal notifications
#define CORE_NOT_NONE   0
#define CORE_NOT_TOUCH  1

// internal commands
#define CORE_CMD_NONE                   0
#define CORE_CMD_OPEN_SESSION           1
#define CORE_CMD_CLOSE_SESSION          2
#define CORE_CMD_SET_IMAGE              3
#define CORE_CMD_GET_SCREEN_INFO        4
#define CORE_CMD_GET_TOUCH_EVENT        5


/* [INTERNAL] TODO-[2013-10-10]-[julare01]: Should be moved to a separate header with generic
 * stuff like structs in drTuiHal (tuiImage, tuiCoordinate, etc). [/INTERNAL] */
#define FRAMEBUF_VIRT_BASE          0x00800000

typedef uint32_t    coreReturnCode_t;
typedef volatile uint32_t    coreCommandId_t;
typedef volatile uint32_t    coreResponseId_t;
typedef volatile uint32_t    coreNotificationId_t;


/**
 * CORE command struct
 */
typedef struct{
    coreCommandId_t     id;
    uint32_t            client;
    union {
        tuiImage_t      image;
    };
} coreCommand_t, *coreCommand_ptr;

/**
 * Core response struct
 */
typedef struct{
    coreResponseId_t    id; // must be (command ID | RSP_ID_MASK )
    coreReturnCode_t    returnCode;
    union {
        tlApiTuiTouchEvent_t    touchEvent;
        tlApiTuiScreenInfo_t    screenInfo;
    };
} coreResponse_t, *coreResponse_ptr;

/**
 * Core shared buffer struct
 */
typedef struct {
    coreNotificationId_t    touchNotif;
    coreCommand_t           ipchCmd;
    coreResponse_t          rspIpch;
} tuiCoreMsg_t, *tuiCoreMsg_ptr;


// Global variables
extern tuiScreenInfo_t    gScreenInfo;
extern drTouchInfo_t        g_touchInfo;
extern tuiFbInfo_t          gTuiFrameBuffer;
extern tuiFbInfo_t          gTuiWorkBuffer;


// Global functions
drError_t tuiCoreMapL1Buffer(addr_t virtAddrStart, phys_addr_t physAddrStart,
                uint32_t size);
drError_t tuiCoreUnmapL1Buffer(addr_t virtAddrStart,
                uint32_t size);


#endif // __DRTUICOREH_H__





