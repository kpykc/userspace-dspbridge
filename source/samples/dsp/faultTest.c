/*
 *  Copyright 2000 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  U.S. Patent Nos. 5,283,900  5,392,448
 */
/* "@(#) DSPBridge 0.85 05-25-00 (__imports)" */
/*
 *  ======== faultTest.c =========
 *
 *! Oct 07,2003  connell created from faultCreate.c
 *
 */


#include <std.h>
#include <stdlib.h>
#include <string.h>
#include <mem.h>
#include <strm.h>
#include <tsk.h>
#include <rms.h>
#include <node.h>
#include <sys.h>
#include <log.h>
#include "faultTest.h"

void generate_fault(FaultObj *pFaultObj)
{
    Int     i;
    MdUns   val;

    switch (pFaultObj->faultParams.errType)
    {

    case MMU_FAULT_READ:
        /* Attempt to read from invalid address */
        for (i = 0; i < pFaultObj->faultParams.nMMUFaults; i++)
        {
#ifdef _64P_
            val = *((short *) 0x48000000);
#else
            val = *((short *) 0x480000);
#endif
        }
        break;
    case DSP_GEN_ERROR:
        generate_message(pFaultObj);
        SYS_error(pFaultObj->errorMessage,TEST_EXCEPTION_VALUE);
        break;
    case MMU_FAULT_BRANCH:
        /* Branch to invalid address */

#ifdef _64P_
	// there is no "goto" command in the 64x assembly language, so
	// store invalid address in a register, then branch to it:

	//store address 0x009000000 into register B4
	asm(" MVKL .S2 0x8000, B4");
	asm(" MVKLH .S2 0x400F, B4");
	//now branch to this invalid address
	asm(" B .S2 B4");
	asm(" NOP 5");

#else
 //       asm(" goto 0x900000");
#endif
        break;
    }
}

void generate_message(FaultObj *pFaultObj)
{
    char *phaseMsg, *errPosMsg;

    switch (pFaultObj->faultParams.phase)
    {
    case CREATE_PHASE:
        phaseMsg = CREATE_PHASE_STRING;
        break;
    case EXECUTE_PHASE:
        phaseMsg = EXECUTE_PHASE_STRING;
        break;
    case DELETE_PHASE:
        phaseMsg = DELETE_PHASE_STRING;
        break;
    }

    if (pFaultObj->faultParams.phase == EXECUTE_PHASE)
    {
        switch (pFaultObj->faultParams.errPos)
        {
        case BEFORE_RECLAIM:
            errPosMsg = BEFORE_RECLAIM_STRING;
            break;
        case AFTER_RECLAIM:
            errPosMsg = AFTER_RECLAIM_STRING;
            break;
        case AFTER_ISSUE:
            errPosMsg = AFTER_ISSUE_STRING;
            break;
        }
    } else
    {
        switch (pFaultObj->faultParams.errPos)
        {
        case START:
            errPosMsg = START_STRING;
            break;
        case MIDDLE:
            errPosMsg = MIDDLE_STRING;
            break;
        case END:
            errPosMsg = END_STRING;
            break;
        }
    }

    pFaultObj->errorMessage = strncat(DSP_GEN_ERROR_STRING, phaseMsg, 30);
    pFaultObj->errorMessage = strncat(pFaultObj->errorMessage, errPosMsg, 30);

}

