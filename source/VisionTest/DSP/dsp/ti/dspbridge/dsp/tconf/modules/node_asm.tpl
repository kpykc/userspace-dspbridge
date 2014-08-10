
; NODE assembly source output

%%{
    var NODE_DEVICE = 0;
    var NODE_TASK = 1;
    var NODE_DAISSOCKET = 2;

    var STATIC_LOAD = 0;
    var DYNAMIC_LOAD = 1;
    var OVERLAY_LOAD = 2;

    var ts;
    var nodeTypeVal;
    var dynLoadVal;
    var bCacheVal;
    var notifyType;
    var nodes = this.instances();
%%}


%for (var i = 0 ; i < nodes.length ; i++) {
%    var a = nodes[i];
%
%    nodeTypeVal = (a.nodeType == "TASKNODE") ? NODE_TASK :
%        ( (a.nodeType == "DEVNODE") ? NODE_DEVICE : NODE_DAISSOCKET );
%
%    dynLoadVal = (a.loadType == "Static") ? STATIC_LOAD :
%        ( (a.loadType == "Dynamic") ? DYNAMIC_LOAD : OVERLAY_LOAD );
%
%    bCacheVal = a.bCacheOnGPP ? 1 : 0;
%
%    notifyType = (a.sNotifyType == "Post Semaphore") ? 0x8000 : 0x8000;

    .sect ".`a.uuid`"                       ;
    .string "1024,"                         ; cbstruct (NOT USED);
    .string "`a.uuid`,"                     ; uuid;
%
%   if (a.nodeType == "DEVNODE") {
    .string "/`a.name`,"                    ; name;
%   } else {
    .string "`a.name`,"                     ; name;
%   }
%
    .string "`nodeTypeVal`,"                ; type;

    .string "`bCacheVal`,"                  ; (NOT USED);
    .string "`a.dspr_cbStruct`,"            ; (NOT USED);
    .string "`a.dspr_uStaticDataSize`,"     ; (NOT USED);
    .string "`a.dspr_uGlobalDataSize`,"     ; (NOT USED);
    .string "`a.dspr_ProgMemSize`,"         ; (NOT USED);
    .string "`a.dspr_uWCExecutionTime`,"    ; (NOT USED);
    .string "`a.dspr_uWCPeriod`,"           ; (NOT USED);
    .string "`a.dspr_uWCDeadline`,"         ; (NOT USED);
    .string "`a.dspr_uAvgExecutionTime`,"   ; (NOT USED);
    .string "`a.dspr_MinPeriod`,"           ; (NOT USED);
    .string "`a.priority`,"                 ; priority;
    .string "`a.stackSize`,"                ; stack size;
%   if (bridge.GBL.DSPTYPE != "55") {
    .string "`0x10`,"                       ; system stack size (arbitrary)
%   } else {
    .string "`a.sysStackSize`,"             ; system stack size (NOT USED);
%   }

    .string "`a.stackSeg`,"                 ; stack segment;
    .string "`a.messageDepth`,"             ; max message depth queued to node;
    .string "`a.numInputStreams`,"          ; # of input streams;
    .string "`a.numOutputStreams`,"         ; # of output streams;
    .string "`dspbridge.toHex(a.timeout)`," ; timeout value of GPP blocking calls;

    .string "`a.createFxn`,"                ; create phase name;
    .string "`a.executeFxn`,"               ; execute phase name;
    .string "`a.deleteFxn`,"                ; delete phase name;

    .string "`a.uMsgSegid`,"                ; message segment;
    .string "`notifyType`,"                 ; (NOT USED);

    .string "`a.algStructName`,"            ; XDAIS algorithm structure name;
    .string "`dynLoadVal`,"                 ; dynamic loading flag;

    .string "`dspbridge.toHex(a.dataMask)`,"; dynamic load data mem seg mask;
    .string "`dspbridge.toHex(a.codeMask)`,"; dynamic load code mem seg mask;
%   if (bridge.BRIDGE.TINODEHEAP) {
    .string "`this.NUMPROFILES`,"                ; max # of node profiles supported;
    .string "`a.heapSizeProfile0`,"         ; node profile 0;
    .string "`a.heapSizeProfile1`,"         ; node profile 1;
    .string "`a.heapSizeProfile2`,"         ; node profile 2;
    .string "`a.heapSizeProfile3`,"         ; node profile 3;
    .string "`a.heapSizeProfile4`,"         ; node profile 4;
    .string "`a.heapSizeProfile5`,"         ; node profile 5;
    .string "`a.heapSizeProfile6`,"         ; node profile 6;
    .string "`a.heapSizeProfile7`,"         ; node profile 7;
    .string "`a.heapSizeProfile8`,"         ; node profile 8;
    .string "`a.heapSizeProfile9`,"         ; node profile 9;
    .string "`a.heapSizeProfile10`,"        ; node profile 10;
    .string "`a.heapSizeProfile11`,"        ; node profile 11;
    .string "`a.heapSizeProfile12`,"        ; node profile 12;
    .string "`a.heapSizeProfile13`,"        ; node profile 13;
    .string "`a.heapSizeProfile14`,"        ; node profile 14;
    .string "`a.heapSizeProfile15`,"        ; node profile 15;
    .string "`a.stackSegName`,"             ; stackSegName segment;    
%   }

%   if (a.bRegisterObject) {
    .sect ".dcd_register";
    .string "`a.uuid`:0,";
%   }


%    if (nodeTypeVal != NODE_DEVICE) {
    .sect ".dspbridge_symbols"                  ;
    .string "`a.createFxn`,"                ; create phase name;
    .string "`a.executeFxn`,"               ; execute phase name;
    .string "`a.deleteFxn`,"                ; delete phase name;

%        if (nodeTypeVal == NODE_DAISSOCKET) {
    .string "`a.algStructName`,"            ; XDAIS algorithm structure name;
%        }
%    }
%}
