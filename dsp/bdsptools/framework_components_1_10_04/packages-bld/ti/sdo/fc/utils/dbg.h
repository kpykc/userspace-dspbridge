/* 
 *  Copyright 2007
 *  Texas Instruments Incorporated
 *
 *  All rights reserved.  Property of Texas Instruments Incorporated
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 * 
 */
/*
 *  ======== dbg.h ========
 *
 *  DSP-side debug support.
 *
 */

#ifndef _DBGSUPPORT_
#define _DBGSUPPORT_


#ifdef _DBG_                    /* set when 'dbg' macro used in MAKE file */

#ifdef _DBG_WITH_SYS_
#include <sys.h>
#define TRACE 0 /* unused with _DBG_WITH_SYS, but necessary */

#define DBG_printf0(a,b) SYS_printf(b)
#define DBG_printf1(a,b,c) SYS_printf(b,c)
#define DBG_printf2(a,b,c,d) SYS_printf(b,c,d)

#else /* ! _DBG_WITH_SYS_ */

#include <log.h>
#define TRACE &bridge
extern LOG_Obj bridge;
#define DBG_printf0(a,b) LOG_printf(a,b)
#define DBG_printf1(a,b,c) LOG_printf(a,b,c)
#define DBG_printf2(a,b,c,d) LOG_printf(a,b,c,d)

#endif

#else /* !_DBG_ */

#define DBG_printf0(a,b)
#define DBG_printf1(a,b,c)
#define DBG_printf2(a,b,c,d)

#endif


#endif /* _DBGSUPPORT_ */

/*
 *  @(#) ti.sdo.fc.utils; 1, 0, 0,58; 5-10-2007 10:50:25; /db/atree/library/trees/fc-b12x/src/
 */

