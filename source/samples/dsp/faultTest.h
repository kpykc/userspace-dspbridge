/*
 *! ============================================================
 *! Copyright 2004 by Texas Instruments Incorporated.
 *! All rights reserved. Property of Texas Instruments Incorporated.
 *! Restricted rights to use, duplicate or disclose this code are
 *! granted through contract.
 *! ============================================================
 */
/*
 *  Copyright 2000 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  U.S. Patent Nos. 5,283,900  5,392,448
 */
/* "@(#) DSPBridge 0.85 05-25-00 (__imports)" */
/*
 *  ======== faultTest.h =========
 *
 *! Jan 24,2003  mk  Created from fault.c.
 */


#ifndef FAULTTEST_
#define FAULTTEST_

enum
{
    NO_ERROR, MMU_FAULT_READ, DSP_GEN_ERROR, MMU_FAULT_BRANCH
};
enum
{
    CREATE_PHASE = 1, EXECUTE_PHASE, DELETE_PHASE
};
enum
{
    BEFORE_RECLAIM = 1, AFTER_RECLAIM, AFTER_ISSUE
};
enum
{
    START, MIDDLE, END
};

#define DSP_GEN_ERROR_STRING "DSP GENERAL ERROR: "

#define CREATE_PHASE_STRING "Create Phase, "
#define EXECUTE_PHASE_STRING "Execute Phase, "
#define DELETE_PHASE_STRING "Delete Phase, "

#define BEFORE_RECLAIM_STRING "Before Reclaim."
#define AFTER_RECLAIM_STRING "After Reclaim."
#define AFTER_ISSUE_STRING "After Issue."

#define START_STRING "Start."
#define MIDDLE_STRING "Middle."
#define END_STRING "End."

#define TEST_EXCEPTION_VALUE SYS_EUSER

#define TRACE &trace

typedef struct
{
    MdUns       errType;        /* errType: Type of error. 0 = No error, 1 = MMU Fault, */
                                /* 2 = DSP General Exception */
    MdUns       phase;          /* 1 = error occurs during create phase, 2 = execute phase,*/
                                /* 3 = delete phase */
    MdUns       count;          /* Provoque fault when count = 0 */
    MdUns       errPos;         /* For execute phase only. 1 = Before issue, 2 = between */
                                /* issue and reclaim, 3 = after reclaim */
    MdUns       nMMUFaults;     /* Number of MMU Faults to generate */
} FaultParams;

/* Fault task object properties. */
typedef struct
{
    STRM_Handle  inStream;       /* An SIO input channel. */
    STRM_Handle  outStream;      /* An SIO output channel. */
    Ptr         pInBuf;         /* Input data buffer. */
    Ptr         pOutBuf;        /* Output data buffer. */
    Uns         uSegid;         /* Memory segment ID. */
    Int         nInBufsize;     /* Size of input data buffer. */
    Int         nOutBufsize;    /* Size of output data buffer. */
    FaultParams faultParams;
    char        *errorMessage;
} FaultObj;

void generate_fault(FaultObj *pFaultObj);
void generate_message(FaultObj *pFaultObj);

#endif  /* FAULTTEST_ */
