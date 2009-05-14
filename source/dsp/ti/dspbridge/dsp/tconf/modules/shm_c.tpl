%%{
    var cpu = prog.cpu.attrs.cpuCore;
    var is6x = (cpu[0] == '6') ? true : false;
    var farString = (is6x) ? "far " : "";
    var shmSize = this.confBufSize();
%%}

/* SHM C source output */
`farString`Uns SHM_confBufSize = `utils.toHex(shmSize)`;
`farString`Uns SHM_maxMessages = `this.MAXMESSAGES`;

#include "msg.h"
%if (!is6x) {
%%{
        var inst = this.instances();
        var count = 0; //loop counter
        var cmdTab_init = ""; //used to initialized SHM_cmdTab array
        var declarations = ""; //used for function declarations
        var table_len = inst.length ;
        for (var i = 0 ; i < inst.length ; i++) 
        {
        	count++;
        	//set up variable used to declare functions
        	declarations += "extern short " + inst[i].fxn + "(); ";
        	cmdTab_init += "{";
        	cmdTab_init += inst[i].fxn;
        	cmdTab_init += ","; 
        	cmdTab_init += inst[i].id;
        	cmdTab_init += ",";
        	cmdTab_init += inst[i].ack;
        	cmdTab_init += "}";
        	if( count < (inst.length) ) 
        	{
        		cmdTab_init += ",";
        	}
        }
%%}



% var inst = this.instances();
Uns SHM_cmdTabLen = `table_len` ;
`declarations`
	%if(this.instances().length > 0) {
		SHM_customCmd SHM_cmdTab[] = { `cmdTab_init` } ; 
	%} else {
		SHM_customCmd SHM_cmdTab[];
	%}
Uns SHM_mbxID = `this.MBXID`;
%}
