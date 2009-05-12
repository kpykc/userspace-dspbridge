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
 *  ======== dman3_api.c ========
 *  DMA Resource manager and utilities for granting logical channel 
 *  resources to algorithms.
 */

#pragma CODE_SECTION(DMAN3_grantDmaChannels, ".text:DMAN3_grantDmaChannels");
#pragma CODE_SECTION(DMAN3_releaseDmaChannels,".text:DMAN3_releaseDmaChannels");

#pragma CODE_SECTION(DMAN3_createChannels, ".text:DMAN3_createChannels");
#pragma CODE_SECTION(DMAN3_freeChannels, ".text:DMAN3_freeChannels");


#include <std.h>
#include <mem.h>

#include <dbc.h>

#include <dman3.h>
#include <dman3_impl.h>
#include <rmm.h>

#include <ti/xdais/idma3.h>


/*
 *  Number of words and MAUs in one PaRam entry. PARAMSIZEMAUS should be
 *  sizeof(ACPY3_PaRamRegs), but use #define since we can't include ACPY3
 *  header file here.
 */
#define PARAMSIZE 8
#define PARAMSIZEMAUS (PARAMSIZE * sizeof(Int))

/*
 *  ======== DMAN3_ContextObj ========
 */
typedef struct DMAN3_ContextObj {
    Int           groupId;
    Int           tccBase;
    Int           numTccs;
    Int           nextFreeTccIdx;
    Int           paRamBaseIdx;
    Uns          *paRamBase;
    Int           numPaRams;
    Int           numAssignedPaRams;
    Uns          *nextFreePaRamAddr; 
    Int           nextQdmaChannel;
    Int           refCount;
} DMAN3_ContextObj;

/*
 *  ======== DMAN3_ChannelObj ========
 *  Structure to store a handle to the DMAN3 context object that
 *  was used to create the IDMA3 channel.
 */
typedef struct DMAN3_ChannelObj {
    IDMA3_Obj           idmaChan;
    DMAN3_Context       context;
    Int                 segEnv;  /* segment id where env is allocated */
    MdUns               sizeEnv; /* size of env */
} DMAN3_ChannelObj, *DMAN3_Channel;

static Int  createChannels(DMAN3_Context context, IALG_Handle alg, Int groupId,
                           IDMA3_ChannelRec dmaTab[], Int numChans);
static Int  createContext(DMAN3_Context *pContext, Int groupId, Int nTccs,
                       Int nPaRams);
static Void freeChannels(IDMA3_ChannelRec dmaTab[], Int numChan);
static Void freeContext(DMAN3_Context context);
static Void freeScratch(Int groupId, IALG_MemRec memTab[], Int numRecs);
static Int  getContext(DMAN3_Context *pContext, Int groupId, Int nTccs,
                       Int nPaRams);
static Bool initHandle(IDMA3_Handle handle, IDMA3_ChannelRec *dmaTab,
                       SmUns *tccList, Uns qdmaChannel,
                       MdInt envSize, Void *env, Void *scratchEnv, Int envSeg,
                       DMAN3_Context context);


/* 
 *  ======== DMAN3_createChannels ======== 
 *  Allocate and initialize memory for channel handle.
 */
Int DMAN3_createChannels(Int groupId, IDMA3_ChannelRec dmaTab[], Int numChans)
{
    Int             i;
    DMAN3_Context   context;
    Int             maxWaits = 0;
    Int             maxTransfers = 0;
    Int             status = DMAN3_SOK;

    DBC_require(groupId < DMAN3_MAXGROUPS);
    DBC_require(numChans >= 0);
    DBC_require((dmaTab != NULL) || (numChans == 0));

    /*
     *  Get the maximum number of waits and transfers to see how many
     *  PaRams and TCCs are needed.
     */
    for (i = 0; i < numChans; i++) {
        DBC_require(dmaTab[i].numWaits > 0);
        DBC_require(dmaTab[i].numTransfers > 0);

        dmaTab[i].handle = NULL;
        maxWaits += dmaTab[i].numWaits;
        maxTransfers += dmaTab[i].numTransfers;
    }

    /* Get the context for this group ID, or create a new one. */
    status = getContext(&context, groupId, maxWaits, maxTransfers);

    /* Now create the channels */
    if (status == DMAN3_SOK) {
        status = createChannels(context, NULL, groupId, dmaTab, numChans);

        /* Free the context object if the refCount is 0 */
        if (context->refCount == 0) {
            if (context->groupId >= 0) {
                _DMAN3_groupContext[context->groupId] = NULL;
            }
            freeContext(context);
        }
    }

    return (status);
}

