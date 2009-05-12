%%{
    var cpu = prog.cpu.attrs.cpuCore;
    var is6x = (cpu[0] == '6') ? true : false;
    var farString = (is6x) ? "far " : "";
%%}

/* DSM */

typedef struct DSM_Config {
    Uns PIDMASK;
    Uns NUMCHANS;
} DSM_Config;

DSM_Config DSM_config = {
    (Uns)-1,    /* PIDMASK */
    32,         /* NUMCHANS */
};

`farString`DSM_Config *DSM_params = &DSM_config;
