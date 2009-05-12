/* 
 *  Copyright 2007
 *  Texas Instruments Incorporated
 *
 *  All rights reserved.  Property of Texas Instruments Incorporated
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 * 
 */
/*
 *  ======== getLibs ========
 */
function getLibs() 
{
    if (Program.build.target.suffix != "64P") {
        return (null);
    }
    
    var libs   = "acpy3";

    if (this.ACPY3.debug) {
        libs += "d";
    }
    
    libs += ".a" + Program.build.target.suffix;

    print("    will link with " + this.$name + ":" + libs);
    return (libs);
}

/*
 *  ======== getSects ========
 */
function getSects() {

   return (null);
}

/*
 *  @(#) ti.sdo.fc.acpy3; 1, 0, 0,66; 5-10-2007 10:49:19; /db/atree/library/trees/fc-b12x/src/
 */

