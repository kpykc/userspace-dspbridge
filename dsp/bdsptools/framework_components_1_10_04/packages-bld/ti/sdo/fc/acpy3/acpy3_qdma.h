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
 *  ======== ti/sdo/fc/acpy3/acpy3_qdma.h ========
 *  This header defines hardware QDMA settings used by the
 *  ACPY3 library.
 *
 */

#ifndef ACPY3_QDMA_
#define ACPY3_QDMA_


#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

#ifndef _ACPY3_CPUCOPY_

#define IDMA0_STATUS    ((volatile Uns *)0x01820000)
#define IDMA0_MASK      ((volatile Uns *)0x01820004)
#define IDMA0_SOURCE    ((volatile Uns *)0x01820008)
#define IDMA0_DEST      ((volatile Uns *)0x0182000C)
#define IDMA0_COUNT     ((volatile Uns *)0x01820010)

#define  ACPY3_NUM_QDMA_CHANNELS   4
#define  ACPY3_MAX_QDMA_CHANNELS   8    /* DaVinci has 8; 2430 has 4 */

/* Offsets of EDMA/QDMA peripheral registers from their base address */
#define ACPY3_QCHMAP_OFFSET          0x0200
#define ACPY3_QDMAQNUM_OFFSET        0x0260
#define ACPY3_QUETCMAP_OFFSET        0x0280
#define ACPY3_QUEPRI_OFFSET          0x0284
#define ACPY3_EMR_OFFSET             0x0300
#define ACPY3_EMRH_OFFSET            0x0304
#define ACPY3_EMCR_OFFSET            0x0308
#define ACPY3_EMCRH_OFFSET           0x030C
#define ACPY3_QEMR_OFFSET            0x0310
#define ACPY3_QEMCR_OFFSET           0x0314
#define ACPY3_QRAE0_OFFSET           0x0380
#define ACPY3_QDMA_IPR_OFFSET        0x1068
#define ACPY3_QDMA_IPRH_OFFSET       0x106C
#define ACPY3_QDMA_ICR_OFFSET        0x1070
#define ACPY3_QDMA_ICRH_OFFSET       0x1074
#define ACPY3_QER_OFFSET             0x1080
#define ACPY3_QEER_OFFSET            0x1084
#define ACPY3_QEECR_OFFSET           0x1088
#define ACPY3_QEESR_OFFSET           0x108C
#define ACPY3_QSER_OFFSET            0x1090
#define ACPY3_QSECR_OFFSET           0x1094
#define ACPY3_QDMA_PARAM_OFFSET      0x4000

/*
 * This is the definition of the QDMA/EDMA peripheral registers.
 * They are at a different base depending upon hardware.
 * But the offsets of the particular fields remains the same.
 */
typedef struct {
  Uns filler1[ACPY3_QCHMAP_OFFSET/sizeof(int)];
  Uns qchMap[ACPY3_MAX_QDMA_CHANNELS];    /* ACPY3_QCHMAP */
  Uns filler2[(ACPY3_QDMAQNUM_OFFSET -
              (ACPY3_QCHMAP_OFFSET + (ACPY3_MAX_QDMA_CHANNELS * sizeof(int))))
              / sizeof(int)];
  Uns qdmaQNum;                           /* ACPY3_QDMAQNUM */
  Uns filler3[(ACPY3_QUETCMAP_OFFSET -
              (ACPY3_QDMAQNUM_OFFSET + sizeof(int))) / sizeof(int)];
  Uns queTcMap;                           /* ACPY3_QUETCMAP */
  Uns quePri;                             /* ACPY3_QUEPRI */
  Uns filler4[(ACPY3_EMR_OFFSET -
              (ACPY3_QUEPRI_OFFSET + sizeof(int))) / sizeof(int)];
  Uns emr;                                /* ACPY3_EMR */
  Uns emrH;                               /* ACPY3_EMRH */
  Uns emcr;                               /* ACPY3_EMCR */
  Uns emcrH;                              /* ACPY3_EMCRH */
  Uns qemr;                               /* ACPY3_QEMR */
  Uns qemcr;                              /* ACPY3_QEMCR */
  Uns filler5[(ACPY3_QRAE0_OFFSET -
              (ACPY3_QEMCR_OFFSET + sizeof(int))) / sizeof(int)];
  Uns qrae0;                              /* ACPY3_QRAE0 */
  Uns filler6[(ACPY3_QDMA_IPR_OFFSET -
              (ACPY3_QRAE0_OFFSET + sizeof(int))) / sizeof(int)];
  Uns ipr;                                /* ACPY3_QDMA_IPR */
  Uns iprH;                               /* ACPY3_QDMA_IPRH */
  Uns icr;                                /* ACPY3_QDMA_ICR */
  Uns icrH;                               /* ACPY3_QDMA_ICRH */
  Uns filler7[(ACPY3_QER_OFFSET -
              (ACPY3_QDMA_ICRH_OFFSET + sizeof(int))) / sizeof(int)];
  Uns qer;                                /* ACPY3_QER */
  Uns qeer;                               /* ACPY3_QEER */
  Uns qeecr;                              /* ACPY3_QEECR */
  Uns qeesr;                              /* ACPY3_QEESR */
  Uns qser;                               /* ACPY3_QSER */
  Uns qsecr;                              /* ACPY3_QSECR */
  Uns filler8[(ACPY3_QDMA_PARAM_OFFSET -
              (ACPY3_QSECR_OFFSET + sizeof(int))) / sizeof(int)];
  Uns paRamBase[1];                       /* ACPY3_QDMA_PARAM_OFFSET */
} ACPY3_QDMA_REG_Str;

