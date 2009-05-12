xdc.loadCapsule('xdc/om2.xs');

var $om = xdc.om;


/* ======== IMPORTS ======== */

    xdc.loadPackage('xdc');
    xdc.loadPackage('xdc.bld');

/* ======== OBJECTS ======== */

// package ti.dspbridge.dsp.doffbuild
    var po = $om.$$bind('ti.dspbridge.dsp.doffbuild.Package', $$PObj());
    $om.$$bind('ti.dspbridge.dsp.doffbuild', $$VObj('ti.dspbridge.dsp.doffbuild', po));
// module Doff
    var po = $om.$$bind('ti.dspbridge.dsp.doffbuild.Doff.Module', $$PObj());
    $om.$$bind('ti.dspbridge.dsp.doffbuild.Doff', $$VObj('ti.dspbridge.dsp.doffbuild.Doff', po));
    $om.$$bind('ti.dspbridge.dsp.doffbuild.Doff$$ConfigurationParams', $$PObj());
    $om.$$bind('ti.dspbridge.dsp.doffbuild.Doff.ConfigurationParams', $$PStr($om['ti.dspbridge.dsp.doffbuild.Doff$$ConfigurationParams'], true));
    $om.$$bind('ti.dspbridge.dsp.doffbuild.Doff$$Configuration', $$PObj());
    $om.$$bind('ti.dspbridge.dsp.doffbuild.Doff.Configuration', $$PStr($om['ti.dspbridge.dsp.doffbuild.Doff$$Configuration'], true));
    $om.$$bind('ti.dspbridge.dsp.doffbuild.Doff$$DllParams', $$PObj());
    $om.$$bind('ti.dspbridge.dsp.doffbuild.Doff.DllParams', $$PStr($om['ti.dspbridge.dsp.doffbuild.Doff$$DllParams'], true));

/* ======== CONSTS ======== */

// module Doff

/* ======== CREATES ======== */

// module Doff

/* ======== FUNCTIONS ======== */

// fxn Doff.addTarget
    var fxn = $om.$$bind('ti.dspbridge.dsp.doffbuild.Doff$$addTarget', $$PFxn($om['ti.dspbridge.dsp.doffbuild.Doff.Module'], null, 1, 1, false));
        fxn.$$arg(0, 'target', $om['xdc.bld.ITarget.Module'], undefined);
// fxn Doff.complete
    var fxn = $om.$$bind('ti.dspbridge.dsp.doffbuild.Doff$$complete', $$PFxn($om['ti.dspbridge.dsp.doffbuild.Doff.Module'], null, 0, 0, false));
// fxn Doff.addExecutable
    var fxn = $om.$$bind('ti.dspbridge.dsp.doffbuild.Doff$$addExecutable', $$PFxn($om['ti.dspbridge.dsp.doffbuild.Doff.Module'], $om['xdc.bld.Executable.Instance'], 4, 4, false));
        fxn.$$arg(0, 'programName', $$T_Str, undefined);
        fxn.$$arg(1, 'target', $om['xdc.bld.ITarget.Module'], undefined);
        fxn.$$arg(2, 'platform', $$T_Str, undefined);
        fxn.$$arg(3, 'buildParams', $om['xdc.bld.Executable.Attrs'], $$DEFAULT);
// fxn Doff.addConfiguration
    var fxn = $om.$$bind('ti.dspbridge.dsp.doffbuild.Doff$$addConfiguration', $$PFxn($om['ti.dspbridge.dsp.doffbuild.Doff.Module'], $$T_Obj, 1, 1, false));
        fxn.$$arg(0, 'configParams', $om['ti.dspbridge.dsp.doffbuild.Doff.ConfigurationParams'], $$DEFAULT);
// fxn Doff.addDll
    var fxn = $om.$$bind('ti.dspbridge.dsp.doffbuild.Doff$$addDll', $$PFxn($om['ti.dspbridge.dsp.doffbuild.Doff.Module'], $om['xdc.bld.Executable.Instance'], 1, 1, false));
        fxn.$$arg(0, 'dllParams', $om['ti.dspbridge.dsp.doffbuild.Doff.DllParams'], $$DEFAULT);

