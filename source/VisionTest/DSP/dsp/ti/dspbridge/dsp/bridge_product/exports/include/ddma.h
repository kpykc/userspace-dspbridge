/*
 *! ============================================================
 *! Copyright 2004 by Texas Instruments Incorporated.
 *! All rights reserved. Property of Texas Instruments Incorporated.
 *! Restricted rights to use, duplicate or disclose this code are
 *! granted through contract.
 *! ============================================================
 */

/*
 *  ======== ddma.h ========
 *
 *  DSP/BIOS Bridge DSP-DMA(DDMA) interface
 *
 *
 */

#ifndef DDMA_
#define DDMA_


#define DDMA_HIGHPRI   1
#define DDMA_LOWPRI    0

#define DDMA_DMADEFAULTCHAN 0  /* default dma chnl if not specified in open */

extern DSM_ChannelObj * DDMA_dmaChans;   /* array of channel object ptrs */

/*
 * Global Dsp-Dma configuration params
 */ 

typedef struct DmaConfigObj {

   Void * sm;   /* dma shared memory base */

} DmaConfigObj, * DmaConfig;

/*
 *  ======== DDMA_close ========
 *  Close the DSP-DMA device channel specified.
 */
extern Void DDMA_close(DSM_ChannelObj * chan);


/*
 *  ======== DDMA_idleChan ========
 *  Idle the channel using DMA specified by chan.
 *  Does not idle the DMA device channel itself.
 */
extern Void DDMA_idleChan(DSM_ChannelObj * chan, Bool flush);

/*
 *  ======== DDMA_init ========
 *  Perform DDMA module initialization.
 */
extern Void DDMA_init(Void);


/*
 *  ======== DDMA_open ========
 *  Open the specified sio channel to use a dma device channel.
 */
extern Int DDMA_open(DSM_ChannelObj * chan, Bool hiPri);

/*
 *  ======== DDMA_reqIo ========
 *  Request DSP-DMA processing on the specified channel.
 *  
 */
extern Void DDMA_reqIo(DSM_ChannelObj * chan, Uns arg);



#endif /* DDMA_ */