/*
 * The value DMAN3_EDMA3BASE should be defined as the offset
 * of the QDMA/EDMA registers at link time.
 * For 2430 and DaVinci, the value should be 0x01C00000.
 */
extern volatile ACPY3_QDMA_REG_Str DMAN3_EDMA3BASE;

/* #define  ACPY3_QDMA_BASE   0x01C00000 */

/*
 *  QCHMAP: Queue to channel map register which specifies which parameter
 *  entry in bits 13-5 and the trigger word (which is word 7 in ACPY3
 *  implementation), in bits 4-2.
 */
/* #define ACPY3_QCHMAP ((volatile Uns *)(ACPY3_QDMA_BASE + 0x0200)) */
#define ACPY3_QCHMAP DMAN3_EDMA3BASE.qchMap

/*
 *  QDMAQNUM: The QDMA queue number specifies which queue needs to be used
 *  for each of the individual QDMA channels, 4 in the case of Davinci. This
 *  register is defined in Figure 10 of the 3PCC spec.           
 */
/* #define ACPY3_QDMAQNUM ((volatile Uns *)(ACPY3_QDMA_BASE + 0x0260)) */
#define ACPY3_QDMAQNUM &DMAN3_EDMA3BASE.qdmaQNum

/*
 *  QUETCMAP: This register is specified in Figure 11 of the 3PCC  spec. In the
 *  case of Davinci there are 2 event queues and 2 TC channels. The default
 *  mapping is TC channel0 to Event Queue 0, etc.
 */
/* #define ACPY3_QUETCMAP ((volatile Uns *)(ACPY3_QDMA_BASE + 0x0280)) */
#define ACPY3_QUETCMAP &DMAN3_EDMA3BASE.queTcMap

/*
 *  QUEPRI: This register defines the priority for event queue. There are two
 *  event queues for Davinci, and hence two priorities need to be programmed.
 *  A "higher" value for the priority represents a lower priority.
 */
/* #define ACPY3_QUEPRI ((volatile Uns *)(ACPY3_QDMA_BASE + 0x0284)) */
#define ACPY3_QUEPRI &DMAN3_EDMA3BASE.quePri

/* Event Missed Register */
/* #define ACPY3_EMR   ((volatile Uns *)(ACPY3_QDMA_BASE + 0x0300))  */
#define ACPY3_EMR &DMAN3_EDMA3BASE.emr
/* Event Missed Register High */
/* #define ACPY3_EMRH  ((volatile Uns *)(ACPY3_QDMA_BASE + 0x0304))  */
#define ACPY3_EMRH &DMAN3_EDMA3BASE.emrH
/* Event Missed Clear Register */
/* #define ACPY3_EMCR  ((volatile Uns *)(ACPY3_QDMA_BASE + 0x0308))  */
#define ACPY3_EMCR &DMAN3_EDMA3BASE.emcr
/* Event Missed Clear Register High */
/* #define ACPY3_EMCRH ((volatile Uns *)(ACPY3_QDMA_BASE + 0x030C))  */
#define ACPY3_EMCRH &DMAN3_EDMA3BASE.emcrH
/* QDMA Event Missed Register */
/* #define ACPY3_QEMR  ((volatile Uns *)(ACPY3_QDMA_BASE + 0x0310))  */
#define ACPY3_QEMR &DMAN3_EDMA3BASE.qemr
/* QDMA Event Missed Clear Register */
/* #define ACPY3_QEMCR ((volatile Uns *)(ACPY3_QDMA_BASE + 0x0314))  */
#define ACPY3_QEMCR &DMAN3_EDMA3BASE.qemcr

/*
 *  QRAE0: This register represents the QDMA access enable register. This
 *  register needs to be set if the interrupts for specific TCC's is going
 *  to be used. This library uses polling of the interrupt pending register
 *  and hence does not need to set QRAE0.                             
 */
