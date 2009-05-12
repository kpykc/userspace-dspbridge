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
 *  ======== mgs3/_dmaOmaphw.h ========
 *  This file contains the hardware defines and Macros for the dma device.
 *
 */

#ifndef _DMAHW_
#define _DMAHW


#include <std.h>

#define CCR_CHANENABL    0x80        /* channel enable */
#define CCR_AMODEAUTOINC 0x5000      /* SRC/DST automatic post increment */
#define CCR_END_AUTOINIT 0x0000      /* End Program and Autoinit bits */
#define CCR_HIGHPRIO     (Uns)0x40   /* Set high priority */
#define NO_SRC_DEST_INDEX 0x0        /* Dest indx is same as src index */
#define SRC_DEST_INDEX    0x1        /* Dest indx is different than src indx */

/* Source and addressing modes */
#define SRC_AMODE_CONST   0x0
#define SRC_AMODE_PINC    0x1000
#define SRC_AMODE_SNDX    0x2000
#define SRC_AMODE_DNDX    0x3000
#define DST_AMODE_CONST   0x0
#define DST_AMODE_PINC    0x4000        
#define DST_AMODE_SNDX    0x8000
#define DST_AMODE_DNDX    0xc000

#define IER0_DMAC1       (1 << 9)    /* interrupt enable regs 0 */
#define IER0_DMAC4       (1 << 14)
#define IER0_DMAC5       (1 << 15)

#define IER1_DMAC0       (1 << 2)      /* interrupt enable regs 1 */
#define IER1_DMAC2       (1 << 4)
#define IER1_DMAC3       (1 << 5)

#define CICR_TIMEOUTIE   (1 << 0)      /* interrupt control reg */
#define CICR_BLOCKIE     (1 << 5)
#define CSR_TIMEOUT      (1 << 0)      /* status reg */
#define CSR_BLOCK        (1 << 5)

#define CSDP_DATATYPE     0x3       /* datatype (bits 0-1): 1 = 16 bit */
#define CSDP_SRCSARAM     0x0       /* src saram port  (2-5) */
#define CSDP_SRCDARAM     0x4       /* src daram port   "    */
#define CSDP_SRCEMIF      0x8       /* src emif port    "    */

#define CSDP_SRCPACK_DIS  0x0       /* src packing (6). 0 = disabled */
#define CSDP_SRCBEN_DIS   0x0       /* src bursting (7-8). 00 = disabled */

#define CSDP_SRCPACK_ENA  0x40      /* src packing (6). 1 = enabled */
#define CSDP_SRCBEN_ENA   0x100     /* src bursting (7-8). 10 = enabled for 
                                     * 4x Src_width
                                     */

#define CSDP_DSTSARAM     0x0       /* dst saram port (9-12) */
#define CSDP_DSTDARAM     0x200     /* dst daram port   "    */
#define CSDP_DSTEMIF      0x400     /* dst emif port    "    */

#define CSDP_DSTPACK_DIS  0x0       /* dst packing (13). 0 = disabled */
#define CSDP_DSTBEN_DIS   0x0       /* dst bursting (14-15). 00 = disabled */

#define CSDP_DSTPACK_ENA  0x2000    /* dst packing (13). 1 = enabled */
#define CSDP_DSTBEN_ENA   0x8000    /* src bursting (14-15) 10 = enabled for 
                                     * 4x Dst_width
                                     */
#define CSDP_SRCDSTPKBEN_ENA   0xa140

/* Endianism settings */
#define CCR2_BE_SRC       0x0       /* DST port is Big endian port if EMIF */
#define CCR2_BE_DST       0x0       /* DST port is Big endian port if EMIF */

#define CCR2_LE_SRC       0x8       /* DST port is Little endian port if EMIF */
#define CCR2_LE_DST       0x10       /* DST port is Little endian port if EMIF */
/*
 *  Define address ranges used for dma SRC / DST port values lookup.
 *   Note: These values should be configurable for future memory sizes.
 */
#define DARAMSIZE      ((32L * 1024L) << 1)  /* 32 kwords = 64k bytes */
#define SARAMSIZE      ((48L * 1024L) << 1)  /* 48 kwords = 96k bytes */
#define SDRAMBOTTOM    0x500000L << 1/* 2 Mbyte(1 Megaword) SDRAM window */
#define SDRAMTOP       0x5fffffL << 1
#define ILLEGALPORTVAL 0xffff            /* Invalid port value */


#define SRCPORT 0
#define DSTPORT 1
#define BYTESPERDSPWORD  2        /* 2 bytes per dsp word */

#define MINVAL(x,y)   ((x < y) ? x : y)
#define GETLOWORD(x)  ((Uns)(x & 0xffff))
#define GETHIWORD(x)  ((Uns)((x >> 16) & 0xffff))

typedef struct {
    Uns CSDP;       /* source and destination parameters register */
    Uns CCR;        /* channel control register */
    Uns CICR;       /* interrupt control register*/
    Uns CSR;        /* status register */
    Uns CSSA_L;     /* source start address (low part) */
    Uns CSSA_H;     /* source start address (high part) */
    Uns CDSA_L;     /* destination start address (low part) */
    Uns CDSA_H;     /* destination start address (high part) */
    Uns CEN;        /* element number register */
    Uns CFN;        /* frame number register */
    Uns CSFI;       /* Source frame index register */
    Uns CSEI;       /* Source element index register */
    Uns CSACO;      /* Source Address Counter */
    Uns CDACO;      /* Destination Address Counter */
    Uns CDEI;       /* Destination frame index register */
    Uns CDFI;       /* Destination element index register */
    Uns RSVD1;      /* Not Used */
    Uns RSVD2;      /* Not Used */
    Uns CCR2;       /* Endiansim register */
} DDMA_ChanRegs;

#endif /* _DMAHW_ */
/*
 *  @(#) ti.sdo.fc.acpy2; 1,0,0,20; 5-10-2007 10:49:06; /db/atree/library/trees/fc-b12x/src/
 */

