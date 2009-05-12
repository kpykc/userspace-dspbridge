%%{
    var numEnabled = 0;
    for (var i = 0; i < this.NUMCHANNELS; i++) {
        if (this.instance("DMAN24XX_CH" + i).enabled == true) {
            numEnabled++;
        }
    }
%%}

%if (numEnabled) {

/* DMAN24XX linker output */
SECTIONS {
        .dman: block(0x20000){
            *(.dman)
        } > `this.OBJMEMSEG.name` PAGE 0
}
%}