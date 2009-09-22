/*
 *  Copyright 2005 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 *
 *  inst2.h
 *
 * public header file for instrumentation of the code for benchmarking
 */

#include <std.h>
#include <clk.h>
#include <mem.h>
#include <node.h>
#include <rms_sh.h>
#include <gbl.h>
#include <tsk.h>
#include <log.h>

#ifndef INST2
#define INST2 // avoid multiple inclusions

// INST2 global object
typedef struct INST2_Obj
{
    LgUns       ulMmMask;
    LgUns       *pMpuTimerMsw;
    LgUns       *pMpuTimerLsw;
    LOG_Event   *pOrigLogBuf;
    LgUns       ulOrigLogSize;
} INST2_Obj ;


//#ifdef _INST2_
// INST2 global object (accessible by all the INST2 clients)
extern far INST2_Obj tInstGlobal_Obj;
//#endif

////////////////////////////////
// define all the possible LOG format
////////////////////////////////

// Format manipulation constants
#define INST2_FORMAT_SHIFT 0
#define INST2_FORMAT_MASK  (0x0007 << INST2_FORMAT_SHIFT)

// Pred-defined record type
#define INST2_FORMAT_BENCH (0x0001 << INST2_FORMAT_SHIFT)
#define INST2_FORMAT_USER0 (0x0002 << INST2_FORMAT_SHIFT) /* Reserved for future usage */
#define INST2_FORMAT_USER1 (0x0004 << INST2_FORMAT_SHIFT) /* Reserved for future usage */
#define INST2_FORMAT_ALL   INST2_FORMAT_MASK

//////////////////////////////
// define constants for FORMAT 0
//////////////////////////////

// Tag manipulation constants
#define INST2_TAG_SHIFT (INST2_FORMAT_SHIFT + 3)
#define INST2_TAG_MASK  (0x03ff << INST2_TAG_SHIFT)

// Pre-defined section tag
#define INST2_TAG_PROCESS (0x0001 << INST2_TAG_SHIFT)
#define INST2_TAG_CONTROL (0x0002 << INST2_TAG_SHIFT)
#define INST2_TAG_RESET   (0x0004 << INST2_TAG_SHIFT)
#define INST2_TAG_NODE    (0x0008 << INST2_TAG_SHIFT)
#define INST2_TAG_WAIT    (0x0010 << INST2_TAG_SHIFT)
#define INST2_TAG_ALGO0   (0x0020 << INST2_TAG_SHIFT)
#define INST2_TAG_ALGO1   (0x0040 << INST2_TAG_SHIFT)
#define INST2_TAG_ALGO2   (0x0080 << INST2_TAG_SHIFT)
#define INST2_TAG_USER0   (0x0100 << INST2_TAG_SHIFT) /* Reserved for future usage */
#define INST2_TAG_USER1   (0x0200 << INST2_TAG_SHIFT) /* Reserved for future usage */
#define INST2_TAG_ALL     INST2_TAG_MASK

// Suffix manipulation constants
#define INST2_SUFFIX_SHIFT (INST2_TAG_SHIFT + 10)
#define INST2_SUFFIX_MASK  (0x000f << INST2_SUFFIX_SHIFT)

// Pre-defined section suffix
#define INST2_SUFFIX_START (0x0001 << INST2_SUFFIX_SHIFT)
#define INST2_SUFFIX_STOP  (0x0002 << INST2_SUFFIX_SHIFT)
#define INST2_SUFFIX_TICKS (0x0004 << INST2_SUFFIX_SHIFT)
#define INST2_SUFFIX_IDEN  (0x0008 << INST2_SUFFIX_SHIFT)

//////////////////////////////
// define constants for FORMAT 1
//////////////////////////////

/* Reserved for future usage */

//////////////////////////////
// define constants for FORMAT 2
//////////////////////////////

/* Reserved for future usage */



//  ===========================================================================
//  func    INST2_init
//  desc    Initialisation of the Bios mask and MM mask
//              This function must be called once at system initialisation (main function)
//
//  arg     MdUns usBiosMask : control traces coming from various Bios modules
//  arg     LgUns ulMmMask  : control traces coming from MM  modules
//
//  ret     Void
//
//  ===========================================================================
extern Void INST2_init(MdUns usBiosMask, LgUns ulMmMask);

#ifdef _INST2_
#define INST2_INIT(a,b) INST2_init(a,b);
#else
#define INST2_INIT(a,b)
#endif
//  ===========================================================================


//  ===========================================================================
//  func    INST2_enable
//  desc    allows to modify Bios mask or MM mask to set some bits
//
//  arg     MdUns usBiosMask : control traces coming from various Bios modules
//  arg     LgUns ulMmMask  : control traces coming from MM  modules
//
//  ret     Void
//
//  ===========================================================================
extern Void  INST2_enable(MdUns usBiosMask, LgUns ulMmMask);

#ifdef _INST2_
#define INST2_ENABLE(a,b) INST2_enable(a,b);
#else
#define INST2_ENABLE(a,b)
#endif
//  ===========================================================================

//  ===========================================================================
//  func    INST2_disable
//  desc    allows to modify Bios mask or MM mask to clear some bits
//
//  arg     MdUns usBiosMask :   control traces coming from various Bios modules
//  arg     LgUns ulMmMask  :    control traces coming from MM  modules
//
//  ret     Void
//
//  ===========================================================================
extern Void  INST2_disable(MdUns usBiosMask, LgUns ulMmMask);

#ifdef _INST2_
#define INST2_DISABLE(a,b) INST2_disable(a,b);
#else
#define INST2_DISABLE(a,b)
#endif
//  ===========================================================================

//  ===========================================================================
//  func    INST2_collect
//  desc    Collects cpu clock ticks, TSK or SWI handle, format as well as user datas.
//
//  arg     Arg format : gives information on the log format used
//  arg     Arg user0 : user value
//  arg     Arg user1  : user value
//  arg     Arg user2 : user value
//
//  ret     Void
//
//  ===========================================================================
extern Void INST2_collect(Arg format, Arg user0, Arg user1, Arg user2);

#ifdef _INST2_
#define INST2_COLLECT(format,b,c,d) \
    if ((format & INST2_FORMAT_ALL) & tInstGlobal_Obj.ulMmMask) { \
        INST2_collect(format,b,c,d); \
    }
#else
#define INST2_COLLECT(a,b,c,d)
#endif
//  ===========================================================================

//  ===========================================================================
//  func    INST2_IDENTIFY
//  desc    Associates SN name (two 4 character words) with task handle.
//              This function must be called once at each SN initialization
//
//  arg     MdUns usBiosMask : control traces coming from various Bios modules
//  arg     LgUns ulMmMask  : control traces coming from MM  modules
//
//  ret     Void
//
//  ===========================================================================
extern Void INST2_idleFxn(Void);
#ifdef _INST2_
#define INST2_IDENTIFY(format, name8, hNode) {\
        INST2_idleFxn(); \
        INST2_collect(format,*(Arg*)name8,*((Arg*)name8+1),(Arg)hNode); \
    }
#else
#define INST2_IDENTIFY(format, name8, hNode)
#endif

#endif //INST
