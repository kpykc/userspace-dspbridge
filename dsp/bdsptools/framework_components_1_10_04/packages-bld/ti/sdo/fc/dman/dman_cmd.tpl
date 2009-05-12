
/* DMAN linker output */
SECTIONS {
        .dman: block(0x20000){
            *(.dman)
        } > `this.OBJMEMSEG.name` PAGE 0
}