/* 
 *  ======== DMAN3_grantDmaChannels ======== 
 *  Add an algorithm to the DMA Manager.  The DMA Manager will grant DMA
 *  resources to the algorithm as a result.  This function is called when 
 *  initializing an algorithm instance.
 */
Int DMAN3_grantDmaChannels(Int groupId, IALG_Handle algHandle[],
        IDMA3_Fxns *dmaFxnsArray[], Int numAlgs)
{
    Uns              numChans;
    Int              i, j;
    IALG_Handle      alg;
    IDMA3_Fxns      *dmaFxns;
    IDMA3_ChannelRec *dmaTab;
    Int              maxWaits = 0;
    Int              maxTransfers = 0;
    DMAN3_Context    context;
    Int              status = DMAN3_EFAIL;

    DBC_require(algHandle != NULL);
    DBC_require(dmaFxnsArray != NULL);
    DBC_require(numAlgs > 0);

    /* use dynamic allocation instead of stack to avoid large array on stack */
    dmaTab = (IDMA3_ChannelRec *)MEM_calloc(_DMAN3_heapExt,
            sizeof(IDMA3_ChannelRec) * DMAN3_MAXDMARECS, sizeof(Int));

    if (dmaTab == MEM_ILLEGAL) {
        return (DMAN3_EOUTOFMEMORY);
    }

    /*
     *  Determine the maximum number of TCCs and PaRams that will be needed
     *  by this group of algorithms.
     */
    for (i = 0; i < numAlgs; i++) {
        alg = algHandle[i];
        dmaFxns = dmaFxnsArray[i];

        DBC_require(alg != NULL);
        DBC_require(dmaFxns != NULL);
        
        /*  alg and idma2 fxns must be from same implementation */
        DBC_require(dmaFxns->implementationId == alg->fxns->implementationId);

        /* 
         * In debug build verify that algo is not using two many buffers, 
         * in release build trust that contract is respected 
         */
        DBC_require(dmaFxns->dmaGetChannelCnt() <= DMAN3_MAXDMARECS);

        numChans = dmaFxns->dmaGetChannels(alg, dmaTab);
        DBC_require(numChans <= DMAN3_MAXDMARECS);

        for (j = 0; j < numChans; j++) {
            dmaTab[j].handle = NULL;
            DBC_require(dmaTab[j].numWaits > 0);
            DBC_require(dmaTab[j].numTransfers > 0);

            maxWaits += dmaTab[j].numWaits;
            maxTransfers += dmaTab[j].numTransfers;
        }
    }

    /* Get the context for this group, or create a new one. */
    status = getContext(&context, groupId, maxWaits, maxTransfers);

    if (status == DMAN3_SOK) {
        for (i = 0; i < numAlgs; i++) {
            alg = algHandle[i];
            dmaFxns = dmaFxnsArray[i];

            DBC_require(alg != NULL);
            DBC_require(dmaFxns != NULL);

            /*  alg and idma2 fxns must be from same implementation */
            DBC_require(dmaFxns->implementationId ==
                    alg->fxns->implementationId);

            /* Verify that algo is not using two many buffers */
            DBC_require(dmaFxns->dmaGetChannelCnt() <= DMAN3_MAXDMARECS);

            numChans = dmaFxns->dmaGetChannels(alg, dmaTab);
            DBC_require(numChans <= DMAN3_MAXDMARECS);

            status = createChannels(context, alg, groupId, dmaTab, numChans);

            if ((status == DMAN3_SOK)) {
                if ((status = dmaFxns->dmaInit(alg, dmaTab)) != IALG_EOK) {
                    /* If dmaInit fails, free channel resources for alg[i] */
                    freeChannels(dmaTab, numChans);
                    status = DMAN3_EFAIL;
                }
            }

            if (status != DMAN3_SOK) {
                /* Free channels allocated for alg[0],...,alg[i-1] */
                for (j = 0; j < i; j++) {
                    alg = algHandle[j];
                    dmaFxns = dmaFxnsArray[j];
                    numChans = dmaFxns->dmaGetChannels(alg, dmaTab);

                    /* freeChannels frees context if refCount goes to 0 */
                    freeChannels(dmaTab, numChans);
                }
            }
        }
    }

    MEM_free(_DMAN3_heapExt, dmaTab,
            sizeof(IDMA3_ChannelRec) * DMAN3_MAXDMARECS);

    return (status);
}

