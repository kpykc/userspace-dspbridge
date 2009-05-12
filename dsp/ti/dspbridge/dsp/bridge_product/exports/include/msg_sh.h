/*
 *! ============================================================
 *! Copyright 2004 by Texas Instruments Incorporated.
 *! All rights reserved. Property of Texas Instruments Incorporated.
 *! Restricted rights to use, duplicate or disclose this code are
 *! granted through contract.
 *! ============================================================
 */
/*
 *  ======== msg_sh.h ========
 *
 *  RM Server Messenger shared definitions.
 *
 */

#ifndef MSG_SH_
#define MSG_SH_

/* Masks defining how to notify of a message arrival: */
#define MSG_SEM		0x8000	/* notify destination by posting a semaphore */
#define MSG_SWIPOST 	0x4000	/* notify destination via SWI_post */
#define MSG_SWIANDN 	0x2000	/* notify destination via SWI_andn(swi,0x1) */

#endif /* MSG_SH_ */