/* ======== SIZES ======== */

// struct Doff.ConfigurationParams
    var _Doff$$ConfigurationParams_sz = -1, _Doff$$ConfigurationParams_al = -1;
    function _Doff$$ConfigurationParams_initSize() {
        if ($om.$name != 'cfg' || !Program.build) return;
        var t, a, a0 = 0, n = 1, sz = 0;
        t = Program.build.target.stdTypes.t_Ptr;
        a = t.align - 1; sz = (((sz + a) & ~a) + t.size) * n; n = 1;
        a0 = a;
        t = Program.build.target.stdTypes.t_Ptr;
        a = t.align - 1; sz = (((sz + a) & ~a) + t.size) * n; n = 1;
        t = Program.build.target.stdTypes.t_Ptr;
        a = t.align - 1; sz = (((sz + a) & ~a) + t.size) * n; n = 1;
        t = Program.build.target.stdTypes.t_Ptr;
        a = t.align - 1; sz = (((sz + a) & ~a) + t.size) * n; n = 1;
        t = Program.build.target.stdTypes.t_Ptr;
        a = t.align - 1; sz = (((sz + a) & ~a) + t.size) * n; n = 1;
        t = Program.build.target.stdTypes.t_Ptr;
        a = t.align - 1; sz = (((sz + a) & ~a) + t.size) * n; n = 1;
        _Doff$$ConfigurationParams_sz = ((sz + a0) & ~a0);
        _Doff$$ConfigurationParams_al = a0 + 1;
    }
    $om['ti.dspbridge.dsp.doffbuild.Doff.ConfigurationParams'].$$bind('$sizeof', function() { _Doff$$ConfigurationParams_initSize(); return _Doff$$ConfigurationParams_sz; });
    $om['ti.dspbridge.dsp.doffbuild.Doff.ConfigurationParams'].$$bind('$alignof', function() { _Doff$$ConfigurationParams_initSize(); return _Doff$$ConfigurationParams_al; });
// struct Doff.Configuration
    var _Doff$$Configuration_sz = -1, _Doff$$Configuration_al = -1;
    function _Doff$$Configuration_initSize() {
        if ($om.$name != 'cfg' || !Program.build) return;
        var t, a, a0 = 0, n = 1, sz = 0;
        t = Program.build.target.stdTypes.t_Ptr;
        a = t.align - 1; sz = (((sz + a) & ~a) + t.size) * n; n = 1;
        a0 = a;
        t = Program.build.target.stdTypes.t_Ptr;
        a = t.align - 1; sz = (((sz + a) & ~a) + t.size) * n; n = 1;
        t = Program.build.target.stdTypes.t_Ptr;
        a = t.align - 1; sz = (((sz + a) & ~a) + t.size) * n; n = 1;
        _Doff$$Configuration_sz = ((sz + a0) & ~a0);
        _Doff$$Configuration_al = a0 + 1;
    }
    $om['ti.dspbridge.dsp.doffbuild.Doff.Configuration'].$$bind('$sizeof', function() { _Doff$$Configuration_initSize(); return _Doff$$Configuration_sz; });
    $om['ti.dspbridge.dsp.doffbuild.Doff.Configuration'].$$bind('$alignof', function() { _Doff$$Configuration_initSize(); return _Doff$$Configuration_al; });
