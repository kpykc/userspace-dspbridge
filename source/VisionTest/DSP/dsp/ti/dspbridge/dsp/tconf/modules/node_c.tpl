%%{
/* do not generate anything here in case of dynamic nodes */
if (!dspbridge.module("BRIDGE").BRIDGELIB) {
%%}

/* NODE C source output */
%%{
    var cpu = prog.cpu.attrs.cpuCore;
    var is6x = (cpu[0] == '6') ? true : false;
    var farString = (is6x) ? "far " : "";

    var maxNodeHeaps = this.MAXNODEHEAPS;
    var heap = this.MSGFRAMESEG;

    // Don't want leading '_' in heap label.
    if (this.MSGFRAMESEG.heapLabel.language == "C") {
        var heapId = heap.heapLabel.name.substr(1);
    }
    else {
        var heapId = heap.heapLabel.name;
    }
%%}

#include <node.h>

%if (dspbridge.module("BRIDGE").TINODEHEAP) {
NODE_Config NODE_config = {-1, `maxNodeHeaps`};
%} else {
NODE_Config NODE_config = {-1, 0};
%}
NODE_Config *NODE = &NODE_config;

extern `farString`Int `heapId`;

Bool NODE_initCfg() {
    NODE_config.MSGFRAMESEG = `heapId`;
    return TRUE;
}

%%{
} /* end of if (!dspbridge.module("BRIDGE").BRIDGELIB) */
%%}
