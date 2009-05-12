// fixme: comments/autodoc

/*
 *  ======== Doff.xdc ========
 */

/*!
 *  ======== Doff ========
 *  A Bridge utility capsule for generating DOFF files, both static and dynamic.
 *
 *  This capsule provides utility services for building DOFF files suitable
 *  for the dynamic loader.  There is support for static, dynamic, split-phase,
 *  and dependant libraries.
 */
metaonly module Doff {

    // configuration params passed to the addConfiguration() function
    struct ConfigurationParams {
        string                 chipType;
        string                 nodeName;
        xdc.bld.ITarget.Module target;
        string                 tcfName;
        string                 tcfImportPath[];
        string                 chartName;
    };

    // configuration object returned by addConfiguration() function
    // details are exposed here in case the user needs the info
    struct Configuration {
        string nodeName;
        xdc.bld.ITarget.Module target;
        string                 generatedFiles[];
    }

    // params to addDll() function
    struct DllParams {
        string                   dllName;
        string                   dllType; // "node", "phase_{create,execute,delete}", "dependent"
        Configuration            configuration;
        bool                     ignoreDefaultLinkCmdFile;  // false by default
        string                   additionalLinkCmdFiles[];  // null by default
        xdc.bld.Executable.Attrs buildParams;               // {} by default
        bool                     skipXdcConfigStep;         // false by defailt
    };

    // fixme -- not necessary: Doff can internally keep a list of used targets and add new ones
    /*!
     *  ======== addTarget ========
     *  Used to add a new target type to the list of "known" DOFF targets.
     *
     *  @param(target)          The new target to recognize.
     */
    void addTarget(xdc.bld.ITarget.Module target);


    /*!
     *  ======== complete ========
     *  Must be called when BOM configuration of DOFF files is complete.
     */
    void complete();


    /*!
     *  ======== addExecutable ========
     *  Add a DOFF executable.
     *
     *  This utility fuction should be used instead of Pkg.addExecutable to
     *  add a DOFF executable to the BOM.
     *
     *  @param(exeName)         The Executable name.
     *
     *  @param(target)          The Target.
     *
     *  @param(platform)        The Platform.
     *
     *  @param(buildParams)     An array of build parameters.
     */
    xdc.bld.Executable.Instance addExecutable(
        string                   programName,
        xdc.bld.ITarget.Module   target,
        string                   platform,
        xdc.bld.Executable.Attrs buildParams
        );


    any addConfiguration( ConfigurationParams configParams );

    /*!
     *  ======== addNodeDll ========
     *  Add a dynamically loadable Node dll to the BOM.  fixme -- autodoc
     *
     *  @param(target)          The Target.
     *
     *  @param(dllName)         The Library base name.
     *
     *  @param(chartFileName)   The Chart file name.
     *
     *  @param(buildParams)     An array of build parameters.
     *
     *  @param(cfgScript)       The configuration script.
     */
    xdc.bld.Executable.Instance addDll( DllParams dllParams );

}


/* issues:
*/
