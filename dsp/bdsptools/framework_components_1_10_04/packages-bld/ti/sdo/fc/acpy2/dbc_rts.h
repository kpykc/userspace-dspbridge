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
 *  ======== dbc_rts.h ========
 *
 * This file contains Non-Bios version of DBC implementation
 *
 */


#ifndef _DBCRTS_
#define _DBCRTS_

#define DBC_ASSERTS     1       /* set to zero to remove assertions */


#if DBC_ASSERTS

extern Void SYS_abort(String s, ...);

#define DBC_assert(c) if (!(c)) { \
        SYS_abort("Assertion Failed: file=%s, line=%d.\n", __FILE__, \
        __LINE__); }

#define DBC_require     DBC_assert
#define DBC_ensure      DBC_assert

#else

#define DBC_assert(c)
#define DBC_require(c)
#define DBC_ensure(c)

#endif


#endif /* _DBCRTS_ */

/*
 *  @(#) ti.sdo.fc.acpy2; 1,0,0,20; 5-10-2007 10:49:05; /db/atree/library/trees/fc-b12x/src/
 */

