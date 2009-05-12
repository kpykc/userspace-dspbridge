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
 *  ======== dma4/_dma4_omap.h ========
 *  This file contains object declarations used by the acpy library. 
 */

#ifndef _DMA4_OMAP_
#define _DMA4_OMAP_



/* DMA IRQ line numbers */
#define DMA4_IRQL0              0
#define DMA4_IRQL1              1
#define DMA4_IRQL2              2
#define DMA4_IRQL3              3

/* Corresponding interrupt mapping */
#define DMA4_IRQ0INT            18          /* HWI_INT18 */
#define DMA4_IRQ1INT            9           /* HWI_INT9 */
#define DMA4_IRQ2INT            20          /* HWI_INT20 */
#define DMA4_IRQ3INT            21          /* HWI_INT21 */

/* Mask used to enable interrupt in ier0 or ier1 */
#define DMA4_ier1Mask(intNum)  (((intNum) > 15) ? (1 << ((intNum - 16))) : 0)
#define DMA4_ier0Mask(intNum)  (((intNum) < 16) ? (1 << ((intNum))) : 0)

/*
 *  DMA4_CCR - Channel control register
 *  Bits:
 *    6         - prio (1 = high priority, 0 = low priority)
 *    7         - enable (1 = logical channel is enabled, 0 = disabled)
 *    13:12     - src_amode (00: constant address, 01: post-increment,
 *                          10: single index, 11: double index)
 *    15:14     - dst_amode
 */
#define DMA4_CCR_ENABLE       0x80       /* channel enable */
#define DMA4_CCR_HIGHPRIO     (Uns)0x40  /* Set high priority */

/* Source and addressing modes */
#define DMA4_CCR_AMODE_MASK        0x0000f000
#define DMA4_CCR_SRC_AMODE_CONST   0x0000   /* constant */
#define DMA4_CCR_SRC_AMODE_PINC    0x1000   /* post-increment */
#define DMA4_CCR_SRC_AMODE_SNDX    0x2000   /* single index */
#define DMA4_CCR_SRC_AMODE_DNDX    0x3000   /* double index */
#define DMA4_CCR_DST_AMODE_CONST   0x0
#define DMA4_CCR_DST_AMODE_PINC    0x4000    
#define DMA4_CCR_DST_AMODE_SNDX    0x8000
#define DMA4_CCR_DST_AMODE_DNDX    0xc000
#define DMA4_CCR_AMODEAUTOINC      0x5000   /* src and dst post-increment */

/*
 *  DMA4_CICR - Channel interrupt control register
 *  Bits:
 *    5         - Block_ie (enables end of block interrupt)
 */
#define DMA4_CICR_BLOCKIE     (1 << 5)


/*
 *  DMA4_CSR - Channel status register
 *  Bits:
 *    1         - Drop (synchronization event drop occurred during transfer
 *    5         - Block (end of block event - read 1 --> end of block
 *                                            write 1 --> reset status bit)
 */
#define DMA4_CSR_DROP         (1 << 0)
#define DMA4_CSR_BLOCK        (1 << 5)

/*
 *  DMA4_CSDP - Channel source destination parameters
 *  Bits:
 *    1:0       - Data_type 00 = 8-bit, 01 = 16-bit, 10 = 32-bit, 11 = undef.
 *    6         - src_packed (1 = packed, 0 = not packed)
 *    8:7       - src_burst_en (00: single access, 01: 16-bytes, 10 32-bytes,
 *                              11: 64-bytes)
 *    13        - dst_packed
 *    15:14     - dst_burst_en
 *    18        - dst endianism lock (0 = adapt, 1 = lock)
 *    19        - dst endianness (0 = little endian, 1 = big endian)
 *    20        - src endianism lock (0 = adapt, 1 = lock)
 *    21        - src endianness (0 = little endian, 1 = big endian)
 */
#define DMA4_CSDP_DATATYPE_MASK       0x03  /* Bits 1:0 */
#define DMA4_CSDP_SRCPACK_ENA         0x40
#define DMA4_CSDP_SRCBEN_ENA         0x100
#define DMA4_CSDP_DSTPACK_ENA       0x2000
#define DMA4_CSDP_DSTBEN_ENA        0x8000
#define DMA4_CSDP_SRCDSTPKBEN_ENA   0xa140
#define DMA4_CSDP_SRCDSTPKBEN_MASK  0xe1c0  /* Bits 6:8, 13:15 */
#define DMA4_CSDP_DSTENDIAN_LOCK   0x40000
#define DMA4_CSDP_DST_BIGENDIAN    0x80000
#define DMA4_CSDP_SRCENDIAN_LOCK  0x100000
#define DMA4_CSDP_SRC_BIGENDIAN   0x200000

/*
 *  ======== DMA4_ChnlRegs ========
 */
typedef struct DMA4_ChnlRegs {
    LgUns           ccr;                        /* Offset: 0x80 */
    LgUns           clnkCtrl;                   /* Offset: 0x84 */
    LgUns           cicr;                       /* Offset: 0x88 */
    LgUns           csr;                        /* Offset: 0x8c */
    LgUns           csdp;                       /* Offset: 0x90 */
    LgUns           cen;                        /* Offset: 0x94 */
    LgUns           cfn;                        /* Offset: 0x98 */
    LgUns           cssa;                       /* Offset: 0x9c */
    LgUns           cdsa;                       /* Offset: 0xa0 */
    LgUns           csei;                       /* Offset: 0xa4 */
    LgUns           csfi;                       /* Offset: 0xa8 */
    LgUns           cdei;                       /* Offset: 0xac */
    LgUns           cdfi;                       /* Offset: 0xb0 */
    LgUns           csac;                       /* Offset: 0xb4 */
    LgUns           cdac;                       /* Offset: 0xb8 */
    LgUns           ccen;                       /* Offset: 0xbc */
    LgUns           ccfn;                       /* Offset: 0xc0 */
    LgUns           color;                      /* Offset: 0xc4 */
    LgUns           reserved1;                  /* Offset: 0xc8 */
    LgUns           reserved2;                  /* Offset: 0xcc */
    LgUns           reserved3[4];               /* Offset: 0xd0 - 0xdc */
} DMA4_ChnlRegs;

/*
 *  ======== DMA4_Regs ========
 */
typedef struct DMA4_Regs {
    LgUns           rev;                        /* Offset: 0x0 */
    LgUns           reserved0;                  /* Offset: 0x4 */
    LgUns           irqStatus[4];               /* Offset: 0x8 - 0x17 */
    LgUns           irqEnable[4];               /* Offset: 0x18 - 0x27 */
    LgUns           sysStatus;                  /* Offset: 0x28 */
    LgUns           sysConfig;                  /* Offset: 0x2c */
    LgUns           reserved1[13];              /* Offset: 0x30 - 0x60 */
    LgUns           caps0;                      /* Offset: 0x64 */
    LgUns           reserved2;                  /* Offset: 0x68 (no caps1) */
    LgUns           caps2;                      /* Offset: 0x6c */
    LgUns           caps3;                      /* Offset: 0x70 */
    LgUns           caps4;                      /* Offset: 0x74 */
    LgUns           gcr;                        /* Offset: 0x78 */
    LgUns           reserved3;                  /* Offset: 0x7c */
    DMA4_ChnlRegs   channelRegs[32];
} DMA4_Regs;

extern DMA4_Regs *DMA4_dmaRegs;


#endif /* _DMA4_OMAP_ */
/*
 *  @(#) ti.sdo.fc.acpy2; 1,0,0,20; 5-10-2007 10:49:05; /db/atree/library/trees/fc-b12x/src/
 */

