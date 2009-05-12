xdc.loadCapsule('xdc/om2.xs');

var $om = xdc.om;


/* ======== IMPORTS ======== */

    xdc.loadPackage('xdc');

/* ======== OBJECTS ======== */

// package ti.sdo.fc.dskt2
    var po = $om.$$bind('ti.sdo.fc.dskt2.Package', $$PObj());
    $om.$$bind('ti.sdo.fc.dskt2', $$VObj('ti.sdo.fc.dskt2', po));
// module DSKT2
    var po = $om.$$bind('ti.sdo.fc.dskt2.DSKT2.Module', $$PObj());
    $om.$$bind('ti.sdo.fc.dskt2.DSKT2', $$VObj('ti.sdo.fc.dskt2.DSKT2', po));

/* ======== CONSTS ======== */

// module DSKT2
    $om.$$bind('ti.sdo.fc.dskt2.DSKT2.DEFAULTSEG', "IRAM");

/* ======== CREATES ======== */

// module DSKT2

/* ======== FUNCTIONS ======== */


/* ======== SIZES ======== */


/* ======== TYPES ======== */

// module DSKT2
    var po = $om['ti.sdo.fc.dskt2.DSKT2.Module'].$$init('ti.sdo.fc.dskt2.DSKT2.Module', $$Module);
        po.$$fld('$hostonly', $$T_Num, 1, 'r');
        po.$$fld('DARAM0', $$T_Str, "IRAM", 'wh');
        po.$$fld('DARAM1', $$T_Str, "IRAM", 'wh');
        po.$$fld('DARAM2', $$T_Str, "IRAM", 'wh');
        po.$$fld('SARAM0', $$T_Str, "IRAM", 'wh');
        po.$$fld('SARAM1', $$T_Str, "IRAM", 'wh');
        po.$$fld('SARAM2', $$T_Str, "IRAM", 'wh');
        po.$$fld('ESDATA', $$T_Str, "IRAM", 'wh');
        po.$$fld('IPROG', $$T_Str, "IRAM", 'wh');
        po.$$fld('EPROG', $$T_Str, "IRAM", 'wh');
        po.$$fld('DSKT2_HEAP', $$T_Str, "IRAM", 'wh');
        po.$$fld('ALLOW_EXTERNAL_SCRATCH', $$T_Bool, true, 'wh');
        po.$$fld('DARAM_SCRATCH_SIZES', $$PArr($$T_CNum('(xdc_UInt)'), false), $$SO([0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]), 'wh');
        po.$$fld('SARAM_SCRATCH_SIZES', $$PArr($$T_CNum('(xdc_UInt)'), false), $$SO([0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]), 'wh');
        po.$$fld('debug', $$T_Bool, false, 'wh');
        po.$$fld('cacheWritebackInvalidateFxn', $$T_Str, "BCACHE_wbInv", 'wh');

/* ======== SINGLETONS ======== */

// package ti.sdo.fc.dskt2
    var po = $om['ti.sdo.fc.dskt2.Package'].$$init('ti.sdo.fc.dskt2.Package', $om['xdc.IPackage.Module']);
    var cap = $om.$$bind('xdc.IPackage$$capsule', xdc.loadCapsule('../package.xs'));
        if (cap['init']) po.$$fxn('init', $om['xdc.IPackage$$init'], cap['init']);
        if (cap['close']) po.$$fxn('close', $om['xdc.IPackage$$close'], cap['close']);
        if (cap['validate']) po.$$fxn('validate', $om['xdc.IPackage$$validate'], cap['validate']);
        if (cap['exit']) po.$$fxn('exit', $om['xdc.IPackage$$exit'], cap['exit']);
        if (cap['getLibs']) po.$$fxn('getLibs', $om['xdc.IPackage$$getLibs'], cap['getLibs']);
        if (cap['getSects']) po.$$fxn('getSects', $om['xdc.IPackage$$getSects'], cap['getSects']);
    var pkg = $om['ti.sdo.fc.dskt2'].$$init(po, 'ti.sdo.fc.dskt2', $$DEFAULT, false);
    $om.$packages.$add(pkg);
    pkg.$$bind('$name', 'ti.sdo.fc.dskt2');
    pkg.$$bind('$category', 'Package');
    pkg.$$bind('$$qn', 'ti.sdo.fc.dskt2.');
    pkg.$$bind('$vers', [1, 0, 1]);
    pkg.$attr.$seal('length');
    pkg.$$bind('$imports', [
    ]);
    var $PKG = pkg;
    pkg.build.libraries = [
        'dskt2d.a55L',
        'dskt2.a55L',
        'dskt2d.a64P',
        'dskt2.a64P',
    ];
// module DSKT2
    var vo = $om['ti.sdo.fc.dskt2.DSKT2'];
    var po = $om['ti.sdo.fc.dskt2.DSKT2.Module'];
    vo.$$init(po, 'ti.sdo.fc.dskt2.DSKT2', $$DEFAULT, false);
    vo.$$bind('Module', po);
    vo.$$bind('$category', 'Module');
    vo.$$bind('$package', $om['ti.sdo.fc.dskt2']);
    vo.$$bind('$$tdefs', []);
    vo.$$bind('$$proxies', []);
    vo.$$bind('DEFAULTSEG', $om['ti.sdo.fc.dskt2.DSKT2.DEFAULTSEG']);
    vo.$$bind('$private', {});
    pkg.$modules.$add(vo);
    $om.$modules.$add(vo);
    vo.$$bind('$$instflag', 0);
    vo.$$bind('$$iobjflag', 1);
    vo.$$bind('$$sizeflag', 1);
    vo.$$bind('$$dlgflag', 0);
    vo.$$bind('$$iflag', 0);
    if ('Module__Object' in vo) vo.$$bind('$object', new vo.Module__Object);
    vo.$$bind('$$meta_iobj', 0 + ('ti.sdo.fc.dskt2.DSKT2$$instance$static$init' in $om));
    vo.$$bind('$$fxntab', []);
    vo.$attr['@Template'] = "./DSKT2.xdt";
    vo.$attr.$seal('length');
    vo.$$bind('TEMPLATE$', './DSKT2.xdt');
    pkg.$$bind('DSKT2', vo);
    pkg.$unitNames.$add('DSKT2');

/* ======== INITIALIZATION ======== */

    $om['ti.sdo.fc.dskt2.DSKT2'].module$meta$init();
    pkg.init();
    $om['ti.sdo.fc.dskt2.DSKT2'].$$bless();
