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
 *  ======== _dskt.h ========
 *  xDAIS socket library definitions.
 *
 */

#ifndef _DSKT_
#define _DSKT_

#ifdef _6x_
extern far Uns _DSKT_DARAM0;
extern far Uns _DSKT_DARAM1;
extern far Uns _DSKT_DARAM2;
extern far Uns _DSKT_SARAM;
extern far Uns _DSKT_SARAM0;
extern far Uns _DSKT_SARAM1;
extern far Uns _DSKT_SARAM2;
extern far Uns _DSKT_IPROG;
extern far Uns _DSKT_EPROG;
extern far Uns _DSKT_ESDATA;
extern far Uns _DSKT_EXTERNAL;
#else
extern Uns _DSKT_DARAM0;
extern Uns _DSKT_DARAM1;
extern Uns _DSKT_DARAM2;
extern Uns _DSKT_SARAM;
extern Uns _DSKT_SARAM0;
extern Uns _DSKT_SARAM1;
extern Uns _DSKT_SARAM2;
extern Uns _DSKT_IPROG;
extern Uns _DSKT_EPROG;
extern Uns _DSKT_ESDATA;
extern Uns _DSKT_EXTERNAL;
#endif

#endif  /* _DSKT_ */

/*
 *  @(#) ti.sdo.fc.dskt2; 1, 0, 1,66; 5-10-2007 10:50:09; /db/atree/library/trees/fc-b12x/src/
 */

