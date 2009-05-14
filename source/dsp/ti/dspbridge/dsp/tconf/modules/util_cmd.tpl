%%{
    // UTIL linker output
    var libSuffix;
    var debugSuffix;
    var instrSuffix;
    var biosGPTimer;
    var loadmonGPTimer;

    // Set archive/library type.
    if (bridge.GBL.DSPTYPE == 55) {
        libSuffix = (bridge.GBL.MEMORYMODEL == "SMALL") ? "a55" : "a55L";
    }
    else if (bridge.GBL.DSPTYPE == 62) {
        libSuffix = (bridge.GBL.ENDIANMODE == "little") ? "a64P" : "a64Pe";
    }
    else {
        throw ("Invalid DSPTYPE specified");
    }

    // Set debug library suffix.
    debugSuffix = (bridge.BRIDGE.enableDebug) ? "d" : "";

    // Set instrumentation suffix if necessary.
    instrSuffix = (bridge.BRIDGE.enableInst) ? "i" : debugSuffix;

    // Set BIOS timer.
    if (bridge.CLK.ENABLECLK) {
        switch (bridge.CLK.TIMERSELECT) {
        case "Timer 5":
            biosGPTimer = 0x10;
            break;

        case "Timer 6":
            biosGPTimer = 0x11;
            break;

        case "Timer 7":
            biosGPTimer = 0x12;
            break;

        case "Timer 8":
            biosGPTimer = 0x13;
            break;

        default:
            biosGPTimer = 0xFFFF;
            break;
        }
    }
    else {
        biosGPTimer = 0xFFFF;
    }

    // Set LOADMONTOR timer.
    if (bridge.PWRM.LOADENABLE) {
        switch (bridge.PWRM.TIMERID) {
        case "Timer 5":
            loadmonGPTimer = 0x10;
            break;

        case "Timer 6":
            loadmonGPTimer = 0x11;
            break;

        case "Timer 7":
            loadmonGPTimer = 0x12;
            break;

        case "Timer 8":
            loadmonGPTimer = 0x13;
            break;

        default:
            loadmonGPTimer = 0xFFFF;
            break;
        }
    }
    else {
        loadmonGPTimer = 0xFFFF;
    }
%%}

%debugSuffix += ("." + libSuffix);
%instrSuffix += ("." + libSuffix);

%if (bridge.BRIDGE.fcInsideBridge == true) {
-ldskt2`debugSuffix`
%}
%if (bridge.BRIDGE.TIOMAPV1035) {
-lbridgev1035`instrSuffix`    /* Bridge library */
%} else {
-lbpwr`debugSuffix`      /* BPWR support -- platform independent lib */
-lbridge`instrSuffix`    /* Bridge library */
%}
%
%if (bridge.QOS.ENABLEQOS) {
-lthrload`debugSuffix` /* QOS support */
%}
%
%if (bridge.BRIDGE.TIOMAP24XX) {
%if (bridge.BRIDGE.fcInsideBridge == true) {
-lacpy2_dma4`debugSuffix`     /* ACPY Library */
-ldman_dma4`debugSuffix`     /* DMAN Library */
%}
%if (bridge.BRIDGE.TIOMAPV1035) {
-lnewdevv1035`debugSuffix`   /* Shared memory driver */
-ltiomapv1035`instrSuffix`   /* TI OMAP-specific library */
%if (bridge.BRIDGE.includeMipcLib) {
-lmipcv1035`debugSuffix`
"mipc.cmd"
%}
%} else {  /*TIOMAP24XX only*/
-lbpwr242x`debugSuffix`     /* BPWR support for OMAP242x */
-lnewdev24xx`debugSuffix`   /* Shared memory driver */
-ltiomap24xx`instrSuffix`   /* TI OMAP-specific library */
%}
%}
%
%if (bridge.BRIDGE.C64P) {
%if (bridge.BRIDGE.fcInsideBridge == true) {
%var acpyUseCpuCopy = (bridge.DMAN3.USECPUCOPY) ? "cpu" : "";
-ldman3`acpyUseCpuCopy``debugSuffix`   /* DMAN Library */
-lacpy3`acpyUseCpuCopy``debugSuffix`   /* ACPY Library */
%}
-lnewdev2430`debugSuffix`   /* Shared memory driver */
-ltiomap24xx`instrSuffix`   /* TI OMAP-specific library */
%if (bridge.BRIDGE.TIOMAP2430){
-lbpwr243x`debugSuffix`      /* BPWR support for OMAP243x */
%}else if (bridge.BRIDGE.TIOMAP3430) {
-lbpwr343x`debugSuffix`      /* BPWR support for OMAP343x */
%}
-ehwi0                       /* Set entry point to HWI reset */
%}
%
%if (bridge.BRIDGE.TIOMAP1510) {
-ldman_mgs3`debugSuffix`       /* DMAN Library */
-lacpy2_mgs3`debugSuffix`       /* ACPY Library */
-lbpwr1510`debugSuffix`   /* BPWR support for OMAP1xxx */
-ltiomap1510`debugSuffix` /* TI OMAP-specific library */
-lnewdev1510`debugSuffix` /* Shared memory driver */
%} // bridge.BRIDGE.TIOMAP1510

_BRIDGEINIT_BIOSGPTIMER = `utils.toHex(biosGPTimer)`;

_BRIDGEINIT_LOADMON_GPTIMER = `utils.toHex(loadmonGPTimer)`;

%if ((bridge.SYS.PUTCFXN.name == "_USR_doPutc") ||
%    (bridge.SYS.PUTCFXN.name == "_USR_doPutcNotifyGPP")) { 
SECTIONS {
    .bridge_trace: fill = 0x0  align = 0x4 {
        _SYS_PUTCBEG = .;
        . += `utils.toHex(bridge.SYS.TRACESIZE)`;
        _SYS_PUTCEND = . - 1;
    } > `bridge.SYS.TRACESEG.name`
}
%}
