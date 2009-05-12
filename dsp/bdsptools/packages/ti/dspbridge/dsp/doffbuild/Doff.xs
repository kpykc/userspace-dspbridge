/*
 *  ======== Doff.xs ========
 *    Shared XDC build rules for bridge packages.
 *
 */

var dprint = 0 ? print : function() {print("");};  // set to 1 to turn on debugging

var rules = ""; // rules emitted in the makefile
var epilogueRules = "";
var thisDir;    // directory of the doffbuild package

function addTarget( target ) {
    thisDir = this.$package.packageBase;
    addCOF2DOFRule ( target );
    addCOFCompRules( target );
}

function complete() {
    Pkg.makePrologue += rules;
    Pkg.makeEpilogue += epilogueRules;
}

function addExecutable( exeName, target, platform, buildParams )
{
    // fixme: uses xdc built-in tconf understanding -- we need to avoid that
    // and do same thing here as for ti.bios middleware;
    // otherwise ugly Pkg.uses is needed by the caller
    var exe = Pkg.addExecutable( exeName, target, platform, buildParams );
    addDOFDependencies( target, exeName );
    return exe;
}

/* local */
function addCOF2DOFRule(targ)
{
    var coff2doffRule;

    if (environment[ "xdc.hostOS" ] == "Linux") {
        var ext = "";
    } else {
        var ext = ".exe";
    }

    coff2doffRule = "\n# COFF -> DOFF Build rules for " + targ.name + "\n" +
        "%.dof" + targ.suffix + " : %.x" + targ.suffix + " %.x" +
        targ.suffix + ".ent\n" +
        "\t$(ECHO) \"running DLLcreate $< $@...\"\n" +
        "\t" + thisDir + "bin/DLLcreate" + ext +
        " -q -e=$<.ent " +
        "-e=" + thisDir + "entries/bios_entries.ent $< -o=$@\n\n";

    coff2doffRule += "%.dll" + targ.suffix + " : %.o" + targ.suffix + "\n" +
        "\t$(ECHO) \"running DLLcreate $< $@...\"\n" +
        "\t" + thisDir + "bin/DLLcreate" + ext +
        " $< -o=$@\n\n";

    coff2doffRule += "# COFF entries file generation\n" +
        "%.x" + targ.suffix + ".ent : %.x" + targ.suffix + "\n" +
        "\t$(ECHO) \"running dbsymgen $< $@...\"\n" +
        "\t" + thisDir + "bin/dbsymgen $< > $@\n\n";

    coff2doffRule += "# COFF entries file cleanup\n" +
        "clean," + targ.suffix + " clean :: \n" +
        "\t-$(RM) *.ent\n\n";

    rules += coff2doffRule;
}

/* local */
function addCOFCompRules(targ)
{
    if (environment[ "xdc.hostOS" ] == "Linux") {
	    var ext = "";
    } else {
	    var ext = ".dll";
    }

    var strRule = "\n# DOF dll copy/cleanup rules\n";

    strRule += "clean," + targ.suffix + " clean :: \n" +
        "\t-$(RM) libcdb.so\n";

    rules += strRule;
}

/* local */
function addDOFDependencies( targ, programBase )
{
    var strDep;

    strDep = "\n# DOF Dependencies for " + programBase + "\n";

    // Note that this uses internal knowlege that .programs and
    // .programs,<targ.suffix> are goals for building programs.
    // If RTSC adds new goals, or changes the names of the ones we're using
    // this dependency lines should be modified to reflect it.
    strDep += programBase + ".dof" + targ.suffix + " : " +
        programBase + ".x" + targ.suffix + "\n" +
        ".executables," + targ.suffix + "  .executables : " +
        programBase + ".dof" + targ.suffix + "\n";

    strDep += "clean," + targ.suffix + " clean :: \n" +
        "\t-$(RM) " + programBase + ".dof" + targ.suffix + "\n\n";

    rules += strDep;
}



var configurations = [];

