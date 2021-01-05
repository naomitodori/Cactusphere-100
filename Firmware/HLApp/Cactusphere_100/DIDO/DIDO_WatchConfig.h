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

#ifndef _DIDO_WATCH_CONFIG_H_
#define _DIDO_WATCH_CONFIG_H_

#ifndef _STDBOOL_H
#include <stdbool.h>
#endif

#ifndef CONTAINERS_VECTOR_H
#include <vector.h>
#endif

typedef struct DIDO_WatchConfig	DIDO_WatchConfig;
typedef struct _json_value	json_value;

#ifndef NUM_DIDO
#define NUM_DIDO 4
#endif

// Initialization and cleanup
extern DIDO_WatchConfig*	DIDO_WatchConfig_New(void);
extern void	DIDO_WatchConfig_Destroy(DIDO_WatchConfig* me);

// Load DIDO contact input watcher configuration from JSON
extern bool	DIDO_WatchConfig_LoadFromJSON(DIDO_WatchConfig* me,
    const json_value* json, bool desire, vector propertyItem, const char* version);

// Get configuration of DIDO contact input watchers
extern vector	DIDO_WatchConfig_GetFetchItems(DIDO_WatchConfig* me);

// Get enable port number of DIDO contact input
int DIDO_WatchConfig_GetWatchEnablePorts(DIDO_WatchConfig* me, bool* status);

#endif  // _DIDO_WATCH_CONFIG_H_
