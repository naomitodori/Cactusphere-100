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

#ifndef _DIDO_CONFIG_MGR_H_
#define _DIDO_CONFIG_MGR_H_

#include "DIDO_FetchConfig.h"
#include "DIDO_WatchConfig.h"
#include "cactusphere_error.h"

#ifndef NUM_DIDO
#define NUM_DIDO 4
#endif

// Initializaition and cleanup
extern void	DIDO_ConfigMgr_Initialize(void);
extern void	DIDO_ConfigMgr_Cleanup(void);

// Apply new configuration
extern SphereWarning	DIDO_ConfigMgr_LoadAndApplyIfChanged(
    const unsigned char* payload, unsigned int payloadSize, vector item);

// Get configuratioin
extern DIDO_FetchConfig*  DIDO_ConfigMgr_GetFetchConfig(void);
extern DIDO_WatchConfig*  DIDO_ConfigMgr_GetWatchConfig(void);

#endif  // _DIDO_CONFIG_MGR_H_