function addConfiguration( configurationParams )
{
    var nodeName      = configurationParams.nodeName;
    var tcfName       = configurationParams.tcfName;
    var tcfImportPath = configurationParams.tcfImportPath != undefined ?
                        configurationParams.tcfImportPath :  "";
    var target        = configurationParams.target;
    var chartName     = configurationParams.chartName     != undefined ?
                        configurationParams.chartName     :  null;
    var chipType      = configurationParams.chipType;

    if (nodeName == undefined) {
        throw "\nError: Doff.addConfiguration() params: nodeName field " +
              "must be defined\n";
    }
    if (tcfName == undefined) {
        throw "\nError: Doff.addConfiguration() params: nodeName field " +
              "must be defined\n";
    }
    if (target == undefined) {
        throw "\nError: Doff.addConfiguration() params: target field " +
              "must be defined";
    }

    var config = {
        nodeName      : nodeName,
        target        : target,
        generatedFiles: [],
    };

    var importPath = ".;" + environment[ "xdc.path" ];

    if (tcfImportPath != null) {
        for (var i = 0; i < tcfImportPath.length; i++) {
            // fixme! constructs the import path based on the xdc path
            // which is b.a.d.
            if (tcfImportPath[i].charAt( 0 ) == "/" ||
                tcfImportPath[i].charAt( 1 ) == ":" ||
                tcfImportPath[i].charAt( 0 ) == "." ) {
                importPath += tcfImportPath[i] + ";";
            } else {
                var xdcPaths = ("." + ";" + environment[ "xdc.path" ]).
                                split( ";" );
                for (var j = 0; j < xdcPaths.length; j++) {
                    importPath += xdcPaths[j] + "/" + tcfImportPath[i] + ";"
                }
            }
        }
    }

    var outDir = "./package/cfg/tconfGeneratedFiles/" + nodeName +
                 "_" + target.suffix;
    var genFileList = "genFileList.txt";
    var depFileList = "depFileList.txt";

    java.io.File( outDir ).mkdirs();

    var tconfCmd = environment[ "xdc.root" ] + "/tconf " +
        "-Dconfig.chipType=" + chipType + " " +
        "-Dconfig.importPath=" + importPath + " " +
        "-Dconfig.userTcf=" + tcfName + " " +
        (chartName == null ? "" : "-Dconfig.userChart=" + chartName ) + " " +
        "-Dconfig.outDir=" + outDir + " " +
        "-Dconfig.genFileList=" + genFileList + " " +
        "-Dconfig.depFileList=" + depFileList + " " +
        thisDir + "tconf/Doff.tcf" +
        "\n";

    print( "Doff.xs: running tconf over " + tcfName +
           " to generate CDB/asm..." );
    dprint( tconfCmd );

    // copy the .dll file to the package dir
    var dbdllName = "dbdllcfg" +
                  (environment["xdc.hostOS"] == "Windows" ? ".dll" : "");
    copyFile( thisDir + "bin/" + dbdllName , "./" + dbdllName );

	var status  = { output: "", exitStatus: "" };

	utils.exec( tconfCmd, {}, status );

    dprint( "--> Build " +
          (status.exitStatus == 0 ? "successful" : "failed") + ".\n" );

    if (status.exitStatus != 0) {
        throw "\n\nDoff: Error -- tconf step failed. " +
              "Below is the command line and the output:\n\n" +
              "--- cmd line: ---\n" + tconfCmd + "\n\n" +
              "--- output: ---\n" + status.output;
    }

    var generatedFiles  = readFile( outDir + "/" + genFileList ).split( "," );
    var tcfDependencies = readFile( outDir + "/" + depFileList ).
                          replace( /,/g, " " ) + " " +
                          (chartName == null ? "" : chartName );
                          // fixme: check if chart gets here twice

    // find the first .c/asm file and make it the main "hooking" point
    // other will depend on it, and the executable will be rebuilt when
    // the tcf is touched
    for (var i = 0; i < generatedFiles.length; i++) {
        var file = generatedFiles[i];
        if (file.match( /\.c$/   )    != null ||
            file.match( /\.asm$/ )    != null ||
            file.match( /\.s.{1,3}/ ) != null ) {
            if (i > 0) {
                var x = generatedFiles[0];
                generatedFiles[0] = generatedFiles[i];
                generatedFiles[i] = x;
            }
            break;
        }
    }


    // modify the tconfCmd for MAKE by adding quotes around the importPath.
    // but we shouldn't be dealing with MAKE...
    tconfCmd = tconfCmd.replace( /-Dconfig.importPath=(\S*)\s/,
                                 "-Dconfig.importPath=\"$1\" " );

    var tcfRules =
        "# rules for generating cdb/cmd/asm files from " + tcfName +
           " for DLL " + nodeName + "\n" +
        generatedFiles[0] + ": " + tcfDependencies + "\n" +
        "\t -$(RMDIR)    " + outDir + "\n" +
        "\t -$(MKDIR) -p " + outDir + "\n" +
        "\t$(ECHO) \"running tconf " + tcfName + " to gen cdb/asm...\"\n" +
        "\t " + tconfCmd + "\n";

    for (var i = 1; i < generatedFiles.length; i++) {
        if (generatedFiles[i].length == 0) {
            continue;
        }
        tcfRules += generatedFiles[i] + ": " + generatedFiles[0] + "\n";
    }

    tcfRules += "clean::\n" +
        "\t -$(RM) " + dbdllName + "\n" +
        "\t -$(RMDIR) " + outDir + "\n";

    dprint( "\n\n\ntcf make rule: \n" + tcfRules );

    config.generatedFiles = generatedFiles;

    rules += tcfRules;

    return config;
}

