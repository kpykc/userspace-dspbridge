/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 *
 *  _inst.h
 *
 * internal header file for instrumentation of the code for benchmarking
 */
#ifndef _INST2
#define _INST2 // avoid multiple inclusions

// include public module definition
#include "inst2.h"

// define dedicated LOG handle
extern LOG_Obj LOG_multimedia;

// INST2 global object (accessible by all the INST2 clients)
far INST2_Obj tInstGlobal_Obj;

// INST2 ARM adapter
typedef struct INST2_Arm_Adapter
{
	LgUns		ulMmMask;
	LgUns		ulBiosMask;
    LgUns       *pMpuTimerMsw;
    LgUns       *pMpuTimerLsw;
	LgUns		bUpdated;
	void        *pNewLogBuf;   
	LgUns 	    ulNewLogSize;
} INST2_Arm_Adapter ;

#endif //_INST