/* 
 *  ======== DMAN3_freeChannels ======== 
 *  Free memory for channel handles in dmaTab[].
 */
Int DMAN3_freeChannels(IDMA3_Handle channelTab[], Int numChan)
{
    Int             i;
    IDMA3_Handle    chan;
    DMAN3_Context   context;
    DMAN3_Channel   dmanChan;
    Void           *scratchEnv;
#if DBC_ASSERTS
    Bool            fRet;
#endif

    DBC_require(numChan >= 0);
    DBC_require((channelTab != NULL) || (numChan == 0));

    for (i = 0; i < numChan; i++) {
        
        chan = channelTab[i];
        DBC_require(chan != NULL);

        dmanChan = (DMAN3_Channel)chan;
        context = dmanChan->context;
        DBC_assert(context != NULL);

        context->refCount--;

        /* De-init the environment */
        if (chan->protocol && chan->protocol->deInitHandle) {
#if DBC_ASSERTS
            fRet = chan->protocol->deInitHandle(chan);
            DBC_assert(fRet);
#else
            chan->protocol->deInitHandle(chan);
#endif
        }
        
        /* Free 'env' buffer, but only if it has been allocated. */
        if ((dmanChan->sizeEnv > 0) && (chan->env != NULL)) {

            /*
             *  Free scratchEnv if it has been allocated. The deInit() function
             *  must put the scratch pointer back at the beginning of the env
             *  address.
             */
            scratchEnv = (Void *)(*(LgUns *)(chan->env));

            if ((scratchEnv != NULL) && (_DMAN3_freeScratch != NULL)) {
                (_DMAN3_freeScratch)(context->groupId, scratchEnv,
                        dmanChan->sizeEnv);
            }
            /* Get the memory segment for the environment */
#if DBC_ASSERTS
            fRet = MEM_free(dmanChan->segEnv, chan->env, dmanChan->sizeEnv);
            DBC_assert(fRet);
#else
            MEM_free(dmanChan->segEnv, chan->env, dmanChan->sizeEnv);
#endif
        }

        /* Free DMAN3 channel object memory */
#if DBC_ASSERTS
        fRet = MEM_free(_DMAN3_heapIdma3, chan, sizeof(DMAN3_ChannelObj));
        DBC_assert(fRet);
#else
        MEM_free(_DMAN3_heapIdma3, chan, sizeof(DMAN3_ChannelObj));
#endif
        /* Free the context object if the refCount is 0 */
        if (context->refCount == 0) {
            if (context->groupId >= 0) {
                _DMAN3_groupContext[context->groupId] = NULL;
            }
            freeContext(context);
        }
    }

    return (DMAN3_SOK);
}

/*
 *  ======== DMAN3_releaseDmaChannels ========
 *  Remove logical channel resources from an algorithm instance
 */
