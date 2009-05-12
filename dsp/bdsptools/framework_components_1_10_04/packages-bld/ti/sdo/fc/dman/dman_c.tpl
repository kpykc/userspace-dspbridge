%%{
    var dmanObjs = this.instances();
    var maxChannels = 6;
    var queueLen = 16;
    var maxQID = 8;
    var numEnabled = this.getNumEnabled();
    var heapId = 0;
    var numDMAN = this.getNumDMAN(); // Number of nodes using DMA

    var nodes = [];
    if (dspbridge.module("NODE") != null) {
        nodes = dspbridge.module("NODE").instances();
    }

    //print("dman_c.tpl: Number of nodes = " + nodes.length);
    //print("dman_c.tpl: Number of nodes using DMA = " + numDMAN);
    //print("dman_c.tpl: Number of enabled DMA channels = " + numEnabled);
    var tabLen = numDMAN;
%%}

%// Only generate if there is at least one DMA channel enabled for DMAN
%if (numEnabled > 0) {
/* DMAN C source output */

#include <ti/sdo/fc/dman/mgs3/_dman.h>

#pragma DATA_SECTION(DMAN_heapId, ".dman")
#pragma DATA_SECTION(DMAN_A_TABBEG, ".dman")
#pragma DATA_SECTION(DMAN_channel, ".dman")
#pragma DATA_SECTION(DMAN_NUMDMAN, ".dman")
#pragma DATA_SECTION(_DMA_jobs, ".dman")
#pragma DATA_SECTION(_DMA_queues, ".dman")
#pragma DATA_SECTION(_DMA_jobQueue, ".dman")

%if (numDMAN > 0) {
%// Generation of table mapping NODE queueId to DMA channel

/* Extern declarations of IDMA fxns */
%for (var i = 0 ; i < nodes.length; i++) {
%    var node = nodes[i];
%    if (node.usesDMA) {
%        var fxns = node.iDMAFxns.name;
%        if (node.iDMAFxns.language == "C") {
%            // Ignore the leading "_" that gets attached with prog.extern()
%            fxns = fxns.substr(1);
%        }
extern IDMA2_Fxns `fxns`;
%    }
%}

//
//  ======== DMAN_A_TABBEG ========
//  Queue ID to DMA channel mappings for nodes configured to use DMA
//
DMAN_Obj DMAN_A_TABBEG[`tabLen`] = {
    /*  selfPtr        q0 q1 q2 q3 q4 q5 q6 q7          idmaFxn    mask rsvd */
%var index = 0;
%for (var i = 0 ; i < nodes.length; i++) {
%    var node = nodes[i];
%    if ((nodes[i].nodeType == "XDAISNODE") && node.usesDMA) {
%        var q0 = node.queueId0;
%        var q1 = node.queueId1;
%        var q2 = node.queueId2;
%        var q3 = node.queueId3;
%        var q4 = node.queueId4;
%        var q5 = node.queueId5;
%        var q6 = node.queueId6;
%        var q7 = node.queueId7;
%        var fxns = node.iDMAFxns.name;
%        if (node.iDMAFxns.language == "C") {
%            // Ignore the leading "_" that gets attached with prog.extern()
%            fxns = fxns.substr(1);
%        }
    {&DMAN_A_TABBEG[`index`], `q0`, `q1`, `q2`, `q3`, `q4`, `q5`, `q6`, `q7`, (Fxn)&`fxns`, 0, 0 },
%        index++;
%    }
%}
};
%} // if (numDMAN > 0)
%else {
DMAN_Obj *DMAN_A_TABBEG = NULL;
%}


Int DMAN_heapId = `heapId`;

/* Channel enable array */
Int DMAN_channel[`maxChannels`] = {
%for (var i = 0 ; i < maxChannels; i++) {
%    var chEnable = "this.CHANNEL" + i + "_ENABLE";
%    if (eval(chEnable) == true) {
         1,    /* channel `i` is used by DMAN */
%    }
%    else {
         -1,   /* channel `i` is not used by DMAN */
%    }
%}   
};

Int DMAN_NUMDMAN = `numDMAN`;    /* Number of elements in DMAN_A_TABBEG */

#include <ti/xdais/idma2.h>

typedef struct _DMA_Job {
    Uns              cnt;
    IDMA2_AdrPtr     src;
    IDMA2_AdrPtr     dst;
    IDMA2_Handle     handle;
    struct _DMA_Job *nextJob;
} _DMA_Job;

typedef struct _DMA_QueueObj {
    IDMA2_Handle          curHandle;
    volatile Int          reqPending;
    Int                   len;
    _DMA_Job             *readPtr;
    _DMA_Job             *writePtr;
    Int                   channelNum;
    Fxn                   cbFxn;
    Arg                   cbArg;
} _DMA_QueueObj;

%var nJobs = queueLen * numEnabled;
_DMA_Job _DMA_jobs[`nJobs`];

_DMA_QueueObj _DMA_queues[`numEnabled`] = {
%for (var i = 0; i < numEnabled; i++) {
%    var idx = i * queueLen;
    { 0, 0, `queueLen`, &_DMA_jobs[`idx`], &_DMA_jobs[`idx`], 0, 0, 0},
%}
};

_DMA_QueueObj *_DMA_jobQueue[`maxChannels`] = {
%var idx = 0;
%for (var i = 0 ; i < maxChannels; i++) {
%    var chEnable = "this.CHANNEL" + i + "_ENABLE";
%    if (eval(chEnable) == true) {
    &_DMA_queues[`idx`],
%        idx++;
%    }
%    else {
         0,
%    }
%}   
};

%} // numEnabled > 0
