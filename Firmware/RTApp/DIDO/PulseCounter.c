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

#include "PulseCounter.h"

#include "mt3620-gpio.h"

//
// Initialization
//
void 
PulseCounter_Initialize(PulseCounter* me, int pin)
{
    me->pinId = pin;
    me->isCountHight = true;
    me->prevState = false;
    me->currentState = false;
    me->pulseCounter = 0;
    me->pulseOnTime = 0;
    me->isSetPulse = false;
    me->minPulseSetTime = 0;
    me->pulseElapsedTime = 0;
    me->isRising = false;
    me->maxPulseCounter = 0;
    me->isStart = false;
    me->pulseOnTimeS = 0;
}

//
// Attribute
//
int
PulseCounter_GetPinId(PulseCounter* me)
{
    return me->pinId;
}

//
// Pulse counter driver operation
//
void 
PulseCounter_SetConfigCounter(PulseCounter* me,
    bool isCountHight, int minPulse, int maxPulse)
{
    me->isCountHight    = isCountHight;
    me->minPulseSetTime = minPulse;
    me->maxPulseCounter = maxPulse;
    me->prevState       = isCountHight;
    me->isRising        = !(isCountHight);
    me->isStart         = true;
}

void
PulseCounter_Clear(PulseCounter* me, int initValue)
{
    bool prevIsStart     = me->isStart;
    
    me->isStart          = false; // stop
    me->pulseCounter     = initValue;
    me->prevState        = me->isCountHight;
    me->isRising         = !(me->isCountHight);
    me->pulseOnTime      = 0;
    me->pulseOnTimeS     = 0;
    me->pulseElapsedTime = 0;
    me->isSetPulse       = false;
    if (prevIsStart) {
        me->isStart      = true; // restart
    }
}

int
PulseCounter_GetPulseCount(PulseCounter* me)
{
    return me->pulseCounter;
}

int 
PulseCounter_GetPulseOnTime(PulseCounter* me)
{
    return me->pulseOnTimeS;
}

bool 
PulseCounter_GetLevel(PulseCounter* me)
{
    return me->prevState;
}

bool 
PulseCounter_GetPinLevel(PulseCounter* me)
{
    return me->currentState;
}

int 
PulseCounter_SetPinLevel(PulseCounter* me)
{
    return Mt3620_Gpio_Write(me->pinId, me->setState);
}

//
// Handle polling based pulse counting task
//
void
PulseCounter_Counter(PulseCounter* me)
{
    bool newState;

    // check DIn pin's input level and do pulse counting task as state machine
    Mt3620_Gpio_Read(me->pinId, &newState);
    if (newState != me->prevState) {
        me->pulseElapsedTime = 0;
        me->isSetPulse = false;
        if (!me->prevState) {
            me->isRising = true;
        } else {
            me->isRising = false;
        }
        me->prevState = newState;
    } else {
        if (! me->isSetPulse) {
            me->pulseElapsedTime++;
            if (me->pulseElapsedTime > me->minPulseSetTime) {
                me->currentState = me->prevState;
                if ((me->isRising && me->isCountHight)
                ||  (!me->isRising && !me->isCountHight)) {
                    if (me->pulseCounter >= me->maxPulseCounter) {
                        me->pulseCounter = 0;
                    }
                    me->pulseCounter++;
                }
                me->pulseElapsedTime = 0;
                me->isSetPulse = true;
            }
        } else if (me->isRising) {
            me->pulseOnTime++;
            if (me->pulseOnTime > 1000) {
                me->pulseOnTimeS += me->pulseOnTime / 1000;
                me->pulseOnTime = me->pulseOnTime % 1000;
            }
        }
    }
}

void
dido_tmp_move(PulseCounter* me) {
    if (me->triggerActived == true) {
        if (me->delayEnable == true) {
            me->flagDelay = true;
            me->delayElapsedTime = 0;
        } else {
            Mt3620_Gpio_Write(me->pinId, me->driveState);
        }
        if (me->driveCertainEnable == true) {
            me->flagDriveTime = true;
            me->driveElapsedTime = 0;
        }
        me->triggerActived = false;
    }
    if (me->flagDelay == true) {
        if (me->delayElapsedTime >= me->delayTime) {
            Mt3620_Gpio_Write(me->pinId, me->driveState);
            me->flagDelay = false;
        } else {
            me->delayElapsedTime++;
        }
    }
    if (me->flagDriveTime == true) {
        if (me->driveElapsedTime >= me->driveTime) {
            Mt3620_Gpio_Write(me->pinId, me->defaultState);
            me->flagDriveTime = false;
        } else {
            me->driveElapsedTime++;
        }
    }
}
