/*
 *! ============================================================
 *! Copyright 2005 by Texas Instruments Incorporated.
 *! All rights reserved. Property of Texas Instruments Incorporated.
 *! Restricted rights to use, duplicate or disclose this code are
 *! granted through contract.
 *! ============================================================
 */
/**
 *  @file       rmstypes.h
 *
 *  @brief      DSP Bridge Resource Manager Server Type Definitions.
 */

/**
 *  @defgroup   DSPRMSTYPES RMS Types 
 *
 *              The Resource Manager Server (RMS) type header defines
 *              special RMS types and the RMS message structure.
 */

#ifndef RMSTYPES_
#define RMSTYPES_


/** @ingroup DSPRMSTYPES */
/*@{*/


#ifdef __cplusplus
extern "C" {
#endif


#ifdef _GPP_

/* GPP-side type definitions */
typedef DWORD   RMS_WORD;
typedef DWORD   RMS_CHAR;
typedef DWORD   RMS_STATUS;

#else /* default to DSP-side */

/* DSP-side definitions */
#include <std.h>

typedef LgUns   RMS_WORD;           /**< 32-bit data type */
typedef Char    RMS_CHAR;           /**< 16-bit data type */
typedef LgUns   RMS_STATUS;         /**< 32-bit data type */
typedef LgUns   (*LgFxn)();         /**< Generic LgUns function type */

#endif


/**
 *  @brief		RMS Message Structure.
 */
typedef struct {
    RMS_WORD	cmd;   	/**< Message code */
    RMS_WORD    arg1;   /**< First message argument */
    RMS_WORD    arg2;   /**< Second message argument */
} RMS_DSPMSG;


#ifdef __cplusplus
}
#endif /* extern "C" */


/*@}*/ /* DSPRMSTYPES */


#endif /* RMSTYPES_ */