// struct Doff.DllParams
    var _Doff$$DllParams_sz = -1, _Doff$$DllParams_al = -1;
    function _Doff$$DllParams_initSize() {
        if ($om.$name != 'cfg' || !Program.build) return;
        var t, a, a0 = 0, n = 1, sz = 0;
        t = Program.build.target.stdTypes.t_Ptr;
        a = t.align - 1; sz = (((sz + a) & ~a) + t.size) * n; n = 1;
        a0 = a;
        t = Program.build.target.stdTypes.t_Ptr;
        a = t.align - 1; sz = (((sz + a) & ~a) + t.size) * n; n = 1;
        sz += $om['ti.dspbridge.dsp.doffbuild.Doff']['Configuration'].$sizeof() * n; n = 1;
        t = Program.build.target.stdTypes.t_Short;
        a = t.align - 1; sz = (((sz + a) & ~a) + t.size) * n; n = 1;
        t = Program.build.target.stdTypes.t_Ptr;
        a = t.align - 1; sz = (((sz + a) & ~a) + t.size) * n; n = 1;
        sz += $om['xdc.bld.Executable']['Attrs'].$sizeof() * n; n = 1;
        t = Program.build.target.stdTypes.t_Short;
        a = t.align - 1; sz = (((sz + a) & ~a) + t.size) * n; n = 1;
        _Doff$$DllParams_sz = ((sz + a0) & ~a0);
        _Doff$$DllParams_al = a0 + 1;
    }
    $om['ti.dspbridge.dsp.doffbuild.Doff.DllParams'].$$bind('$sizeof', function() { _Doff$$DllParams_initSize(); return _Doff$$DllParams_sz; });
    $om['ti.dspbridge.dsp.doffbuild.Doff.DllParams'].$$bind('$alignof', function() { _Doff$$DllParams_initSize(); return _Doff$$DllParams_al; });

/* ======== TYPES ======== */

// module Doff
    var cap = $om.$$bind('ti.dspbridge.dsp.doffbuild.Doff$$capsule', xdc.loadCapsule('../Doff.xs'));
    var po = $om['ti.dspbridge.dsp.doffbuild.Doff.Module'].$$init('ti.dspbridge.dsp.doffbuild.Doff.Module', $$Module);
        po.$$fld('$hostonly', $$T_Num, 1, 'r');
        var fxn = cap['module$meta$init'];
        if (fxn) $om.$$bind('ti.dspbridge.dsp.doffbuild.Doff$$module$meta$init', true);
        if (fxn) po.$$fxn('module$meta$init', $$T_Met, fxn);
        po.$$fxn('addTarget', $om['ti.dspbridge.dsp.doffbuild.Doff$$addTarget'], cap['addTarget']);
        po.$$fxn('complete', $om['ti.dspbridge.dsp.doffbuild.Doff$$complete'], cap['complete']);
        po.$$fxn('addExecutable', $om['ti.dspbridge.dsp.doffbuild.Doff$$addExecutable'], cap['addExecutable']);
        po.$$fxn('addConfiguration', $om['ti.dspbridge.dsp.doffbuild.Doff$$addConfiguration'], cap['addConfiguration']);
        po.$$fxn('addDll', $om['ti.dspbridge.dsp.doffbuild.Doff$$addDll'], cap['addDll']);
// struct Doff.ConfigurationParams
    var po = $om['ti.dspbridge.dsp.doffbuild.Doff$$ConfigurationParams'].$$init('ti.dspbridge.dsp.doffbuild.Doff.ConfigurationParams', null);
        po.$$fld('$hostonly', $$T_Num, 1, 'r');
        po.$$fld('chipType', $$T_Str, undefined, 'w');
        po.$$fld('nodeName', $$T_Str, undefined, 'w');
        po.$$fld('target', $om['xdc.bld.ITarget.Module'], undefined, 'w');
        po.$$fld('tcfName', $$T_Str, undefined, 'w');
        po.$$fld('tcfImportPath', $$PArr($$T_Str, false), $$DEFAULT, 'w');
        po.$$fld('chartName', $$T_Str, undefined, 'w');
// struct Doff.Configuration
    var po = $om['ti.dspbridge.dsp.doffbuild.Doff$$Configuration'].$$init('ti.dspbridge.dsp.doffbuild.Doff.Configuration', null);
        po.$$fld('$hostonly', $$T_Num, 1, 'r');
        po.$$fld('nodeName', $$T_Str, undefined, 'w');
        po.$$fld('target', $om['xdc.bld.ITarget.Module'], undefined, 'w');
        po.$$fld('generatedFiles', $$PArr($$T_Str, false), $$DEFAULT, 'w');
