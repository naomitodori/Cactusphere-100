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

#ifndef _DIDO_FETCH_TARGETS_H_
#define _DIDO_FETCH_TARGETS_H_

#ifndef CONTAINERS_VECTOR_H
#include <vector.h>
#endif

typedef struct DIDO_FetchTargets	DIDO_FetchTargets;
typedef struct DIDO_FetchItem	DIDO_FetchItem;

// Initialization and cleanup
extern DIDO_FetchTargets*	DIDO_FetchTargets_New(void);
extern void	DIDO_FetchTargets_Destroy(DIDO_FetchTargets* me);

// Get current acquisition targets
extern vector	DIDO_FetchTargets_GetFetchItems(DIDO_FetchTargets* me);

// Manage acquisition targets
extern void	DIDO_FetchTargets_Add(
    DIDO_FetchTargets* me, const DIDO_FetchItem* target);
extern void	DIDO_FetchTargets_Clear(DIDO_FetchTargets* me);

#endif  // _DIDO_FETCH_TARGETS_H_
