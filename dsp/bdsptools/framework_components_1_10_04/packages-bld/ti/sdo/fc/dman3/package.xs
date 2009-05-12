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

    var libs = "dman3Cfg.a" + Program.build.target.suffix;

    if (this.profile.toLowerCase() == "debug" && (this.DMAN3.debug == false)) {
        // TODO: validate this rather than just print a warning.
//        print( "Warning: " + this.$name + " supports debug only if " +
//               "DMAN3.debug = true; you must also have 'bridge' LOG object " +
//               "in your configuration." ); 
    }
    
    if (this.DMAN3.debug) {
        // TODO: validate this rather than just print a warning.
//        print( "Warning: " + this.$name + " library in DEBUG mode requires " +
//               "that a LOG object named 'bridge' exists in BIOS config." );
        libs = "dman3Cfgd.a" +  Program.build.target.suffix;
    }        
            
    print("    will link with " + this.$name + ":" + libs);
    return (libs);
}

/*
 *  ======== getSects ========
 */
function getSects() {
   if (this.DMAN3.$used) {
       return (this.$name.replace( /\./g, "/" ) + "/DMAN3.link.xdt");
   }

   return (null);
}

/*
 *  @(#) ti.sdo.fc.dman3; 1, 0, 1,66; 5-10-2007 10:49:43; /db/atree/library/trees/fc-b12x/src/
 */

