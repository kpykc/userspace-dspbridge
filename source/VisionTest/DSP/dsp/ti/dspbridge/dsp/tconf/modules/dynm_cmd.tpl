
SECTIONS {
    .dspbridge_mem (COPY): {} /* Dynamic load mem */
    .dllview: {} > `this.DLLVIEWSEG.name`
}

% var cpu = prog.cpu.attrs.cpuCore;
% var is6x = (cpu[0] == '6') ? true : false;

%if(!is6x) {

%%{
	var dynm = dspbridge.module("DYNM");

	var dynm_arr = new Array();	
	dynm_arr[0] = dynm.instance("DYNM_0");
	dynm_arr[1] = dynm.instance("DYNM_1");
	dynm_arr[2] = dynm.instance("DYNM_2");
	dynm_arr[3] = dynm.instance("DYNM_3");
	
	var dynm_sec = new Array();
	var begin = new Array() ;
	var end = new Array() ;
	
	/* set up the new section names = period + lowercase version of dynm instance */
	for(var i=0; i<4; i++)
	{
	    dynm_sec[i] = ".";
	    dynm_sec[i] += dynm_arr[i].memSeg.name.toLowerCase() ;
	    begin[i] = "_";
	    begin[i] += dynm_arr[i].memSeg.name;
	    begin[i] += "_BEG";
	    end[i] = "_";
	    end[i] += dynm_arr[i].memSeg.name;
	    end[i] += "_END";	
	}

	var cpu = prog.cpu.attrs.cpuCore;
	var is6x = (cpu[0] == '6') ? true : false;
	// Linker command file output for 55 needs to be in bytes, not MAUs.
	var bytesPerMau = (is6x) ? 1 : 2;

%%}



% for( var i=0; i<4; i++) { /* set up new sections */
%if( dynm_arr[i].memSeg.name != "MEM_NULL") {
	
SECTIONS {
    `dynm_sec[i]` : { 
    `begin[i]` = .;
     . += `utils.toHex((dynm_arr[i].len)*bytesPerMau)`;
    `end[i]` = . - 1;
    } > `dynm_arr[i].memSeg.name`
}
	    
%} 

%}


%}