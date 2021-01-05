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

#include "DIDO_ConfigMgr.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "applibs_versions.h"
#include <applibs/log.h>

#include "json.h"
#include "DIDO_FetchConfig.h"
#include "DIDO_WatchConfig.h"
#include "PropertyItems.h"

typedef struct DIDO_ConfigMgr {
    DIDO_FetchConfig* fetchConfig;
    DIDO_WatchConfig* watchConfig;
} DIDO_ConfigMgr;

typedef enum {
    FEATURE_UNSELECT = -1,
    FEATURE_FALSE = 0,
    FEATURE_TRUE = 1
}FEATURE_SELECT;

typedef enum {
    DIDO_FEATURE_PULSECOUNTER = 0,
    DIDO_FEATURE_EDGE,
    DIDO_FEATURE_POLLING,
    DIDO_FEATURE_NUM
}FEATURE_TYPE;

static const char DIDOFeatureKey[DIDO_FEATURE_NUM][PROPERTY_NAME_MAX_LEN] = {
    "Counter_DIDO%d", "Edge_DIDO%d", "Polling_DIDO%d"
};

#define DIDO_PORT_OFFSET 1

static DIDO_ConfigMgr sDIDO_ConfigMgr;  // singleton

// Initializaition and cleanup
void
DIDO_ConfigMgr_Initialize(void)
{
    sDIDO_ConfigMgr.fetchConfig = DIDO_FetchConfig_New();
    sDIDO_ConfigMgr.watchConfig = DIDO_WatchConfig_New();
}

void
DIDO_ConfigMgr_Cleanup(void)
{
    DIDO_FetchConfig_Destroy(sDIDO_ConfigMgr.fetchConfig);
    DIDO_WatchConfig_Destroy(sDIDO_ConfigMgr.watchConfig);
}

static bool
DIDO_ConfigMgr_CheckStateTransition(FEATURE_SELECT* NextStatus, bool* PrevStatus)
{
    bool ret = true;

    if (NextStatus[DIDO_FEATURE_PULSECOUNTER] == FEATURE_TRUE) {
        // PulseCounter
        if ((PrevStatus[DIDO_FEATURE_EDGE] && (NextStatus[DIDO_FEATURE_EDGE] == FEATURE_UNSELECT)) ||
            (PrevStatus[DIDO_FEATURE_POLLING] && (NextStatus[DIDO_FEATURE_POLLING] == FEATURE_UNSELECT))) {
            ret = false;
        }
    } else if (NextStatus[DIDO_FEATURE_EDGE] == FEATURE_TRUE) {
        // Edge
        if ((PrevStatus[DIDO_FEATURE_PULSECOUNTER] && (NextStatus[DIDO_FEATURE_PULSECOUNTER] == FEATURE_UNSELECT)) ||
            (PrevStatus[DIDO_FEATURE_POLLING] && (NextStatus[DIDO_FEATURE_POLLING] == FEATURE_UNSELECT))) {
            ret = false;
        }
    } else if (NextStatus[DIDO_FEATURE_POLLING] == FEATURE_TRUE) {
        // Polling
        if ((PrevStatus[DIDO_FEATURE_PULSECOUNTER] && (NextStatus[DIDO_FEATURE_PULSECOUNTER] == FEATURE_UNSELECT)) ||
            (PrevStatus[DIDO_FEATURE_EDGE] && (NextStatus[DIDO_FEATURE_EDGE] == FEATURE_UNSELECT))) {
            ret = false;
        }
    }
    return ret;
}

static bool
DIDO_ConfigMgr_CheckDuplicate(json_value* json, int* enablePort)
{
    bool diPrevStatus[DIDO_FEATURE_NUM][NUM_DIDO] = {{false}};
    char diFeatureStr[PROPERTY_NAME_MAX_LEN];        
    json_value* jsonObj;

    *enablePort = DIDO_FetchConfig_GetFetchEnablePorts(sDIDO_ConfigMgr.fetchConfig, diPrevStatus[DIDO_FEATURE_PULSECOUNTER], diPrevStatus[DIDO_FEATURE_POLLING]) + 
                  DIDO_WatchConfig_GetWatchEnablePorts(sDIDO_ConfigMgr.watchConfig, diPrevStatus[DIDO_FEATURE_EDGE]);
    
    for (int i = 0; i < NUM_DIDO; i++) {
        FEATURE_SELECT selectStatus[DIDO_FEATURE_NUM] = {FEATURE_UNSELECT, FEATURE_UNSELECT, FEATURE_UNSELECT};
        int selectNum = 0;
        
        for (int j = 0; j < DIDO_FEATURE_NUM; j++) {
            bool value;
            sprintf(diFeatureStr, DIDOFeatureKey[j], i + DIDO_PORT_OFFSET);
            jsonObj = json_GetKeyJson(diFeatureStr, json);
            if (json_GetBoolValue(jsonObj, &value)) {
                selectStatus[j] = value;
                if (selectStatus[j] == FEATURE_TRUE) {
                    if (++selectNum > 1) {
                        Log_Debug("Setting value is duplicated\n");
                        return false;
                    }
                }
            }
        }

        bool prevStatus[DIDO_FEATURE_NUM];
        for (int j = 0; j < DIDO_FEATURE_NUM; j++) {
            prevStatus[j] = diPrevStatus[j][i];
        }

        if (! DIDO_ConfigMgr_CheckStateTransition(selectStatus,prevStatus)) {
            Log_Debug("Setting value is duplicated\n");
            return false;
        }
    }

    return true;
}

// Apply new configuration
SphereWarning
DIDO_ConfigMgr_LoadAndApplyIfChanged(const unsigned char* payload,
    unsigned int payloadSize, vector item)
{	
    json_value* jsonObj = json_parse(payload, payloadSize);
    json_value* desiredObj = json_GetKeyJson("desired", jsonObj);
    bool desireFlg = false;
    int enablePort = 0;

    if (desiredObj) {
        jsonObj = desiredObj;
        desireFlg = true;
    }

    if (! DIDO_ConfigMgr_CheckDuplicate(jsonObj, &enablePort)){
        if (desireFlg && (enablePort < 1)) { // initial desired message
            return ILLEGAL_DESIRED_PROPERTY;
        } else {
            return ILLEGAL_PROPERTY;
        }
    }

    bool initDesired = enablePort < 1 ? true : false;

    // configuration of the pulse counter function
    if (! DIDO_FetchConfig_LoadFromJSON(
        sDIDO_ConfigMgr.fetchConfig, jsonObj, initDesired , item, "1.0")) {
        Log_Debug("failed to DIDO_FetchConfig_LoadFromJSON().\n");
        return ILLEGAL_PROPERTY;
    }

    // configuration of the contact input function
    if (! DIDO_WatchConfig_LoadFromJSON(
        sDIDO_ConfigMgr.watchConfig, jsonObj, initDesired, item, "1.0")) {
        Log_Debug("failed to DIDO_FetchConfig_LoadFromJSON().\n");
        return ILLEGAL_PROPERTY;
    }

    if ( (jsonObj->u.object.length > 1) && (vector_size(item) < 1)) {
        return UNSUPPORTED_PROPERTY;
    }

    return NO_ERROR;
}

// Get configuratioin
DIDO_FetchConfig*
DIDO_ConfigMgr_GetFetchConfig()
{
    return sDIDO_ConfigMgr.fetchConfig;
}

DIDO_WatchConfig*
DIDO_ConfigMgr_GetWatchConfig()
{
    return sDIDO_ConfigMgr.watchConfig;
}
