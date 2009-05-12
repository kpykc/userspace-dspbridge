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
 *  ======== validate ========
 */
function validate() 
{
    /* TODO: validate the range of all the config params */
}

/*
 *  ======== getLibs ========
 */
function getLibs() 
{
    if (Program.build.target.suffix != "64P") {
        return (null);
    }

    var libs = "api.a" + Program.build.target.suffix;
            
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
 *  @(#) ti.sdo.fc.utils.api; 1,0,0,36; 5-10-2007 10:50:31; /db/atree/library/trees/fc-b12x/src/
 */

