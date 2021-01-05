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

#include "DIDO_WatchConfig.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "json.h"
#include "DIDO_WatchItem.h"
#include "TelemetryItems.h"
#include "PropertyItems.h"

struct DIDO_WatchConfig {
    vector	mWatchItems;	// vector of DIDO contact input configuration
    char	version[32];	// version string (not using)
};

// key Items in JSON
extern const char PinIDKey[];
const char DIDO_WatchConfigConfigKey[] = "DIDOWatchConfig";
const char NotifyChangeForeHigh[]    = "notifyChangeForeHigh";

const char EdgeDIDOKey[]             = "Edge_DIDO";
const char EdgeNotifyIsHighDIDOKey[] = "edgeNotifyIsHigh_DIDO";

#define DIDO_WATCH_PORT_OFFSET 1

// Initialization and cleanup
DIDO_WatchConfig*
DIDO_WatchConfig_New(void)
{
    DIDO_WatchConfig*	newObj =
        (DIDO_WatchConfig*)malloc(sizeof(DIDO_WatchConfig));

    if (NULL != newObj) {
        newObj->mWatchItems = vector_init(sizeof(DIDO_WatchItem));
        if (NULL == newObj->mWatchItems) {
            free(newObj);
            return NULL;
        }
        memset(newObj->version, 0, sizeof(newObj->version));
    }

    return newObj;
}

void
DIDO_WatchConfig_Destroy(DIDO_WatchConfig* me)
{
    vector_destroy(me->mWatchItems);
    free(me);
}

// Load DIDO contact input watcher configuration from JSON
bool
DIDO_WatchConfig_LoadFromJSON(DIDO_WatchConfig* me,
    const json_value* json, bool desire, vector propertyItem, const char* version)
{
    DIDO_WatchItem config[NUM_DIDO] = {
        // telemetryName, pinID, notifyChangeForHigh, isCountClear
        {"", 0, false, false},
        {"", 1, false, false},
        {"", 2, false, false},
        {"", 3, false, false}
    };
    bool overWrite[NUM_DIDO] = {false};
    bool ret = true;

    const size_t edgeDiLen = strlen(EdgeDIDOKey);
    const size_t notifyHighDiLen = strlen(EdgeNotifyIsHighDIDOKey);

    if (! json) {
        return false;
    }

    if (! vector_is_empty(me->mWatchItems)) {
        DIDO_WatchItem* tmp = (DIDO_WatchItem*)vector_get_data(me->mWatchItems);
        for (int i = 0; i < vector_size(me->mWatchItems); i++){
            memcpy(&config[tmp->pinID], tmp, sizeof(DIDO_WatchItem));
            overWrite[tmp->pinID] = true;
            tmp ++;
        }
    }

    if (0 != vector_size(me->mWatchItems)) {
        DIDO_WatchItem*	curs = (DIDO_WatchItem*)vector_get_data(me->mWatchItems);

        for (int i = 0, n = vector_size(me->mWatchItems); i < n; ++i) {
            TelemetryItems_RemoveDictionaryElem(curs->telemetryName);
        }
        vector_clear(me->mWatchItems);
        memset(me->version, 0, sizeof(me->version));
    }

    int pinid;
    for (int i = 0; i < json->u.object.length; i++) {
        char* propertyName = json->u.object.values[i].name;
        json_value* item = json->u.object.values[i].value;

        if (0 == strncmp(propertyName, EdgeDIDOKey, edgeDiLen)) {
            pinid = strtol(&propertyName[edgeDiLen], NULL, 10) - DIDO_WATCH_PORT_OFFSET;
            if (pinid < 0) {
                continue;
            }
            bool value;
            if (json_GetBoolValue(item, &value)) {
                if ((overWrite[pinid] && !value) || (!overWrite[pinid] && value)) {
                    // feature has changed
                    config[pinid].isCountClear = true;
                }
                overWrite[pinid] = value;
                sprintf(config[pinid].telemetryName, "DIDO%d_EdgeEvent", pinid + DIDO_WATCH_PORT_OFFSET);
                PropertyItems_AddItem(propertyItem, propertyName, TYPE_BOOL, overWrite[pinid]);
            } else {
                ret = false;
            }
        } else if(0 == strncmp(propertyName, EdgeNotifyIsHighDIDOKey, notifyHighDiLen)) {
            pinid = strtol(&propertyName[notifyHighDiLen], NULL, 10) - DIDO_WATCH_PORT_OFFSET;
            if (pinid < 0) {
                continue;
            }
            bool value;
            if (json_GetBoolValue(item, &value)) {
                if (config[pinid].notifyChangeForHigh != value) {
                    // value has changed
                    config[pinid].isCountClear = true;
                }
                config[pinid].notifyChangeForHigh = value;
                PropertyItems_AddItem(propertyItem, propertyName, TYPE_BOOL, config[pinid].notifyChangeForHigh);
            } else {
                ret = false;
                overWrite[pinid] = false;
            }
        }
    }

    for (int i = 0; i < NUM_DIDO; i++) {
        if (overWrite[i]) {
            vector_add_last(me->mWatchItems, &config[i]);
        }
    }

    if (! vector_is_empty(me->mWatchItems)) {
        // store entity's pointers
        DIDO_WatchItem*	curs = (DIDO_WatchItem*)vector_get_data(me->mWatchItems);

        for (int i = 0, n = vector_size(me->mWatchItems); i < n; ++i) {
            TelemetryItems_AddDictionaryElem(curs->telemetryName, false);
            ++curs;
        }
    }

    return ret;
}

// Get configuration of DIDO contact input watchers
vector
DIDO_WatchConfig_GetFetchItems(DIDO_WatchConfig* me)
{
    return me->mWatchItems;
}

// Get enable port number of DIDO contact input
int
DIDO_WatchConfig_GetWatchEnablePorts(DIDO_WatchConfig* me, bool* status)
{
    DIDO_WatchItem* tmp;
    int enablePort = vector_size(me->mWatchItems);

    for (int i = 0; i < enablePort; i++) {
        tmp = (DIDO_WatchItem*)vector_get_data(me->mWatchItems);
        status[tmp->pinID] = true;
    }

    return enablePort;
}