Int DMAN3_releaseDmaChannels(IALG_Handle algs[], IDMA3_Fxns *dmaFxnsArray[],
        Int numAlgs)
{
    IALG_Handle         alg;
    IDMA3_Fxns         *dmaFxns;
    IDMA3_ChannelRec    *dmaTab;
    Uns                 nChans;
    Int                 i;

    DBC_require(algs != NULL);
    DBC_require(dmaFxnsArray != NULL);
    DBC_require(numAlgs > 0);

    /* use dynamic allocation instead of stack to avoid large array on stack */
    dmaTab = (IDMA3_ChannelRec *)MEM_calloc(_DMAN3_heapExt,
            sizeof(IDMA3_ChannelRec) * DMAN3_MAXDMARECS, sizeof(Int));

    if (dmaTab == MEM_ILLEGAL) {
        return (DMAN3_EOUTOFMEMORY);
    }

    for (i = 0; i < numAlgs; i++) {
        dmaFxns = dmaFxnsArray[i];
        alg = algs[i];

        DBC_require(dmaFxns->implementationId == alg->fxns->implementationId);

        /* 
         * In debug build verify that algo is not using two many buffers, 
         * in release build trust that contract is respected 
         */
        DBC_require(dmaFxns->dmaGetChannelCnt() <= DMAN3_MAXDMARECS);

        nChans = dmaFxns->dmaGetChannels(alg, dmaTab);
        DBC_require(nChans <= DMAN3_MAXDMARECS);

        freeChannels(dmaTab, nChans);
    }

    MEM_free(_DMAN3_heapExt, dmaTab,
            sizeof(IDMA3_ChannelRec) * DMAN3_MAXDMARECS);

    return (DMAN3_SOK);
}


/*
 *  ======== createChannels ========
 */
