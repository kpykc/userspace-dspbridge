/* 
 *  Copyright 2007
 *  Texas Instruments Incorporated
 *
 *  All rights reserved.  Property of Texas Instruments Incorporated
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 * 
 */

/*!
 *  ======== DSKT2 ========
 *  xDAIS algorithm socket
 */
@Template("./DSKT2.xdt")

metaonly module DSKT2 {

    /*!
     *  ======== DEFAULTSEG ========
     *  Default segment to allocate memory from.
     */
    const string DEFAULTSEG = "IRAM";

    /*!
     *  ======== DARAM0 ========
     *  IALG memory space mapping of IALG_DARAM0.  This must match a BIOS heap.
     */
    config string DARAM0     = DEFAULTSEG;

    /*!
     *  ======== DARAM1 ========
     *  IALG memory space mapping of IALG_DARAM1.  This must match a BIOS heap.
     */
    config string DARAM1     = DEFAULTSEG;

    /*!
     *  ======== DARAM2 ========
     *  IALG memory space mapping of IALG_DARAM2.  This must match a BIOS heap.
     */
    config string DARAM2     = DEFAULTSEG;

    /*!
     *  ======== SARAM0 ========
     *  IALG memory space mapping of IALG_SARAM0.  This must match a BIOS heap.
     */
    config string SARAM0     = DEFAULTSEG;

    /*!
     *  ======== SARAM1 ========
     *  IALG memory space mapping of IALG_SARAM1.  This must match a BIOS heap.
     */
    config string SARAM1     = DEFAULTSEG;

    /*!
     *  ======== SARAM2 ========
     *  IALG memory space mapping of IALG_SARAM2.  This must match a BIOS heap.
     */
    config string SARAM2     = DEFAULTSEG;

    /*!
     *  ======== ESDATA ========
     *  IALG memory space mapping of IALG_ESDATA.  This must match a BIOS heap.
     */
    config string ESDATA     = DEFAULTSEG;

    /*!
     *  ======== IPROG ========
     *  IALG memory space mapping of IALG_IPROG.  This must match a BIOS heap.
     */
    config string IPROG      = DEFAULTSEG;

    /*!
     *  ======== EPROG ========
     *  IALG memory space mapping of IALG_EPROG.  This must match a BIOS heap.
     */
    config string EPROG      = DEFAULTSEG;


    /*!
     *  ======== DSKT2_HEAP ========
     *  Memory segment for internal DSKT2 data structures.
     *
     *  Objects internal to the DSKT2 module will be allocated from this heap.
     */
    config string DSKT2_HEAP = DEFAULTSEG;

    /*!
     *  ======== ALLOW_EXTERNAL_SCRATCH ========
     *  Allow external memory to be used for algorithm scratch memory requests.
     *
     *  If an algorithm requests scratch memory, and insufficient scratch
     *  memory is available, scratch buffers will be allocated in external
     *  memory if property is 'true'.
     */
    config bool   ALLOW_EXTERNAL_SCRATCH = true;

    /*!
     *  ======== DARAM_SCRATCH_SIZES ========
     *  Shared IALG_DARAM0 scratch group sizes.
     */
    config UInt   DARAM_SCRATCH_SIZES[] =
                  [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0];

    /*!
     *  ======== SARAM_SCRATCH_SIZES ========
     *  Shared IALG_SARAM0 scratch group sizes.
     */
    config UInt   SARAM_SCRATCH_SIZES[] =
                  [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0];

    /*!
     *  ======== debug ========
     *  Enable the debug profile of the DSKT2 library.
     *
     *  This will likely result in a larger and slower library being linked in,
     *  but it will provide extra parameter checking and debug trace
     *  statements dropped in the BIOS SYS trace buffer.
     */
    config bool debug = false;

    /*!
     *  ======== cacheWritebackInvalidateFxn ========
     *  Name of function implementing cache writeback invalidate with
     *  the following signature: 
     *  (*DSKT2_CacheWBInvFxn)(Ptr blockPtr, size_t byteCnt, bool wait) 
     * 
     *  For C6000 platforms DSKT2 calls this function to ensure that the
     *  algorithm instance memory is not resident in Cache when it is 
     *  instantiated. This is necessary for compliance with XDAIS C6000 DMA
     *  Rules and Guidelines.
     */
    config String cacheWritebackInvalidateFxn = "BCACHE_wbInv";

}

/*
 *  @(#) ti.sdo.fc.dskt2; 1, 0, 1,66; 5-10-2007 10:50:08; /db/atree/library/trees/fc-b12x/src/
 */

