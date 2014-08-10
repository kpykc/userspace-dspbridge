/*
 *! ============================================================
 *! Copyright 2005 by Texas Instruments Incorporated.
 *! All rights reserved. Property of Texas Instruments Incorporated.
 *! Restricted rights to use, duplicate or disclose this code are
 *! granted through contract.
 *! ============================================================
 */
/**
 *  @file       dbc.h
 *
 *  @brief      DSP Bridge Design By Contract Definitions - Provides basic
 *              Design By Contract support, including asserts, preconditions,
 *              and postconditions.
 */

/**
 *  @defgroup   DSPDBC DBC
 *
 *              Design By Contract is a software methodology pioneered by Dr.
 *              Bertrand Meyers.  It utilitizes the concept of preconditions and
 *              postconditions to ensure program correctness.  For additional
 *              reference, please visit
 *              http://archive.eiffel.com/doc/manuals/technology/contract/.
 *
 *  @remarks    To enable the use of DBC in debug builds, add a
 *              "DBC_ASSERTS" define in your makefile or build package.
 */

#ifndef _DBC_
#define _DBC_


/** @ingroup DSPDBC */
/*@{*/


#ifdef __cplusplus
extern "C" {
#endif


/* DBC_ASSERTS is intended to be set to 1 in debug builds */
#if DBC_ASSERTS
#include <sys.h>
#define DBC_assert(c) if (!(c)) { \
        SYS_abort("Assertion Failed: file=%s, line=%d.\n", __FILE__, \
        __LINE__); }            /**< Assert a logical condition to be TRUE. */

#define DBC_require DBC_assert  /**< Assert a precondition to be TRUE. */
#define DBC_ensure  DBC_assert  /**< Assert a postcondition to be TRUE. */

#else

#define DBC_assert(c)   /**< Assert a logical condition to be TRUE. */
#define DBC_require(c)  /**< Assert a precondition to be TRUE. */
#define DBC_ensure(c)   /**< Assert a postcondition to be TRUE. */

#endif


#ifdef __cplusplus
}
#endif /* extern "C" */


/*@}*/ /* ingroup DSPDBC */


#endif /* _DBC_ */