static Int createChannels(DMAN3_Context context, IALG_Handle alg, Int groupId,
        IDMA3_ChannelRec dmaTab[], Int numChans)
{
    Void           *env;
    Void           *scratchEnv;
    Int             envSize;
    Int             seg;
    Int             align;
    IDMA3_MemRec    memRec;
    IALG_MemRec    *memTab = NULL;
    IALG_MemAttrs   memType;
    Int             i;
    Uns             qDmaChan;
    SmUns          *tccTablePtr;
    Uns            *paRamAddr;
    Bool            fRet;
    Int             status = DMAN3_SOK;

    DBC_assert(dmaTab != NULL);

    /*
     *  Allocate array of memTab to be used if env can be allocated from
     *  shared scratch memory. Scratch memory for the alg has to be allocated
     *  all at once, so we need to collect the env memory requirements for
     *  each of the alg's channels. All sizes are initialized to 0 here.
     */
    memTab = (IALG_MemRec *)MEM_calloc(_DMAN3_heapExt,
            sizeof(IALG_MemRec) * numChans, sizeof(Int));

    if (memTab == MEM_ILLEGAL) {
        status = DMAN3_EOUTOFMEMORY;
    }
    else {
        /* Type of memory for env allocated in internal memory */
        memType = (_DMAN3_allocScratch != NULL) ? IALG_SCRATCH : IALG_PERSIST;

        /* Fill in memTab with each channels' env memory requirements */
        for (i = 0; i < numChans; i++) {
            memTab[i].attrs = IALG_PERSIST;

            if (dmaTab[i].protocol && dmaTab[i].protocol->getEnvMemRec) {
            
                dmaTab[i].protocol->getEnvMemRec(&dmaTab[i], &memRec);
                memTab[i].size = memRec.size;
                memTab[i].alignment = memRec.align;

                /*
                 *  If memRec.memType = IDMA3_INTERNAL, we arbitrarily set the
                 *  space to IALG_DARAM0.
                 */
                if (memRec.memType == IDMA3_INTERNAL) {
                    memTab[i].space = IALG_DARAM0;
                    memTab[i].attrs = memType;
                }
                else {
                    memTab[i].space = IALG_EXTERNAL;
                }
            }
        }

        /* Allocate scratch env */
        if ((_DMAN3_allocScratch != NULL) &&
                !_DMAN3_allocScratch(alg, groupId, memTab, numChans)) {
            /*
             *  Don't fail if allocation in scratch memory failed. We will
             *  try to allocate in persistent memory.
             */
            for (i = 0; i < numChans; i++) {
                memTab[i].attrs = IALG_PERSIST;
                memTab[i].base = MEM_ILLEGAL;
            }
        }
    }

    if (status != DMAN3_SOK) {
        /* Check the context refCount to see if it should be freed. */
        if (context->refCount == 0) {
            if (context->groupId >= 0) {
                _DMAN3_groupContext[context->groupId] = NULL;
            }
            freeContext(context);
        }
        return (status);
    }

    /* Now we can start creating channels */
    for (i = 0; i < numChans; i++) {
        envSize = memTab[i].size;
        seg = (memTab[i].space == IALG_EXTERNAL) ? _DMAN3_heapExt :
                                                   _DMAN3_heapInt;
        align = memTab[i].alignment;
        scratchEnv = memTab[i].base; /* NULL unless allocated in scratch */

        /* 
         *  Align on word boundary to simplify accesses of IDMA3_Obj
         *  fields by assembly implementations of ACPY3. Allocate more
         *  than IDMA3_Obj, so we can store the context associated with
         *  this channel.
         */
        dmaTab[i].handle = (IDMA3_Obj *)MEM_calloc(_DMAN3_heapIdma3, 
                sizeof(DMAN3_ChannelObj), sizeof(Int));

        if (dmaTab[i].handle == MEM_ILLEGAL) {
            /*
             *  If we failed to create the first channel, then freeChannels()
             *  will not get called to free the context if its ref count is 0.
             *  This can only happen when i = 0.
             */
            if (context->refCount == 0) {
                if (context->groupId >= 0) {
                    _DMAN3_groupContext[context->groupId] = NULL;
                }
                freeContext(context);
            }
            /* Free up the channels that were already allocated. */
            freeChannels(dmaTab, i);

            /* Free scratch memory not freed in call to freeChannels(). */
            freeScratch(groupId, memTab + i, numChans - i);

            status = DMAN3_EOUTOFMEMORY;
            break;
        }
        else {
            /* Assert that Physical DMA resources can be provided */
            DBC_assert(context->nextFreeTccIdx + dmaTab[i].numWaits <=
                    context->tccBase + context->numTccs);
            DBC_assert(context->numAssignedPaRams + dmaTab[i].numTransfers <=
                    context->numPaRams);

            //TODO: need for atomicity when reading/writing context? (nd)
            context->refCount++;

            /* Assign QDMA Channel in a roundrobin fashion */
            qDmaChan = DMAN3_PARAMS.qdmaChannels[context->nextQdmaChannel++
                    % DMAN3_PARAMS.numQdmaChannels];
         
            /* Assign TCCs sequentially from a free list */
            tccTablePtr = &_DMAN3_tccAllocationTable[context->nextFreeTccIdx];
            context->nextFreeTccIdx += dmaTab[i].numWaits;

            /* Allocate the 'env' buffer only when requested (i.e. > 0) */
            if ((envSize > 0)) {
                /*
                 *  If we have env allocated in scratch, we need to allocate
                 *  a persistent copy in external memory.
                 */
                if (scratchEnv != NULL) {
                    seg = _DMAN3_heapExt;
                }
                env = MEM_calloc(seg, envSize, align);
            }
            else {
                env = NULL;
            }

            if ((envSize <= 0) || (env != MEM_ILLEGAL)) {
                /* 
                 * Initialize the IDMA3_Handle handle with resource allocation
                 * & channel state
                 */
                fRet = initHandle(dmaTab[i].handle, &dmaTab[i], tccTablePtr,
                        qDmaChan, envSize, env, scratchEnv, seg, context);

                if (!fRet) {
                    /* free the channels created so far, including this one */
                    freeChannels(dmaTab, i + 1);

                    /* Free remaining scratch */
                    freeScratch(groupId, memTab + i + 1, numChans - i - 1);
                    status = DMAN3_EFAIL;
                    break;
                }
                else {
                    /* Update paRamAddr for next channel */ 
                    paRamAddr = ((Uns *)context->nextFreePaRamAddr) + 
                        (PARAMSIZE * dmaTab[i].numTransfers);

                    /* Set free ptr to next entry */
                    context->nextFreePaRamAddr = ((Uns *)paRamAddr); 
                    context->numAssignedPaRams += dmaTab[i].numTransfers;
                }
            }
            else {
                /* Couldn't allocate env */
                freeChannels(dmaTab, i + 1);

                /* Free remaining scratch */
                freeScratch(groupId, memTab + i + 1, numChans - i - 1);
                status = DMAN3_EOUTOFMEMORY;
                break;
            }
        }
    }

    if (memTab) {
        MEM_free(_DMAN3_heapExt, memTab, sizeof(IALG_MemRec) * numChans);
    }

    return (status);
}


/*
 *  ======== createContext ========
 */
