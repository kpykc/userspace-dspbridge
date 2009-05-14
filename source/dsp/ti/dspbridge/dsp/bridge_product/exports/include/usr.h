/*
 *! ============================================================
 *! Copyright 2005 by Texas Instruments Incorporated.
 *! All rights reserved. Property of Texas Instruments Incorporated.
 *! Restricted rights to use, duplicate or disclose this code are
 *! granted through contract.
 *! ============================================================
 */
/**
 *  @file       usr.h
 *
 *  @brief      DSP Bridge USR module. It provides utility functions for the
 *              error handling and trace mechanisms.
 */

/**
 *  @defgroup   DSPUSR USR
 *
 *              USR module provides utility functions for the
 *              error handling mechanisms as well as trace system.
 */

#ifndef USR_
#define USR_

/** @ingroup 	DSPUSR */
/*@{*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @brief      This function can be plugged in the DSP BIOS SYS module, as 
 *              an "SYS_error" handler. It is the default handler for 
 *              SYS_error() for any application using DSP Bridge. 
 *
 *  @param[in]  string          Error string provided by the user, it is
 *                              generally a comment regarding the error
 *  @param[in]  errno           Error code of the error being reported. 
 *                              DSP BIOS defines default error codes in sys.h
 *  @param[in]  vargs           vargs is a void pointer which can be 
 *                              interpreted as an argument list and represents
 *                              the sequence of arg parameters originally 
 *                              passed to SYS_error()
 *
 *  @remarks    This function is the default error handler in DSP Bridge. Every
 *              error is treated as a fatal error, meaning that SYS_abort() is
 *              called eventually. The only exception is for SYS_EALLOC error. 
 *              For memory allocation errors, the function simply returns after
 *              printing an error message in the trace buffer. Memory 
 *              allocation failures need to be handled by the application. 
 *              E.g. applications need
 *              to check the returned address when trying to allocate memory.
 *              A NULL pointer indicates a memory allocation failure.
 *
 *  @remarks    Another error handler can be provided by the base image, if the
 *              default error handling policies does not the expectations of
 *              the base image.
 */
extern Void USR_logError(String string, Int errno, va_list vargs);

/**
 *  @brief      This function can be plugged in the DSP BIOS SYS module, as 
 *              a "SYS_abort" handler. It is the default handler for 
 *              SYS_abort() for any application using DSP Bridge. 
 *
 *  @param[in]  string          formated string provided by the user
 *  @param[in]  vargs           vargs is a void pointer which can be 
 *                              interpreted as an argument list and represents
 *                              the sequence of arg parameters originally 
 *                              passed to SYS_abort()
 *
 *  @remarks    This function is the default handler for SYS_abort() in 
 *              DSP Bridge. When SYS_abort() is called it is expected that DSP 
 *              execution will be stopped. As such the primary purpose of this
 *              function is to notify the MPU side of DSP Bridge that DSP 
 *              execution has stopped, so that MPU Bridge clients can be 
 *              notified.
 * 
 *  @remarks    Another error handler can be provided by the base image, if the
 *              default function does not meet the expectations of the base image. 
 *              In such cases, it is mandatory that the other error handler 
 *              call USR_notifyFatalError() so that MPU Bridge is 
 *              notified that DSP execution has stopped.
 */
extern Void USR_doAbort(String string, va_list vargs);


/**
 *  @brief      This function will notify the MPU side of Bridge that a fatal
 *              error has been detected on the DSP, and that DSP execution is 
 *              about to stop.  
 *
 *  @remarks    This function does not stop the execution of the DSP, it is 
 *              required to stop the execution of the DSP, once this function 
 *              returns
 *
 *  @remarks    This function is used by USR_doAbort()
 *
 *  @remarks    A typical use of this function is when USR_doAbort() is not
 *              plugged into the SYS module as the SYS_abort() handler. For 
 *              such systems that implement their own error handling, it is 
 *              required to call USR_notifyFatalError() before stopping the
 *              execution of the DSP, so that MPU Bridge clients can be
 *              notified.
 */
extern Void USR_notifyFatalError();


/**
 *  @brief      This function can be plugged in the DSP BIOS SYS module, as 
 *              a "SYS_putchar" handler. It is the default handler for
 *              SYS_putchar() for any application using DSP Bridge. 
 *
 *  @param[in]  c       next output character, to be placed in the trace buffer
 *
 *  @remarks    This function writes a character in the system trace buffer. 
 *              The global variable BRIDGE_SYS_PUTC_current indicates the 
 *              position of the next character to be put in the trace buffer, 
 *              it is updated each time a carriage return is printed 
 *              (e.g. '\\n')
 */
extern Void USR_doPutc(Char c);

/**
 *  @brief      This function can be plugged in the DSP BIOS SYS module, as 
 *              a "SYS_putchar" handler. 
 *
 *  @param[in]  c       next output character, to be placed in the trace buffer
 *
 *  @remarks    This function is similar to USR_doPutc(), however it also 
 *              sends a notification to the MPU side of Bridge each time a 
 *              carriage return character is detected (e.g. '\\n')
 */
extern Void USR_doPutcNotifyGPP(Char c);



#ifdef __cplusplus
}
#endif /* extern "C" */


/*@}*/ /* ingroup DSPUSR */

#endif /* USR_ */
