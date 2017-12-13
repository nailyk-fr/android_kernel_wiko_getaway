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
 
#ifndef __KEY_LAYOUT_H__
#define __KEY_LAYOUT_H__

#include "tlPinpadLayout.h"

/* x/y coordinates for our pin pad as in pictures/1024x600/pinpad.png (determined manually).
 * Use any graphic tool to look up the coordinates of your image.
 */
#define PAD_COL1_LEFT     (64)
#define PAD_COL1_RIGHT    (214)
#define PAD_COL2_LEFT     (225)
#define PAD_COL2_RIGHT    (375)
#define PAD_COL3_LEFT     (386)
#define PAD_COL3_RIGHT    (536)

/* /!\ yBottom > yTop */
#define PAD_LINE1_TOP      (500)
#define PAD_LINE1_BOTTOM   (560)
#define PAD_LINE2_TOP      (570)
#define PAD_LINE2_BOTTOM   (630)
#define PAD_LINE3_TOP      (640)
#define PAD_LINE3_BOTTOM   (700)
#define PAD_LINE4_TOP      (710)
#define PAD_LINE4_BOTTOM   (770)

/* TODO: Change this part because the entryfield's shape has changed */
#define TEXTAREA_LEFT      (108)
#define TEXTAREA_RIGHT     (491)
#define TEXTAREA_TOP       (355)
#define TEXTAREA_BOTTOM    (414)
#define TEXTAREA_MARGIN    (0)
#define TEXTAREA_CHARSIZE (108)

#define CANCEL_LEFT       (64)
#define CANCEL_RIGHT      (214)
#define CANCEL_TOP        (780)
#define CANCEL_BOTTOM     (840)

#define VALID_LEFT        (386)
#define VALID_RIGHT       (536)
#define VALID_TOP         (780)
#define VALID_BOTTOM      (840)

static const sLayout_t pinpad_layout = {
    /* sPinPadKey_t buttons_array[PINPAD_SIZE+1] */ {
  {0, 0, 0, 0, PAD_NOKEY_VAL, NOKEY},
  {PAD_COL1_LEFT, PAD_COL1_RIGHT, PAD_LINE1_TOP, PAD_LINE1_BOTTOM, 1,                PAD_NUM},
  {PAD_COL2_LEFT, PAD_COL2_RIGHT, PAD_LINE1_TOP, PAD_LINE1_BOTTOM, 2,                PAD_NUM},
  {PAD_COL3_LEFT, PAD_COL3_RIGHT, PAD_LINE1_TOP, PAD_LINE1_BOTTOM, 3,                PAD_NUM},
  {PAD_COL1_LEFT, PAD_COL1_RIGHT, PAD_LINE2_TOP, PAD_LINE2_BOTTOM, 4,                PAD_NUM},
  {PAD_COL2_LEFT, PAD_COL2_RIGHT, PAD_LINE2_TOP, PAD_LINE2_BOTTOM, 5,                PAD_NUM},
  {PAD_COL3_LEFT, PAD_COL3_RIGHT, PAD_LINE2_TOP, PAD_LINE2_BOTTOM, 6,                PAD_NUM},
  {PAD_COL1_LEFT, PAD_COL1_RIGHT, PAD_LINE3_TOP, PAD_LINE3_BOTTOM, 7,                PAD_NUM},
  {PAD_COL2_LEFT, PAD_COL2_RIGHT, PAD_LINE3_TOP, PAD_LINE3_BOTTOM, 8,                PAD_NUM},
  {PAD_COL3_LEFT, PAD_COL3_RIGHT, PAD_LINE3_TOP, PAD_LINE3_BOTTOM, 9,                PAD_NUM},
  {PAD_COL2_LEFT, PAD_COL2_RIGHT, PAD_LINE4_TOP, PAD_LINE4_BOTTOM, 0,                PAD_NUM},
  {PAD_COL3_LEFT, PAD_COL3_RIGHT, PAD_LINE4_TOP, PAD_LINE4_BOTTOM, PAD_CORRECT_VAL,  PAD_CORRECT},
  {CANCEL_LEFT,   CANCEL_RIGHT,   CANCEL_TOP,    CANCEL_BOTTOM,    PAD_CANCEL_VAL,   PAD_CANCEL},
  {VALID_LEFT,    VALID_RIGHT,    VALID_TOP,     VALID_BOTTOM,     PAD_VALIDATE_VAL, PAD_VALIDATE}
    },
    /* sTextAreaLayout_t textarea */ {
    TEXTAREA_LEFT,
    TEXTAREA_RIGHT,
    TEXTAREA_TOP,
    TEXTAREA_BOTTOM,
    TEXTAREA_MARGIN,
    TEXTAREA_CHARSIZE,
    }
};

#endif /* #ifndef __KEY_LAYOUT_H__ */

