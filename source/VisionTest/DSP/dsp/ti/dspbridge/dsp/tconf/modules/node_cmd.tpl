
/* NODE linker output */

%{
%var nodes = this.instances();
%for (var i = 0 ; i < nodes.length ; i++) {

SECTIONS {
    .`nodes[i].uuid` (COPY): {} /* node information */
}

%}
%}