/* local */
function findGeneratedFiles( config, pattern ) {
    var files = [];
    dprint( "Doff: called to search '" + config.nodeName + "' config for '" +
            pattern + "':" );
    for (var i = 0; i < config.generatedFiles.length; i++) {
        var file = config.generatedFiles[i];
        // dprint( "    checking name " + file );
        if (file.match( pattern ) != null) {
            files[ files.length ] = file;
            dprint( "    -> found generated file " + file );
        }
    }
    return files;
}

function getDllsGeneratedLinkCmdName( dllName, targetName, quiet )
{
    var cmdName = dllCmdFiles[ dllName + targetName ];
    if (cmdName == undefined) {
        cmdName = null;
        if (quiet != true) {
            print( "Doff: warning -- asked for a linkCmd file for DLL " +
                   dllName + " (" + targetName + ") that has not been " +
                   "generated (yet)." );
        }
    }
    return cmdName;
}


// function addDll( dllName, target, dllParams )
function addDll( dllParams )
{
    var dllName                  = dllParams.dllName;
    var dllType                  = dllParams.dllType;
    var config                   = dllParams.configuration;
    var ignoreDefaultLinkCmdFile = dllParams.ignoreDefaultLinkCmdFile;
    var additionalLinkCmdFiles   = dllParams.additionalLinkCmdFiles;
    var buildParams              = dllParams.buildParams;
    var skipXdcConfigStep        = dllParams.skipXdcConfigStep;

    // some error checking here...
    if (dllName == undefined) {
        throw "\nError: Doff.addDll() params: dllName field " +
              "must be defined\n";
    }
    switch (dllType) {
        case "node"         :
        case "phase_create" :
        case "phase_execute":
        case "phase_delete" :
        case "dependent"    :
            break;
        default:
            throw "\nError: Doff.addDll() params: dllType field " +
              "must be 'node|phase_{create|execute|delete}|dependent'\n";
    }
    if (config == undefined) {
        throw "\nError: Doff.addDll() params: configuration field " +
              "must be an object returned by addConfiguration()\n";
    }

    // and default settings if otherwise missing
    if (ignoreDefaultLinkCmdFile == undefined) {
        ignoreDefaultLinkCmdFile = false;
    }
    if (additionalLinkCmdFiles == undefined) {
        additionalLinkCmdFiles = [];
    }
    if (buildParams == undefined) {
        buildParams = {};
    }
    if (skipXdcConfigStep == undefined) {
        skipXdcConfigStep = false;
    }

    var target = config.target;

    // we really begin here...

    // create dll output dir, if not current dir
    var programDir = ".", p;
    if ((p = dllName.lastIndexOf( "/" )) > -1) {
        programDir = dllName.substring( 0, p ); // or .replace( /\/[^\/]*$/g, "" )
        java.io.File( programDir ).mkdirs();
    }

    if (skipXdcConfigStep == false) {
        // fixme: a workaround for an XDC bug
        // Remove clashing symbols by adding -D switches
        var name = dllName.replace(/\//, "_");
        buildParams.defs =
            (buildParams.defs == undefined ? "" : buildParams.defs );
        buildParams.defs += " -D__PLAT__=__PLAT__" + name +
                          " -D__ISA__=__ISA__"   + name;
    }

    // and linker switches for partial linking
    buildParams.lopts = (buildParams.lopts == undefined ? "" :
                         buildParams.lopts );
    buildParams.lopts += " -r -cr ";

    // add linker command files, generated and supplied, to linker options
    var cmdFilesList = "";

    // first add generated one by default, depending on the type, if so desired
    if (ignoreDefaultLinkCmdFile == true) {
    } else {
        var genCmdFiles = findGeneratedFiles( config, /\.cmd$/ );
        if (genCmdFiles.length > 0) {
            cmdFilesList += genCmdFiles[0] + " ";
        }
    }

    // then add additional ones, if any
    for (var i = 0; i < additionalLinkCmdFiles.length; i++) {
        var cmdFile = additionalLinkCmdFiles[i];
        if (cmdFile != null && cmdFile.length > 0) {
            cmdFilesList += cmdFile + " ";
        }
    }
    buildParams.lopts += " " + cmdFilesList + " ";

    if (skipXdcConfigStep == false) {
        if (buildParams.cfgScript == undefined ||
            buildParams.cfgScript === null ||
            buildParams.cfgScript == "") {
            // fixme! xdc-i11 has a bug with dll.cfg being elsewhere!
            // must not use "requires" in package.xdc
            buildParams.cfgScript = this.$package.packageBase + "dll.cfg";
        }
    }

    var lib = Pkg.addExecutable( "package/nodelib/" + dllName, target,
                                 target.platform, buildParams );

    // now the tricky part: add generated source files depending on the dll type
    var dllEndName = dllName.replace( /.*\//g, "" ); // dll name without the dir
    switch (dllType) {
        case "node":
            var cFiles = findGeneratedFiles( config, /\.c$/ );
            var sFiles = findGeneratedFiles( config, /cfg\.s.{1,3}$/ );
            var nodeDeplibsFile = findGeneratedFiles( config,
                              RegExp( dllEndName + "_deplibs.s.{1,3}$" ) );
            lib.addObjects( cFiles );
            lib.addObjects( sFiles );
            lib.addObjects( nodeDeplibsFile );
            break;
        case "phase_create" :
        case "phase_execute":
        case "phase_delete" :
            var infix     = (dllType == "phase_create" ) ? "Create"  :
                            (dllType == "phase_execute") ? "Execute" :
                            (dllType == "phase_delete" ) ? "Delete"  : "";
            var psectFile   = findGeneratedFiles( config,
                              RegExp( infix + "_psect.s.{1,3}$" ) );
            var deplibsFile = findGeneratedFiles( config,
                              RegExp( dllEndName + "_deplibs.s.{1,3}$" ) );
            lib.addObjects( psectFile );
            lib.addObjects( deplibsFile );
            break;
        case "dependent" :
            var uuidFile = findGeneratedFiles( config,
                           RegExp( dllEndName + "_uuid.s.{1,3}$" ) );
            var depDeplibsFile = findGeneratedFiles( config,
                              RegExp( dllEndName + "_deplibs.s.{1,3}$" ) );
            lib.addObjects( uuidFile );
            lib.addObjects( depDeplibsFile );
            break;
    }

    var progExe = "package/nodelib/" + dllName + ".x"   + target.suffix;
    var progObj = dllName + ".o"   + target.suffix;
    var progDll = dllName + ".dll" + target.suffix;

    var dllRules =
        "\n# rules for generating " + dllName + " DLL:\n" +
        "all: " + progDll + "\n" +
        progDll + " : " + progObj + "\n"                         +
        //fixme".dlls," + target.suffix + " .dlls : " + progDll + "\n" +
        progObj + " : " + progExe + "\n" +
            "\t$(ECHO) \"copy $< $@...\"\n" +
            "\t$(CP) "  + progExe + " " + progObj + "\n"

    if (cmdFilesList.length > 0) {
        dllRules +=
            "# list of linker-command files on which the exe depends\n" +
            progExe + ": " + cmdFilesList + "\n";
    }
    dllRules += "clean::\n" +
        "\t -$(RMDIR) " + dllName + ".o* " + dllName + ".dll*\n";
    dllRules += "# rules for generating the actual .x exe are further below.\n";

    dprint( "\n\nDLL creation rule: \n" + dllRules );

    rules += dllRules;

    if (skipXdcConfigStep == true) {
        /* nasty little hack: create dummy cfg outfiles so cfg step is skipped */
        var cfgDir  = "package/cfg/package/nodelib/";
        var cfgBase = cfgDir + dllName + "_x" + target.suffix;
        if ((p = dllName.lastIndexOf( "/" )) > -1) {
            cfgDir += dllName.substring( 0, p );
        }
        java.io.File( cfgDir ).mkdirs();
        for (ext in { "cpp":'', "dep":'', "cfg.dot":'', "gel":'', "inc":'',
                      "c"  :'', "pjt":'', "xdl":'', "cfg.xml":'' } ) {
            epilogueRules += cfgBase + "." + ext + " : \n" +
                "\techo " + ( ext == "xdl" ? "'/**/'" : "" ) + " > " +
                cfgBase + "." + ext + " \n";
        }
    }

    return lib;
}


/* local */
function readFile( fileName ) {
    try {
        var file  = new java.io.FileReader( fileName );
        var lfile = new java.io.LineNumberReader( file );
        var line  = lfile.readLine();
        return "" + line;
    } catch (e) {
        throw "Doff internal error: could not read file " + fileName + "\n";
    }
}


/* local */
function copyFile( src, tgt)
{
    var fin = new java.io.File(src);
    var fout = new java.io.File(tgt);
    var status = 0;

    try
    {
        var srcChannel = new java.io.FileInputStream(fin).getChannel();
        var tgtChannel = new java.io.FileOutputStream(fout).getChannel();
        var sz = srcChannel.size();

        tgtChannel.transferFrom(srcChannel, 0, srcChannel.size());

        srcChannel.close();
        tgtChannel.close();

    } // end try

    catch(e)
    {
        print( "Failed to copy " + src + " to " + tgt );
        print( e.message );
        return (2);
    }

    return (status);

} // end copyFile
