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
 *  ======== DMAN3 ========
 *  Joule DMA Manager.
 */
@Template("./DMAN3.xdt")

metaonly module DMAN3 {

    /* These const values must match those in dman3.h */
    const Int MAXDMARECS = 32;  /*! Maximum number of DMA handles allowed
                                  for a XDAIS algorithm. */

    const Int MAXGROUPS = 20;  /*! Maximum number of groups for sharing
                                 TCCs and parameter RAM. */

    /*!
     *  ======== qdmaPaRamBase ========
     *  Physical base address of the PARAM0 in the EDMA3/QDMA
     *  hardware whose resources are being managed by DMAN3.
     */
    config Ptr  qdmaPaRamBase =  0x01C04000 ;  /*! This parameter is set to
                                                *  DMAN3_QDMA_PARAM_BASE
                                                */

    /*!
     *  ======== maxPaRamEntries ========
     *  Total number of PARAM Table entries on the target hardware.
     *  (eg, for IVA2, Davinci this is 128, for Himalaya, this is 256).
     */
    config UInt maxPaRamEntries = 128;  /*! This parameter is set to
                                         *  DMAN3_MAX_PARAM_ENTRIES 
                                         */

    /*!
     *  ======== paRamBaseIndex ========
     *  (0 > value > 255) represents the first PARAM TABLE ENTRY
     *  NUMBER that is assigned by configuration for exclusive DMAN3 allocation.
     *  Typically, addresses (PaRam sets) between 0 and 63 are reserved for use 
     *  by drivers that need EDMA/chaining capabilities. Additionally, on
     *  DM6446 PaRams are shared between the Arm and the DSP, and DMAN3 should
     *  be configured to avoid using what the Arm uses. On 2430, the Arm
     *  doesn't have access to EDMA so PaRam sets below 64 are OK -- they won't 
     *  conflict with the Arm.
     */
    config UInt paRamBaseIndex =  96;  /*! This parameter is set to
                                        *  DMAN3_PARAM_BASE_INDEX
                                        */

    /*!
     *  ======== numPaRamEntries ========
     *  Number of PARAM Table entries starting at DMAN3_PARAM_BASE_INDEX
     *  assigned by configuration for exclusive DMAN3 allocation.
     */
    config UInt numPaRamEntries = 32;  /*! This parameter is set to
                                        *  DMAN3_NUM_CONTIGUOUS_PARAM_ENTRIES
                                        */

    /*!
     *  ======== maxQdmaChannels ========
     *  The total number of Physical QDMA channels available on the 
     *  target hardware (eg, 8 for DaVinci, 4 for IVA2).
     */
    config UInt maxQdmaChannels = 8;  /*! This parameter is set to
                                       *  DMAN3_MAX_QDMA_CHANNELS
                                       */

    /*!
     *  ======== numQdmaChannels ========
     *  The number of Physical QDMA channels that are assigned to
     *  DMAN3 via configuration.
     */
    config UInt numQdmaChannels = 4;  /*! This parameter is set to
                                       *  DMAN3_NUM_QDMA_CHANNELS
                                       */

    /*!
     *  ======== qdmaChannels ========
     *  Array containing list of DMAN3_NUM_QDMA_CHANNELS
     *  many Physical QDMA channels assigned to DMAN3 via configuration.
     */
    config UInt qdmaChannels[length] = [0,1,2,3];  /*! This parameter is set to
                                                    *  DMAN3_QDMA_CHANNELS[]
                                                    */

    /*!
     *  ======== tccAllocationMaskH ========
     *  32-bit bitmask representing configuration provided list of TCCs
     *  for exclusive DMAN3 allocation.
     *
     *  For example, for TCC's in the range 0-31 the Low Mask
     *  (tccAllocationMaskL) is configured so that a '1' in bit position 'i'
     *  indicates the TCC 'i' is assigned to DMAN3.
     */
    config UInt tccAllocationMaskH = 0xffffffff; /*! This parameter is set to
                                                  *  DMAN3_TCC_FREEMASK_H
                                                  */

    /*!
     *  ======== tccAllocationMaskL ========
     *  32-bit bitmask representing configuration provided list of TCCs
     *  for exclusive DMAN3 allocation.
     *
     *  For example, for TCC's in the range 0-31 the Low Mask
     *  (tccAllocationMaskL) is configured so that a '1' in bit position 'i'
     *  indicates the TCC 'i' is assigned to DMAN3.
     */
    config UInt tccAllocationMaskL = 0x00000000;/*! This parameter is set to
                                                 *  DMAN3_TCC_FREEMASK_L
                                                 */

    /*
     * TODO:  These "Strings" should be Ints... how do we get a handle to
     * the BIOS objects in javascript-land?
     */

    /*!
     *  ======== heapInternal ========
     *  BIOS Heap ID for dynamic allocation of DMAN3 objects.
     *
     *  This must be allocated in L1D Internal RAM.
     */
    config String heapInternal = "DMAN3_heapInternal_undefined"; /*! This parameter is set to
                                                                  *  DMAN3_HEAP_INTERNAL
                                                                  */
    /*!
     *  ======== heapExternal ========
     *  BIOS Heap ID for dyn allocation of private DMAN3 data structures.
     */
    config String heapExternal = "DMAN3_heapExternal_undefined";  /*! This parameter is set to
                                                                   *  DMAN3_HEAP_EXTERNAL */

    /*!
     *  ======== numTccGroup ========
     *  Addresses of arrays containing the number of TCC that will be
     *  assigned to groups for sharing.
     *
     *  Channels created with a given group ID will use TCCs and PaRams
     *  allocated for that group ID, and may share them with other channels
     *  created with the same group ID.
     */
    config Int numTccGroup[MAXGROUPS] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0,
            0, 0, 0, 0, 0, 0, 0,];    /*! This parameter is set to
                                       *  DMAN3_NUMTCC_GROUP
                                       */

    /*!
     *  ======== numPaRamGroup ========
     *  Addresses of arrays containing the PaRam entries that will be
     *  assigned to groups for sharing.
     *
     *  Channels created with a given group ID will use TCCs and PaRams
     *  allocated for that group ID, and may share them with other channels
     *  created with the same group ID.
     */
    config Int numPaRamGroup[MAXGROUPS]= [0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0,
            0, 0, 0, 0, 0, 0, 0,] ;  /*! This parameter is set to
                                      *  DMAN3_NUMPARAM_GROUP
                                      */

    /*!
     *  ======== scratchAllocFxn ========
     *  Function for allocating IDMA3 channel object's "env" in scratch
     *  memory.
     */
    config String scratchAllocFxn = null;

    /*!
     *  ======== scratchFreeFxn ========
     *  Function for freeing IDMA3 channel object's "env" that was allocated
     *  in scratch memory.
     */
    config String scratchFreeFxn = null;

    /*!
     *  ======== nullPaRamIndex ========
     *  Index of PARAM to be reserved as a "null" PaRam. This index will be
     *  used to set QCHMAP when the corresponding QDMA channel is not active.
     */
    config UInt nullPaRamIndex = 0;    /*! This parameter is set to
                                        *  DMAN3_NULLPARAMINDEX
                                        */

    /*!
     *  ======== idma3Internal ========
     *  Use the internal memory heap for dynamic allocation of IDMA3 objects.
     *
     *  A value of 'false' means that IDMA3 objects will be allocated in the heap
     *  specified by DMAN3_PARAMS.heapExternal. If the value of idma3Internal
     *  is 'true', IDMA3 objects will be allocated in the heap specified
     *  by DMAN3_PARAMS.heapInternal.
     */
    config bool idma3Internal = true;

    /*!
     *  ======== debug ========
     *  Enable the debug profile of the DMAN3 library.
     *
     *  This will likely result in a larger and slower library being linked in,
     *  but it will provide extra parameter checking and debug trace
     *  statements dropped in the BIOS SYS trace buffer.
     */
    config bool debug = false;
}

/*
 *  @(#) ti.sdo.fc.dman3; 1, 0, 1,66; 5-10-2007 10:49:43; /db/atree/library/trees/fc-b12x/src/
 */