// struct Doff.DllParams
    var po = $om['ti.dspbridge.dsp.doffbuild.Doff$$DllParams'].$$init('ti.dspbridge.dsp.doffbuild.Doff.DllParams', null);
        po.$$fld('$hostonly', $$T_Num, 1, 'r');
        po.$$fld('dllName', $$T_Str, undefined, 'w');
        po.$$fld('dllType', $$T_Str, undefined, 'w');
        po.$$fld('configuration', $om['ti.dspbridge.dsp.doffbuild.Doff.Configuration'], $$DEFAULT, 'w');
        po.$$fld('ignoreDefaultLinkCmdFile', $$T_Bool, undefined, 'w');
        po.$$fld('additionalLinkCmdFiles', $$PArr($$T_Str, false), $$DEFAULT, 'w');
        po.$$fld('buildParams', $om['xdc.bld.Executable.Attrs'], $$DEFAULT, 'w');
        po.$$fld('skipXdcConfigStep', $$T_Bool, undefined, 'w');

/* ======== SINGLETONS ======== */

// package ti.dspbridge.dsp.doffbuild
    var po = $om['ti.dspbridge.dsp.doffbuild.Package'].$$init('ti.dspbridge.dsp.doffbuild.Package', $om['xdc.IPackage.Module']);
    var pkg = $om['ti.dspbridge.dsp.doffbuild'].$$init(po, 'ti.dspbridge.dsp.doffbuild', $$DEFAULT, false);
    $om.$packages.$add(pkg);
    pkg.$$bind('$name', 'ti.dspbridge.dsp.doffbuild');
    pkg.$$bind('$category', 'Package');
    pkg.$$bind('$$qn', 'ti.dspbridge.dsp.doffbuild.');
    pkg.$$bind('$vers', []);
    pkg.$attr.$seal('length');
    pkg.$$bind('$imports', [
    ]);
    var $PKG = pkg;
    pkg.build.libraries = [
    ];
// module Doff
    var vo = $om['ti.dspbridge.dsp.doffbuild.Doff'];
    var po = $om['ti.dspbridge.dsp.doffbuild.Doff.Module'];
    vo.$$init(po, 'ti.dspbridge.dsp.doffbuild.Doff', $$DEFAULT, false);
    vo.$$bind('Module', po);
    vo.$$bind('$category', 'Module');
    vo.$$bind('$capsule', $om['ti.dspbridge.dsp.doffbuild.Doff$$capsule']);
    vo.$$bind('$package', $om['ti.dspbridge.dsp.doffbuild']);
    vo.$$bind('$$tdefs', []);
    vo.$$bind('$$proxies', []);
    vo.$$bind('ConfigurationParams', $om['ti.dspbridge.dsp.doffbuild.Doff.ConfigurationParams']);
    vo.$$tdefs.push($om['ti.dspbridge.dsp.doffbuild.Doff.ConfigurationParams']);
    vo.$$bind('Configuration', $om['ti.dspbridge.dsp.doffbuild.Doff.Configuration']);
    vo.$$tdefs.push($om['ti.dspbridge.dsp.doffbuild.Doff.Configuration']);
    vo.$$bind('DllParams', $om['ti.dspbridge.dsp.doffbuild.Doff.DllParams']);
    vo.$$tdefs.push($om['ti.dspbridge.dsp.doffbuild.Doff.DllParams']);
    vo.$$bind('$private', {});
    pkg.$modules.$add(vo);
    $om.$modules.$add(vo);
    vo.$$bind('$$instflag', 0);
    vo.$$bind('$$iobjflag', 1);
    vo.$$bind('$$sizeflag', 1);
    vo.$$bind('$$dlgflag', 0);
    vo.$$bind('$$iflag', 0);
    if ('Module__Object' in vo) vo.$$bind('$object', new vo.Module__Object);
    vo.$$bind('$$meta_iobj', 0 + ('ti.dspbridge.dsp.doffbuild.Doff$$instance$static$init' in $om));
    vo.$$bind('$$fxntab', []);
    vo.$attr.$seal('length');
    pkg.$$bind('Doff', vo);
    pkg.$unitNames.$add('Doff');

/* ======== INITIALIZATION ======== */

    $om['ti.dspbridge.dsp.doffbuild.Doff'].module$meta$init();
    pkg.init();
    $om['ti.dspbridge.dsp.doffbuild.Doff'].$$bless();
