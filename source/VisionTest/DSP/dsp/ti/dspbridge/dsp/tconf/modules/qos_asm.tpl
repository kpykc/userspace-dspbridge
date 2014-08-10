
    ;  QoS assembly source output
%if (this.ENABLEQOS) {
    ;  All global symbols needed by dynamic QOS modules;

    .ref _THRLOAD_init;
    .ref _THRLOAD_exit;
    .ref _THRLOAD_initHook;
    .ref _THRLOAD_add;
    .ref _THRLOAD_remove;
    .ref _THRLOAD_createHook;
    .ref _THRLOAD_switchHook;
    .ref _THRLOAD_getTskTime;
    .ref _THRLOAD_update;
    .ref _THRLOAD_idleFxn;
%}