static Int createContext(DMAN3_Context *pContext, Int groupId, Int nTccs,
        Int nPaRams) 
{
    Int             maxTccs;
    Int             maxPaRams;
    Int             paRamBaseIndex;
    DMAN3_Context   context;
    Int             status = DMAN3_SOK;


    *pContext = NULL;

    context = (DMAN3_Context)MEM_calloc(_DMAN3_heapExt,
            sizeof(DMAN3_ContextObj), sizeof(Int));

    if (context == MEM_ILLEGAL) {
        status = DMAN3_EOUTOFMEMORY;
    }
    else {
        /* Initialize the context object */
        context->groupId = groupId;
        context->refCount = 0;

        maxTccs = nTccs;
        maxPaRams = nPaRams;

        if (groupId >= 0) {
            /*
             *  Allocate the maximum of nTccs and the number of TCCs
             *  configured for this groupId.
             */
            maxTccs = (_DMAN3_numTccGroup[groupId] > maxTccs) ?
                _DMAN3_numTccGroup[groupId] : maxTccs;

            /*
             *  Allocate the maximum of nPaRams and the number of PaRams
             *  configured for this groupId.
             */
            maxPaRams = (_DMAN3_numPaRamGroup[groupId] > maxPaRams) ?
                _DMAN3_numPaRamGroup[groupId] : maxPaRams;
        }

        /* Allocate the TCCs */
        if (!RMM_alloc(_DMAN3_rmmTccs, maxTccs, (LgUns *)&(context->tccBase))) {
            MEM_free(_DMAN3_heapExt, context, sizeof(DMAN3_ContextObj));
            return (DMAN3_EOUTOFTCCS);
        }
        else {
            context->numTccs = maxTccs;
            context->nextFreeTccIdx = context->tccBase;
        }

        /* Allocate the PaRam */
        if (!RMM_alloc(_DMAN3_rmmPaRams, maxPaRams, (LgUns *)&paRamBaseIndex)) {
            MEM_free(_DMAN3_heapExt, context, sizeof(DMAN3_ContextObj));
            return (DMAN3_EOUTOFPARAMS);
        }
        else {
            context->numPaRams = maxPaRams;
            context->paRamBaseIdx = paRamBaseIndex; /* For freeing later */
        }

        /* If successful allocation of TCCs and PaRam... */
        if (groupId >= 0) {
            _DMAN3_groupContext[groupId] = context;
        }

        context->paRamBase = (Uns *)((Uns)DMAN3_PARAMS.qdmaPaRamBase +
                (paRamBaseIndex * PARAMSIZEMAUS));
        context->nextFreePaRamAddr = context->paRamBase;
        context->numAssignedPaRams = 0;

        context->nextQdmaChannel = 0;
        *pContext = context;
    }

    return (status);
}

/* 
 *  ======== freeChannels ======== 
 *  Free memory for channel handles in dmaTab[].
 */
static Void freeChannels(IDMA3_ChannelRec dmaTab[], Int numChan)
{
    Int             i;
    IDMA3_Handle    chan;

    for (i = 0; i < numChan; i++) {
        if ((chan = dmaTab[i].handle) != MEM_ILLEGAL) {
            DMAN3_freeChannels(&chan, 1);
            dmaTab[i].handle = NULL;
        }
    }
}

/*
 *  ======== freeContext ========
 *  Free context handle object that was used to start allocating from a fresh 
 *  scratch allocation context. 
 */
static Void freeContext(DMAN3_Context handle)
{
#if DBC_ASSERTS
    Bool    fRet;
#endif

    /* Free the TCCs */
#if DBC_ASSERTS
    fRet = RMM_free(_DMAN3_rmmTccs, handle->tccBase, handle->numTccs);
    DBC_assert(fRet);
#else
    RMM_free(_DMAN3_rmmTccs, handle->tccBase, handle->numTccs);
#endif

    /* Free the PaRams */
#if DBC_ASSERTS
    fRet = RMM_free(_DMAN3_rmmPaRams, handle->paRamBaseIdx, handle->numPaRams);
    DBC_assert(fRet);
#else
    RMM_free(_DMAN3_rmmPaRams, handle->paRamBaseIdx, handle->numPaRams);
#endif

    MEM_free(_DMAN3_heapExt, handle, sizeof(DMAN3_ContextObj));
}

