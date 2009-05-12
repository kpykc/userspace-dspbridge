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
    if ((Program.build.target.suffix != "64P") &&
            (Program.build.target.suffix != "55L")) {
        return (null);
    }

    var libs = "dskt2.a" + Program.build.target.suffix;

    if (this.DSKT2.debug) {
        libs = "dskt2d.a" +  Program.build.target.suffix;
    }

    print("    will link with " + this.$name + ":" + libs);
    return (libs);
}

/*
 *  ======== getSects ========
 */
function getSects() {
   if (this.DSKT2.$used) {
       return (this.$name.replace( /\./g, "/" ) + "/DSKT2.link.xdt");
   }

   return (null);
}

/*
 *  @(#) ti.sdo.fc.dskt2; 1, 0, 1,66; 5-10-2007 10:50:09; /db/atree/library/trees/fc-b12x/src/
 */

