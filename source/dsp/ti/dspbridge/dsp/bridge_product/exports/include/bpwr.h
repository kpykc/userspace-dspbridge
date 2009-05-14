/*
 *! ============================================================
 *! Copyright 2005 by Texas Instruments Incorporated.
 *! All rights reserved. Property of Texas Instruments Incorporated.
 *! Restricted rights to use, duplicate or disclose this code are
 *! granted through contract.
 *! ============================================================
 */
/**
 *  @file       bpwr.h
 *
 *  @brief      DSP Bridge BPWR Definitions - Provides DSP clock and power
 *              resource management.
 *
 */

/**
 *  @defgroup   DSPBPWR BPWR
 *
 *              The BPWR API manages clock and power resources that impact the
 *              power consumption of the DSP.  Applications can use the API to
 *              directly toggle the power state of a resource, or indirectly
 *              change a resouce's power state through its auto-idling
 *              mechanism, which powers off a resource whenever the DSP is
 *              idled.
 */

#include <std.h>
#include <pwrm.h>
#include <pwrm3430.h>

#ifndef BPWR_
#define BPWR_


/** @ingroup    DSPBPWR */
/*@{*/


#ifdef __cplusplus
extern "C" {
#endif

/** External symbol representing GPTImer clock ID used by BIOS */
#ifdef _6x_
extern far Char BRIDGEINIT_BIOSGPTIMER[];
extern far Char BRIDGEINIT_LOADMON_GPTIMER[];

typedef enum {
    DO_PWRM_DSPINITSTANDBY,
    DO_PWRM_DSPINITRETENTION,
    DO_PWRM_DSPINITHIBERNATE,
    DO_PWRM_MPUINITFORCEDSTANDBY,
    DO_PWRM_MPUINITFORCEDRETENTION,
    DO_PWRM_MPUINITFORCEDHIBERNATE
} BPWR_sleepCode;
#else
extern Char BRIDGEINIT_BIOSGPTIMER[];
#endif


/* Current global state */
typedef struct {
    Uns count;
    Uns numSlots;
    Bool predInit;
    PWRM_CPULoadInfo *cpuLoadInfo;
} _BPWR_predState;

extern _BPWR_predState pdState;

/* Value to indicate that BRIDGEINIT_BIOSGPTIMER has undefined value */
#define BIOSTIMER_NOTENABLED 0xFFFF

#ifdef PWRM_DVFS
/**
 *  @brief      Evaluates whether a setpoint change is needed
 *
 *  @param[in]  value   A value representing predicted load
 */
extern Void BPWR_evaluateSetpoint(Uns value);

/**
 *  @brief      Notification that a setpoint change is occurring
 *
 *  @param[in]  mask   A value representing the setpoint event
 */
extern Void BPWR_setpointEvent(Uns mask);

/**
 *  @brief      Clock and power resources that can be controlled via the
 *              BPWR API.
 */
#endif // PWRM_DVFS

typedef enum {

    /* OMAP1510/1610/1710 specific resources */
    BPWR_OMAP1XXX_DSPWDG_CK = 0x0,  /**< OMAP1xxx DSP watchdog timer
                                       clock. */
    BPWR_OMAP1XXX_DSPXOR_CK,        /**< OMAP1xxx DSP external reference
                                       clock. */
    BPWR_OMAP1XXX_DSPPER_CK,        /**< OMAP1xxx DSP external
                                       peripheral clock. */
    BPWR_OMAP1XXX_DSPTIM_CK,        /**< OMAP1xxx DSP timer clock. */

    /* OMAP242x specific resources */
    BPWR_OMAP242X_GPTimer5 = 0x10,  /**< OMAP242x general purpose timer 5 --
                                       maps to DSP timer 1. */
    BPWR_OMAP242X_GPTimer6,         /**< OMAP242x general purpose timer 6 --
                                       maps to DSP timer 2. */
    BPWR_OMAP242X_GPTimer7,         /**< OMAP242x general purpose timer 7 --
                                       maps to DSP timer 3. */
    BPWR_OMAP242X_GPTimer8,         /**< OMAP242x general purpose timer 8 --
                                       maps to DSP timer 4. */
    BPWR_OMAP242X_WDTimer3,         /**< OMAP242x watchdog timer */
    BPWR_OMAP242X_MCBSP1,           /**< OMAP242x McBSP 1. */
    BPWR_OMAP242X_MCBSP2,           /**< OMAP242x McBSP 2. */
    BPWR_OMAP242X_SSI,              /**< OMAP242x SSI. */

    /* OMAP243X specific resources */
    BPWR_OMAP243X_GPTimer5 = 0x10,  /**< OMAP243X general purpose timer 5 --
                                       maps to DSP timer 1. */
    BPWR_OMAP243X_GPTimer6,         /**< OMAP243X general purpose timer 6 --
                                       maps to DSP timer 2. */
    BPWR_OMAP243X_GPTimer7,         /**< OMAP243X general purpose timer 7 --
                                       maps to DSP timer 3. */
    BPWR_OMAP243X_GPTimer8,         /**< OMAP243X general purpose timer 8 --
                                       maps to DSP timer 4. */
    BPWR_OMAP243X_WDTimer3,         /**< OMAP243X IVA2.1 watchdog timer */
    BPWR_OMAP243X_MCBSP1,           /**< OMAP243X McBSP 1. */
    BPWR_OMAP243X_MCBSP2,           /**< OMAP243X McBSP 2. */
    BPWR_OMAP243X_MCBSP3,           /**< OMAP243X McBSP 3. */
    BPWR_OMAP243X_MCBSP4,           /**< OMAP243X McBSP 4. */
    BPWR_OMAP243X_MCBSP5,           /**< OMAP243X McBSP 5. */
    BPWR_OMAP243X_SSI,              /**< OMAP243X SSI. */

    /* Provided for backward compatibility */
    BPWR_ExtWDTCK  = BPWR_OMAP1XXX_DSPWDG_CK,   /**< DSP watchdog timer clock
                                                   -- provided for backward
                                                   compatibility. */
    BPWR_ExtXORPCK = BPWR_OMAP1XXX_DSPXOR_CK,   /**< DSP external reference
                                                   clock -- provided for
                                                   backward compatibility. */
    BPWR_ExtPERCK  = BPWR_OMAP1XXX_DSPPER_CK,   /**< DSP external peripheral
                                                   clock -- provided for
                                                   backward compatibility. */
    BPWR_ExtTIMCK  = BPWR_OMAP1XXX_DSPTIM_CK,   /**< DSP timer clock --
                                                   provided for backward
                                                   compatibility. */

    BPWR_OMAP_BIOSTIMER = 0xFFF                 /**< Logical resource referring
                                                     to GPTimer used by DSP
                                                     BIOS for all platforms. */
} BPWR_ExtClock;


/**
 *  @brief  BPWR API return codes.
 */
typedef enum {
    BPWR_ClkErrorID = 0,        /**< Invalid clock ID */
    BPWR_ClkOk,
    BPWR_ClkErrorCall,          /**< Excessive number of functions have
                                   referenced this clock. */
    BPWR_ClkTurnedOn,           /**< Clock has been turned on. */
    BPWR_ClkAlreadyOn,          /**< Clock is already ON. */
    BPWR_ClkTurnedOff,          /**< Clock has been turned OFF. */
    BPWR_ClkLeftOn,             /**< Clock is left ON due to pre-existing clock
                                   references. */
    BPWR_ClkIdlingOff,          /**< Clock idling is turned OFF. */
    BPWR_ClkIdlingAlreadyOff,   /**< Clock idling is already OFF due to
                                   pre-existing clock references. */
    BPWR_ClkIdlingOn,           /**< Clock idling has been turned ON. */
    BPWR_ClkIdlingStillOff,     /**< Clock idling has been left OFF due to
                                   pre-existing clock references. */
    BPWR_ClkError               /**< Attempt to enable/disable the clock has
                                   failed. */
} BPWR_ClkStatus;

/**
 *  @brief     The enumeration lists all the notfication events supported
 *             by BPWR.
 */
typedef enum {
    /**
     *  BPWR_PowerDown is called when the DSP is about to go to sleep.
     *  Drivers can get notifications so that they can prepare to sleep (e.g.
     *  stop DMA transfers, turn off peripheral clocks...)
     */
    BPWR_PowerDown,

    /**
     *  BPWR_PowerUp is called when the DSP is about to go to resume after
     *  sleep. Drivers can get notifications so that they can prepare to resume
     *  (e.g. restart DMA transfers, turn on peripheral clocks...)
     */
    BPWR_PowerUp
} BPWR_NotifyEvent;

/**
 *  @brief     The enumeration lists all the User defined resources
 *
 */
typedef enum {
    PWRM_3430_WDTIMER_3 = PWRM_3430_USER_BASE
} PWRM_UserResource;

/**
 *  @brief     Notification function prototype. Any client trying to register a
 *             function to BPWR notification events, needs to match this
 *             prototype
 */
typedef Void (*BPWR_NotifyFxn)(BPWR_NotifyEvent, Arg);

/**
 *  @brief     Opaque handle for a BPWR notification function handle.
 *             This handle is used to unregister a function.
 */
typedef struct BPWR_NotifyObj *BPWR_NotifyHandle;



/**
 *  @brief      Performs BPWR initialization (for OMAP2430).
 *
 *  @retval     status Boolean value; TRUE indicates success, FALSE failure.
 *
 *  @remarks    BPWR_init() will attempt to disable the BIOS timer clock
 *              which would have been enabled for BIOS startup.  It will
 *              invoke BPWR_disableExtClock() with the BIOS timer as well as
 *              its corresponding GPTimer resource.  This only works for
 *              OMAP2430; For 1510/242x, BPWR_init() does nothing.
 *
 *  @sa         BPWR_disableExtClock()
 */
extern Bool BPWR_init ();


/**
 *  @brief      Enables the specified clock.
 *
 *  @param[in]  clock   A valid clock resource defined in @c BPWR_ExtClock.
 *
 *  @retval     status  See defined BPWR API return codes in @c BPWR_ClkStatus.
 *
 *  @pre        DSP Bridge must be initialized. E.g. this function cannot be
 *              called from a device driver initialization function. It can
 *              only be called in a node create, delete or execute phase
 *
 *  @remarks    BPWR_enableExtClock() enables the specified clock
 *              (gated ON), if the clock is not already enabled.
 *              BPWR_enableExtClock() uses reference counting to track whether
 *              a supported clock is enabled. The reference count is incremented
 *              each time an application invokes BPWR_enableExtClock().  It is
 *              decremented whenever an application invokes
 *              BPWR_disableExtClock() is invoked; it is decremented.  The clock
 *              is disabled (gated OFF) whe the reference count reaches 0.
 *
 *  @sa         BPWR_disableExtClock()
 */
extern BPWR_ClkStatus BPWR_enableExtClock (BPWR_ExtClock clock);


/**
 *  @brief      Disables the specified clock.
 *
 *  @param[in]  clock   A valid clock resource defined in @c BPWR_ExtClock.
 *
 *  @retval     status  See defined BPWR API return codes in @c BPWR_ClkStatus.
 *
 *  @retval     BPWR_ClkErrorCall   The reference count on the specified clock
 *                                  is already 0. It cannot be disabled again.
 *
 *  @pre        DSP Bridge must be initialized. E.g. this function cannot be
 *              called from a device driver initialization function. It can
 *              only be called in a node create, delete or execute phase
 *
 *  @remarks    BPWR_disableExtClock(), in conjunction with
 *              BPWR_enableExtClock(), modifies the reference count of a
 *              specified clock. BPWR_enableExtClock() increments the reference
 *              count, BPWR_disableExtClock() decrements the reference count.
 *
 *  @remarks    When an application invokes BPWR_disableExtClock(), the function
 *              performs one of the following actions:
 *              - If the reference count is greater than 1, the function
 *                decrements the reference count and returns.
 *              - If the reference count is 1, the function decrements the
 *                reference count (count is now 0), and disables the specified
 *                clock (gated OFF).
 *              - If the reference count is already 0, it implies the function
 *                was invoked in error. In this case, the function returns
 *                the status code @c BPWR_ClkErrorCall.
 *
 *  @sa         BPWR_enableExtClock()
 */
extern BPWR_ClkStatus BPWR_disableExtClock (BPWR_ExtClock clock);

/**
 *  @brief      This function registers a function to be called when any
 *              BPWR power event occurs. Registrations and the corresponding
 *              notifications are processed in FIFO order.
 *
 *  @param[in]  fxn       function to call on event *  
 *
 *  @retval     NULL      registration failed
 *  @retval     non-NULL  handle for unregistering
 *
 *  @pre        It cannot be called from a SWI or HWI. This is because
 *              BPWR_registerNotify() internally calls MEM_calloc, which
 *              may cause a context switch.
 *
 *  @pre        DSP Bridge must be initialized. E.g. this function cannot be
 *              called from a device driver initialization function. It can
 *              only be called in a node create, delete or execute phase
 *
 *  @remarks    The arg parameter is an arbitrary argument to be passed to the
 *              client upon notification. This argument may allow one notify
 *              function to be used by multiple instances of a driver
 *              (that is, the clientArg can be used to identify the instance
 *              of the driver that is being notified).
 *
 *  @sa         BPWR_unregisterNotify(), BPWR_notify()
 */
extern BPWR_NotifyHandle BPWR_registerNotify(BPWR_NotifyFxn fxn);


/**
 *  @brief      This function unregisters an event notification function that
 *              was registered by BPWR_registerNotify()
 *
 *  @param[in]  handle     handle to registered function
 *
 *  @retval     TRUE       unregistration complete
 *              FALSE      unregistration failed
 *
 *  @pre        It cannot be called from a SWI or HWI. This is because
 *              BPWR_unregisterNotify() internally calls MEM_free, which
 *              may cause a context switch.
 *
 *  @pre        DSP Bridge must be initialized. E.g. this function cannot be
 *              called from a device driver initialization function. It can
 *              only be called in a node create, delete or execute phase
 *
 *  @sa         BPWR_registerNotify(), BPWR_notify()
 */
extern Bool BPWR_unregisterNotify(BPWR_NotifyHandle handle);

#ifdef __cplusplus
}
#endif /* extern "C" */


/*@}*/ /* ingroup DSPBPWR */


#endif /* BPWR_ */
