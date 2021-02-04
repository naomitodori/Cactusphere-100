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

#ifndef _PULSE_COUNTER_H_
#define _PULSE_COUNTER_H_

#ifndef _STDBOOL_H
#include <stdbool.h>
#endif
#ifndef _STDINT_H
#include <stdint.h>
#endif

typedef enum {
    FunctionType_NotSelected,
    FunctionType_Single,
    FunctionType_Relation
} FunctionType;

typedef enum {
    RelationType_NotSelected,
    RelationType_Drive,
    RelationType_Interlock,
    RelationType_Invert,
    RelationType_Snap,
    RelationType_Pulse,
    RelationType_PWM
} RelationType;

typedef enum {
    RelationTrigger_None,
    RelationTrigger_Always,
    RelationTrigger_Edge,
    RelationTrigger_,
    RelationTrigger_,
    RelationTrigger_,
    RelationTrigger_,
} RelationTrigger;

typedef enum {
    PulseClock_None = 0,
    PulseClock_32KHz = 32768,
    PulseClock_2MHz = 2000000,
    PulseClock_XTAL = 26000000
} PulseClock;

typedef struct PulseCounter {
    int         pinId;             // DIn pin number
    int         pulseCounter;      // pulse counter value
    int         pulseOnTime;       // time integration of pulse [msec]
    int         pulseOnTimeS;      // time integration of pulse [sec]
    int         pulseElapsedTime;  // current pulse's continuation length
    uint32_t    minPulseSetTime;   // minimum length for settlement as pulse
    uint32_t    maxPulseCounter;   // max pulse counter value
    bool        isCountHight;      // whether settlement as pulse when high(:1) or low(:0) level
    bool        prevState;         // previous state of the DIn pin
    bool        currentState;      // state of the DIn pin (After chattering control)
    bool        isSetPulse;        // is settlement have done
    bool        isRising;          // is DIn level rised
    bool        isStart;           // is this counter running
    bool        defaultState;          // next state of DO pin
    bool        driveState;          // next state of DO pin
    bool        immediateFlag;      // Do immediate
    int         driveTime;
    int         driveElapsedTime;
    bool        driveCertainEnable;
    int         delayTime;
    int         delayElapsedTime;
    bool        delayEnable;
    bool        triggerActived;
    bool        flagDriveTime;
    bool        flagDelay;
    bool        flagA;
    bool        flagB;
    FunctionType functionType;
    RelationType relationType;
    int         relationPort;
} PulseCounter;

typedef struct DoTmpStruct
{
    int         pinId;              // DOut pin number
    bool        defaultState;       // default state of DO pin
    bool        driveState;         // next state of DO pin
    bool        immediateFlag;      // Do immediate
    int         driveTime;
    int         driveElapsedTime;
    bool        driveCertainEnable;
    int         delayTime;
    int         delayElapsedTime;
    bool        delayEnable;
    bool        triggerActived;
    bool        flagDriveTime;
    bool        flagDelay;
    int         pulseEffectiveTime;
    int         pulsePeriod;
    PulseClock  pulseClock;
    bool        flagA;
    bool        flagB;
    FunctionType functionType;
    RelationType relationType;
    int         relationPort;
} DoTmpStruct;


// 仮↓

typedef struct FunctionStandAlone {
    bool        DriveState;         // a
    int         DriveTime;          // a
    int         PWMDutyCycle;       // a
} FunctionStandAlone;

typedef struct FunctionCombination {
    int         CombinationPort;    // a
    int         DIEdgeTrigger;      // a
    int         PulseCycle;         // a
    int         PulseDutyCycle;     // a
} FunctionCombination;

// Initialization
extern void PulseCounter_Initialize(PulseCounter* me, int pin);

// Attribute
extern int  PulseCounter_GetPinId(PulseCounter* me);

// Pulse counter driver operation
extern void PulseCounter_SetConfigCounter(PulseCounter* me,
    bool isCountHigh, int minPulse, int maxPulse);
extern void PulseCounter_Clear(PulseCounter* me, int initValue);
extern int  PulseCounter_GetPulseCount(PulseCounter* me);
extern int  PulseCounter_GetPulseOnTime(PulseCounter* me);
extern bool PulseCounter_GetLevel(PulseCounter* me);
extern bool PulseCounter_GetPinLevel(PulseCounter* me);

// Handle polling based pulse counting task
extern void PulseCounter_Counter(PulseCounter* me);

//DIDO
dido_tmp_move(DoTmpStruct* me)

#endif  // _PULSE_COUNTER_H_
