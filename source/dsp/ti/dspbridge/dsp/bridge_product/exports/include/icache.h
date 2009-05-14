/*
 *! ============================================================
 *! Copyright 2004 by Texas Instruments Incorporated.
 *! All rights reserved. Property of Texas Instruments Incorporated.
 *! Restricted rights to use, duplicate or disclose this code are
 *! granted through contract.
 *! ============================================================
 */
/*
 *  ======== icache.h ========
 *  icache manipulation header file
 *
 */

#ifndef ICACHE_
#define ICACHE_

#include <std.h>

/* BCACHE only exists for Joule/GEM */
#ifdef _64P_
#include <bcache.h>
#endif

#include <std.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DYNAMIC_NODE 1  /**< Use dynamic nodes. */

#ifdef _55_
/* Generic MACRO */
#define REG16(A)            (*(volatile ioport unsigned short*)(A))

/* memory mapped register locations: */
#define ST3_REG             0x04
#define ICACHE_ENABLE_MASK  0x4000
#define ST3_CACLR_BIT       0x2000
#define ICACHE_FLUSH_MASK   0x0800
#define ICACHE_FLUSH 1

#define ENABLE_ICACHE   asm("   bit(ST3, #ST3_CAEN) = #1\n");
#define DISABLE_ICACHE  asm("   bit(ST3, #ST3_CAEN) = #0\n");
#define CLEAR_ICACHE    asm("   bit(ST3, #ST3_CACLR) = #1\n");

/* MACROS for operating ICACHE */
#define ICACHE_CLEAR                            \
    asm("  bit(ST3, #ST3_CACLR) = #1\n");
#define ICACHE_CLEAR_WAIT                                       \
    while(((*(volatile Uns *)ST3_REG) & ST3_CACLR_BIT) != 0);
    
/* ICACHE addresses and Masks */
#define ICACHE_2WAY_16K         1
#define ICACHE_BASE_ADDRESS     0x1400
#define ICACHE_GCR_ADDR         (ICACHE_BASE_ADDRESS)
#define ICACHE_NWCR_ADDR        (ICACHE_BASE_ADDRESS+3)
#define ICACHE_ISR_ADDR         (ICACHE_BASE_ADDRESS+4)
#define ICACHE_ISR_ENABLE_MASK  0x4

#define ICACHE_DISABLE_WAIT                                         \
    while ((REG16(ICACHE_ISR_ADDR)& ICACHE_ISR_ENABLE_MASK) != 0 );
#define ICACHE_ENABLE_WAIT                                          \
    while ((REG16(ICACHE_ISR_ADDR)& ICACHE_ISR_ENABLE_MASK) == 0 );
#define ICACHE_STATUSCHECK                          \
    (*(volatile Uns *)ST3_REG & ICACHE_ENABLE_MASK)

#endif /* _55_ */

#endif /* ICACHE_ */
