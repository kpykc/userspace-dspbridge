%%{
    var processors = this.instances();
%%}


%for (var i = 0 ; i < processors.length ; i++) {
%    var a = processors[i];

    .sect ".`a.uiProcessorID`:"         ; processor id;
    .string "`a.cbStruct`,"             ; cbStruct;
    .string "`a.uProcessorFamily`,"     ; processor family;
    .string "`a.uProcessorType`,"       ; processor type;
    .string "`a.uClockRate`,"           ; clock rate;
    .string "`a.ulInternalMemSize`,"    ; internal memory size;
    .string "`a.ulExternalMemSize`,"    ; external memory size;
    .string "`a.uUserID`,"              ; user id;
    .string "`a.tyRunningRTOS`,"        ; running RTOS;
    .string "`a.nNodeMinPriority`,"     ; minimum node priority;
    .string "`a.nNodeMaxPriority`,"     ; maximum node priority;
    .string "`a.physicalEntry0`,"          ; physical address 0;
    .string "`a.virtualEntry0`,"          ; virtual address 0;
    .string "`a.physicalEntry1`,"          ; physical address 1;
    .string "`a.virtualEntry1`,"          ; virtual address 1;
    .string "`a.physicalEntry2`,"          ; physical address 2;
    .string "`a.virtualEntry2`,"          ; virtual address 2;
    .string "`a.physicalEntry3`,"          ; physical address 3;
    .string "`a.virtualEntry3`,"          ; virtual address 3;
    .string "`a.physicalEntry4`,"          ; physical address 4;
    .string "`a.virtualEntry4`,"          ; virtual address 4;
    .string "`a.physicalEntry5`,"          ; physical address 5;
    .string "`a.virtualEntry5`,"          ; virtual address 5;
    .string "`a.physicalEntry6`,"          ; physical address 6;
    .string "`a.virtualEntry6`,"          ; virtual address 6;

%   if (a.bRegisterObject) {
    .sect ".dcd_register";
    .string "`a.uiProcessorID`:1,";
%   }
%}