/* #define ACPY3_QRAE0 ((volatile Uns *)(ACPY3_QDMA_BASE + 0x0380)) */
#define ACPY3_QRAE0 &DMAN3_EDMA3BASE.qrae0

/*
 *  QDMA_IPR : This represents the location where upon completion the DMA
 *  hardware posts completion, if the TCC used is one of the lower 32.
 */
/* #define ACPY3_QDMA_IPR ((volatile Uns *)(ACPY3_QDMA_BASE + 0x1068)) */
#define ACPY3_QDMA_IPR &DMAN3_EDMA3BASE.ipr

/*
 *  QDMA_IPRH : This represents the location where upon completion the DMA
 *  hardware posts completion, if the TCC used is one of the upper 32.
 */
/* #define ACPY3_QDMA_IPRH ((volatile Uns *)(ACPY3_QDMA_BASE + 0x106C)) */
#define ACPY3_QDMA_IPRH &DMAN3_EDMA3BASE.iprH

/*
 *  QDMA_ICR: This represents the interrupt clear reg. Writing to this
 *  register should clear any corresp. bit in QDMA_IPR.
 */
/* #define ACPY3_QDMA_ICR ((volatile Uns *)(ACPY3_QDMA_BASE + 0x1070)) */
#define ACPY3_QDMA_ICR &DMAN3_EDMA3BASE.icr

/*
 *  QDMA_ICRH: This represents the interrupt clear reg. Writing to this
 *  register should clear any corresp. bit in QDMA_IPRH.
 */
/* #define ACPY3_QDMA_ICRH ((volatile Uns *)(ACPY3_QDMA_BASE + 0x1074)) */
#define ACPY3_QDMA_ICRH &DMAN3_EDMA3BASE.icrH

/*
 *  QER: Queue Event register
 */
/* #define ACPY3_QER ((volatile Uns *)(ACPY3_QDMA_BASE + 0x1080)) */
#define ACPY3_QER &DMAN3_EDMA3BASE.qer

/*
 *  QEER: Queue Event Enable register
 */
/* #define ACPY3_QEER ((volatile Uns *)(ACPY3_QDMA_BASE + 0x1084))  */
#define ACPY3_QEER &DMAN3_EDMA3BASE.qeer

/*
 *  QEECR: Queue Event Enable Set register is used to enable all the QDMA
 *  channels. To avoid the problem of read modify write seperate set and
 *  clear registers are provided for enabling the QDMA channels.
 */
/* #define ACPY3_QEECR ((volatile Uns *)(ACPY3_QDMA_BASE + 0x1088)) */
#define ACPY3_QEER &DMAN3_EDMA3BASE.qeer

/* QEESR: register enables the individual QDMA channels to be enabled. */
/* #define ACPY3_QEESR ((volatile Uns *)(ACPY3_QDMA_BASE + 0x108C)) */
#define ACPY3_QEESR &DMAN3_EDMA3BASE.qeesr

/* QDMA Secondary Event Register */
/* #define ACPY3_QSER  ((volatile Uns *)(ACPY3_QDMA_BASE + 0x1090))  */
#define ACPY3_QSER &DMAN3_EDMA3BASE.qser
/* QDMA Secondary Event Clear Register */
/* #define ACPY3_QSECR ((volatile Uns *)(ACPY3_QDMA_BASE + 0x1094))  */
#define ACPY3_QSECR &DMAN3_EDMA3BASE.qsecr

/* #define  ACPY3_QDMA_PARAM_BASE ((Uns)ACPY3_QDMA_BASE + 0x4000) */
#define ACPY3_QDMA_PARAM_BASE ((Uns)DMAN3_EDMA3BASE.paRamBase)

/*
 *  ======== ACPY3_QDMA_Config ========
 *  The module configuration structure for ACPY3 QDMA implementation.
 *  It is set at design time by the system integrator to adjust the behaviour
 *  of the module to be optimal for its execution environment.
 */
typedef struct ACPY3_QDMA_Config {
    /*
     *  Number of Physical QDMA channels reserved for use in ACPY3. It should
     *  be set to a number that does not exceed the hardware limitations.
     */
    MdUns  numQdmaChannels;

    /* List of physical QDMA channels reserved for use by ACPY3. */
    Uns   *qdmaChannels;

    /* Index of PaRam to be used as null PaRam */
    Uns   nullPaRamIndex;

} ACPY3_QDMA_Config;


/*
 *  ======== ACPY3_QDMA ========
 *  Default module configuration structure (defined in acpy2_qdma.c)
 */
extern ACPY3_QDMA_Config ACPY3_QDMA;

#endif  /* _ACPY3_CPUCOPY_ */

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif  /* ACPY3_QDMA_ */


/*
 *  @(#) ti.sdo.fc.acpy3; 1, 0, 0,66; 5-10-2007 10:49:19; /db/atree/library/trees/fc-b12x/src/
 */