/* 
 *  ======== freeScratch ======== 
 *  Free any scratch memory in memTab[].
 */
static Void freeScratch(Int groupId, IALG_MemRec memTab[], Int numRecs)
{
    Int             i;

    if (_DMAN3_freeScratch != NULL) {
        for (i = 0; i < numRecs; i++) {
            if ((memTab[i].attrs == IALG_SCRATCH) && (memTab[i].size > 0)) {
                _DMAN3_freeScratch(groupId, memTab[i].base, memTab[i].size);
                memTab[i].base = MEM_ILLEGAL;
            }
        }
    }
}

/*
 *  ======== getContext ========
 *  Get context handle for groupId. (In the future, we might have a linked
 *  list of contexts for a given groupId, with varying amounts of TCCs and
 *  PaRams. Therefore, we pass nTccs and nPaRams to this function).
 */
static Int getContext(DMAN3_Context *pContext, Int groupId, Int nTccs,
        Int nPaRams)
{
    DMAN3_Context context = NULL;
    Int status = DMAN3_SOK;

    *pContext = NULL;

    if (groupId >= 0) {
        context = _DMAN3_groupContext[groupId];

        /*
         *  Need to allocate a new context if one exists for the given
         *  groupId, but does not have enough PaRam or TCCs.
         */
        if (context && ((nPaRams > context->numPaRams) ||
                    (nTccs > context->numTccs))) {
            groupId = -1;
            context = NULL;
        }
    }

    /*
     *  Create the context handle if not using any sharing group, or if
     *  using a sharing group but the context for that group has not yet
     *  been allocated, or, if not enough TCCs or PaRams are available.
     */
    if (context == NULL) {
        status = createContext(&context, groupId, nTccs, nPaRams);
        if (status != DMAN3_SOK) {
            return (status);
        }
    }

    DBC_assert(context != NULL);

    /*
     *  Re-initialize next available TCC and PaRam back to the beginning.
     *  The channels allocated in this call will not share TCCs, PaRam.
     */
    context->nextFreeTccIdx = context->tccBase;
    context->nextFreePaRamAddr = context->paRamBase;
    context->numAssignedPaRams = 0;
    *pContext = context;

    return (DMAN3_SOK);
}

/*
 *  ======== initHandle ======== 
 *  Initialize the IDMA3_Handle handle with resource allocation/channel state. 
 */
static Bool initHandle(IDMA3_Handle handle, IDMA3_ChannelRec *dmaTab,
        SmUns *tccList, Uns qdmaChannel, MdInt envSize, Void *env,
        Void *scratchEnv, Int seg, DMAN3_Context context)
{
    Bool          fRet = TRUE;

    handle->numTccs = dmaTab->numWaits;
    handle->numPaRams = dmaTab->numTransfers;
    handle->qdmaChan  = qdmaChannel;
    handle->env  = env;

    if (env) {
        DBC_assert(envSize > 0);
        *(LgUns *)handle->env = (LgUns)scratchEnv;
    }

    handle->protocol = dmaTab->protocol;

    /* Keep these for when we free the channel */
    ((DMAN3_Channel)handle)->context = context;
    ((DMAN3_Channel)handle)->sizeEnv = envSize;
    ((DMAN3_Channel)handle)->segEnv = seg;

    /* 
     *  Assign Physical PARAM Blocks:
     *  Grant "numTransfers" many PARAM blocks, but initialize handle with the
     *  address of the "last" PARAM block.
     */ 
    handle->paRamAddr = ((Uns *)context->nextFreePaRamAddr) + 
        (PARAMSIZE * (dmaTab->numTransfers - 1));

    /* Note: TccTable is provided by the framework. */
    handle->tccTable = tccList;

    /* until first transfer gets started there is no transfer to wait. */
    handle->transferPending = 0;

    /* Call the protocol's init function. */
    if (dmaTab->protocol && dmaTab->protocol->initHandle) {
        fRet = dmaTab->protocol->initHandle(handle);
    }

    return (fRet);
}

/*
 *  @(#) ti.sdo.fc.dman3; 1, 0, 1,66; 5-10-2007 10:49:43; /db/atree/library/trees/fc-b12x/src/
 */

