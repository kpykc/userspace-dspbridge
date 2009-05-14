%%{
    var cpu = prog.cpu.attrs.cpuCore;
    var is6x = (cpu[0] == '6') ? true : false;

    // Linker command file output for 55 needs to be in bytes, not MAUs.
    var bytesPerMau = (is6x) ? 1 : 2;

    var _memSeg = "SHMMEM";
    var _shm0RsvdSeg = "SHMSEG0_GPP";

    var _sramBase = 0;
    var _sramSize = 0;
    var _dynExtBase = bridge.MEM.instance("DYNEXTMEM").base;
    var _dynExtEnd = _dynExtBase + this.EXTDYNSIZE - 1;
    var _extBase = _dynExtEnd + 1;
    var _extProgEnd = _extBase + this.EXTMEMSIZE - 1;
    var _shm0base = bridge.MEM.instance("SHMSEG0").base;
    var _shm0End = _shm0base + this.SHAREDHEAPSIZE - 1;

    if (bridge.BRIDGE.C64P == false) {
        _sramBase = bridge.TIOMAP1510SHM.dynSramMemBase;
        _sramSize = bridge.TIOMAP1510SHM.dynSramMemSize;
    }
    else {
        _sramBase =
            dspbridge.module("PROC").instance("PROC_default").virtualEntry2;
        _sramSize = 0x60000;

        var _traceBase = prog.module("MEM").instance("BRIDGE_TRACE").base;
        var _traceEnd = _traceBase + this.TRACESIZE - 1;
    }

    var _sramEnd = _sramBase + _sramSize - 1;

    var _chnlShmBytes = (this.SHAREDMEMSIZE - this.msgShmSize()) * bytesPerMau;
    var _msgShmBytes = this.msgShmSize() * bytesPerMau;

    var _shm0RsvdBytes = (this.SHAREDHEAPSIZE / 2) * bytesPerMau;
%%}


/* MODULE `this.name` */
`this.name`_maxMessages = `this.MAXMESSAGES`;
_EXT_BEG          = `utils.toHex(_extBase)`;
_EXT_END          = `utils.toHex(_extProgEnd)`;
_SHM0_BEG         = `utils.toHex(_shm0base)`;
_SHM0_END         = `utils.toHex(_shm0End)`;
_DYNEXT_BEG       = `utils.toHex(_dynExtBase)`;
_DYNEXT_END       = `utils.toHex(_dynExtEnd)`;
_SRAM_BEG         = `utils.toHex(_sramBase)`;
_SRAM_END         = `utils.toHex(_sramEnd)`;
%if (bridge.BRIDGE.C64P) {
_BRIDGE_TRACE_BEG = `utils.toHex(_traceBase)`;
_BRIDGE_TRACE_END = `utils.toHex(_traceEnd)`;
%}

SECTIONS {
    .shm: {
        _SHM_BEG = .;
        . += `utils.toHex(_chnlShmBytes)`;
        _SHM_END = . - 1;
        _MSG_BEG = .;
        . += `utils.toHex(_msgShmBytes)`;
        _MSG_END = . -1;
    } > `_memSeg`
}

SECTIONS {
    .shm0Resvd: {
        _SHM0_RSVDSTRT = .;
        . += `utils.toHex(_shm0RsvdBytes)`;
    } > `_shm0RsvdSeg`
}
