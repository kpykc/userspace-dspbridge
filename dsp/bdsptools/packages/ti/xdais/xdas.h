/* 
 *  Copyright 2006
 *  Texas Instruments Incorporated
 *
 *  All rights reserved.  Property of Texas Instruments Incorporated
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 * 
 */
/*
 *  ======== xdas.h ========
 *  This header defines types and constants used in the XDAS interfaces.
 *  The types are mapped to the types defined in std.h
 */
#ifndef ti_xdais_XDAS_
#define ti_xdais_XDAS_

#ifdef __cplusplus
extern "C" {
#endif

#define XDAS_TRUE       1
#define XDAS_FALSE      0

typedef Void            XDAS_Void;
typedef Uint8           XDAS_Bool;

typedef Int8            XDAS_Int8;      /* Actual size chip dependent */
typedef Uint8           XDAS_UInt8;     /* Actual size chip dependent */
typedef Int16           XDAS_Int16;     /* Actual size of type is 16 bits */
typedef Uint16          XDAS_UInt16;    /* Actual size of type is 16 bits */
typedef Int32           XDAS_Int32;     /* Actual size of type is 32 bits */
typedef Uint32          XDAS_UInt32;    /* Actual size of type is 32 bits */

#ifdef __cplusplus
}
#endif

#endif  /* ti_xdais_XDAS_ */
/*
 *  @(#) ti.xdais 1, 2.0, 0, 0,20 4-20-2006 dais-f06
*/

