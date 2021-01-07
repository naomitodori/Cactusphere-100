/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Atmark Techno, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef _DIDO_DRIVER_MSG_H_
#define _DIDO_DRIVER_MSG_H_

#ifndef _STDINT_H
#include <stdint.h>
#endif

// request code
enum {
    DIDO_SET_CONFIG_AND_START = 1,  // setting up a pulse conter
    DIDO_PULSE_COUNT_RESET    = 2,  // reset a pulse counter
    DIDO_READ_PULSE_COUNT     = 3,  // read the counter value
    DIDO_READ_DUTY_SUM_TIME   = 4,  // read the time integration of pulse
    DIDO_READ_PULSE_LEVEL     = 5,  // read the input level of all DIDO pin
    DIDO_READ_PIN_LEVEL       = 6,  // read the input level of specific DIDO pin
    DO_WRITE_PIN_LEVEL      = 7,  // write the output level of specific DO pin
    DIDO_READ_VERSION         = 255,// read the RTApp version
};

//
// message structure
//
// header
typedef struct DIDO_DriverMsgHdr {
    uint32_t	requestCode;
    uint32_t	messageLen;
} DIDO_DriverMsgHdr;

// body
    // DIDO_SET_CONFIG_AND_START
typedef struct DIDO_MsgSetConfig {
    uint32_t	pinId;
    uint32_t minPulseWidth;
    uint32_t maxPulseCount;
    bool isPulseHigh;
//
// sizeof(DIDO_MsgSetConfig) == messageLen
//
} DIDO_MsgSetConfig;
    // DIDO_PULSE_COUNT_RESET
typedef struct DIDO_MsgResetPulseCount {
    uint32_t	pinId;
    uint32_t initVal;
//
// sizeof(DIDO_MsgResetPulseCount) == messageLen
//
} DIDO_MsgResetPulseCount;
    // DIDO_READ_xx
typedef struct DIDO_MsgPinId {
    uint32_t	pinId;
//
// sizeof(DIDO_MsgPinId) == messageLen
//
} DIDO_MsgPinId;

// union of messages
typedef struct DIDO_DriverMsg {
    DIDO_DriverMsgHdr	header;
    union {
        DIDO_MsgSetConfig        setConfig;
        DIDO_MsgResetPulseCount  resetPulseCount;
        DIDO_MsgPinId            pinId;
    } body;
} DIDO_DriverMsg;

// response message
typedef struct DIDO_ReturnMsg {
    uint32_t	returnCode;
    uint32_t	messageLen;
    union {
        bool		levels[4];
        char        version[256];
    } message;
} DIDO_ReturnMsg;

#endif  // _DIDO_DRIVER_MSG_H_
