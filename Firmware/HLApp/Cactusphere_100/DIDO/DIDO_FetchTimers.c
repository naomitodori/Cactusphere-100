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

#include "DIDO_FetchTimers.h"

#include "DIDO_FetchItem.h"
#include "LibDIDO.h"

// Initialization
void
DIDO_FetchTimers_InitForTimer(FetchTimers* me, FetchItemBase* fetchItemBase)
{
    // configure and start the pulse counter for a pin
    DIDO_FetchItem*	fetchTime = (DIDO_FetchItem*)fetchItemBase;

    if (fetchTime->isCountClear) {
        DIDO_Lib_ResetPulseCount(fetchTime->pinID, 0);
        fetchTime->isCountClear = false;
    }
    DIDO_Lib_ConfigPulseCounter(fetchTime->pinID, fetchTime->isPulseHigh,
        fetchTime->minPulseWidth, fetchTime->maxPulseCount);
}
