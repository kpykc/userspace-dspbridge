%%{
    var cpu = prog.cpu.attrs.cpuCore;
    var is6x = (cpu[0] == '6') ? true : false;
%%}

%%{
/* do not generate anything here in case of dynamic nodes */
if (!this.BRIDGELIB) {
%%}

/* Bridge.tci content */
%%{
    var dbg = this.enableDebug;
    var list = this.list;
%%}

%%{
/* do not generate anything if there is no init function */
if (list.length != 0) {
%%}

/* Init code generation */
#include <sys.h>
#include <hook.h>

%for (var j = 0 ; j < list.length ; j++) {
/* Module init function from `list[j].name` */
extern Bool `list[j].fxn`();
%}

/*
 * This function is generated only if at least 1 module requires an init 
 * function. It is called in the context of a BIOS HOOK init function.
 * e.g. before main() is called, however most BIOS modules have been 
 * initialized already. Only TSK APIs cannot be called.
 */
Void BRIDGE_initCfg(HOOK_Id id)
{
/* Using bitwise OR to prevent compiler warning */
   id |= id;
   
%for (var j = 0 ; j < list.length ; j++) {
%   if (dbg) {  
    if (`list[j].fxn`() == FALSE) {
        SYS_abort("Init function: `list[j].fxn`() failed...");
    }
%   } else { 
    `list[j].fxn`();
%   }
%}
}

%if (is6x) {
%if (prog.module("PWRM").ENABLE == true) {
far const Uns BRIDGEINIT_DSP_INACTIVITYTIMER_RET = `this.retentionTimeout` / `prog.module("PWRM").TIMERPERIOD`;
far const Uns BRIDGEINIT_DSP_INACTIVITYTIMER_HIB = `this.hibernationTimeout` / `prog.module("PWRM").TIMERPERIOD`;
%}
%}

%%{
} /* end of if (list.length == 0) */
} /* end of if (!this.BRIDGELIB)  */
%%}
