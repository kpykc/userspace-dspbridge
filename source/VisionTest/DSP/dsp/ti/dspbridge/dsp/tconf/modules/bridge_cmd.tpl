
SECTIONS {
    .dspbridge_symbols (COPY): {} /* DSP Bridge symbols */
}

%if (this.BRIDGELIB) {

SECTIONS {
    .vers (COPY): {} /* version */
}

SECTIONS {
    .dspbridge_deplibs (COPY): {} /* dependent libraries */
}

SECTIONS {
    .dspbridge_psect (COPY): {} /* split phase libraries */
}

SECTIONS {
    .dspbridge_uuid (COPY): {} /* DSP Bridge UUIDs */
}

SECTIONS {
    .bss: {}
}

SECTIONS {
    .far: {}
}

SECTIONS {
    .const: {}
}

SECTIONS {
    .cinit: {}
}

SECTIONS {
    .text: {}
}

SECTIONS {
    .text:algAlloc: {}
}

SECTIONS {
    .text:exit: {}
}

SECTIONS {
    .text:algFree: {}
}

SECTIONS {
    .text:init: {}
}

SECTIONS {
    .text:algInit: {}
}

SECTIONS {
    .instrumentation: {}
}
%}
