%%{
    var dynmobjs = this.instances();
    var cpu = prog.cpu.attrs.cpuCore;
    var is6x = (cpu[0] == '6') ? true : false;
    var word = (is6x) ? ".word" : ".long";
%%}

    .sect ".dspbridge_mem"            ;
    `word` `this.NUMDYNMEM`      ; number of mem segments;


%if(is6x) {
	%for (var i = 0 ; i < dynmobjs.length ; i++) {
	%    var a = dynmobjs[i];
	
	    .sect ".dspbridge_mem"            ;
	    `word` `a.id`                ; segment id;
	    `word` `a.base`              ;
	    `word` `a.len`               ;
	    `word` `a.memType`           ;

	%}
%} else {

	%%{
		var dynm = dspbridge.module("DYNM");
	
		var dynm_arr = new Array();	
		dynm_arr[0] = dynm.instance("DYNM_0");
		dynm_arr[1] = dynm.instance("DYNM_1");
		dynm_arr[2] = dynm.instance("DYNM_2");
		dynm_arr[3] = dynm.instance("DYNM_3");
		
		var begin = new Array() ;
		var end = new Array() ;
		
		for(var i=0; i<4; i++)
		{
		    if(dynm_arr[i].memSeg.name != "MEM_NULL")
		    {
		    	begin[i] = "_";
		    	begin[i] += dynm_arr[i].memSeg.name;
		    	begin[i] += "_BEG";
		    	end[i] = "_";
		    	end[i] += dynm_arr[i].memSeg.name;
		    	end[i] += "_END";	
		    }
		    else
		    {
		    	begin[i] = 0;
		    	end[i] = 0;
		    }
		}
	%%}


	% for( var i=0; i<4; i++) { 
		%if( dynm_arr[i].memSeg.name != "MEM_NULL") {
	    		.ref `begin[i]`, `end[i]`	    
		%} 
	%}


	%for (var i = 0 ; i < dynmobjs.length ; i++) {
	%    var a = dynmobjs[i];
	
	    .sect ".dspbridge_mem"            ;
	    `word` `a.id`                ; segment id;
	%if( begin[i] != 0) {
	    `word` `begin[i]`              ;
	    `word` `end[i]` - `begin[i]` +1  ;
	%} else {
	    `word` `begin[i]`              ;
	    `word` `end[i]`               ;
	%}
	    `word` `a.memType`           ;
	%}
%} 
