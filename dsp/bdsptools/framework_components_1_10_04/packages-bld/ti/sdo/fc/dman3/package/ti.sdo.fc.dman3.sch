xdc.loadCapsule('xdc/om2.xs');

var $om = xdc.om;


/* ======== IMPORTS ======== */

    xdc.loadPackage('xdc');

/* ======== OBJECTS ======== */

// package ti.sdo.fc.dman3
    var po = $om.$$bind('ti.sdo.fc.dman3.Package', $$PObj());
    $om.$$bind('ti.sdo.fc.dman3', $$VObj('ti.sdo.fc.dman3', po));
// module DMAN3
    var po = $om.$$bind('ti.sdo.fc.dman3.DMAN3.Module', $$PObj());
    $om.$$bind('ti.sdo.fc.dman3.DMAN3', $$VObj('ti.sdo.fc.dman3.DMAN3', po));

/* ======== CONSTS ======== */

// module DMAN3
    $om.$$bind('ti.sdo.fc.dman3.DMAN3.MAXDMARECS', 32);
    $om.$$bind('ti.sdo.fc.dman3.DMAN3.MAXGROUPS', 20);

/* ======== CREATES ======== */

// module DMAN3

/* ======== FUNCTIONS ======== */


/* ======== SIZES ======== */


/* ======== TYPES ======== */

// module DMAN3
    var po = $om['ti.sdo.fc.dman3.DMAN3.Module'].$$init('ti.sdo.fc.dman3.DMAN3.Module', $$Module);
        po.$$fld('$hostonly', $$T_Num, 1, 'r');
        po.$$fld('qdmaPaRamBase', $$PAdr('xdc_Ptr', 'Pv'), 0x01C04000, 'wh');
        po.$$fld('maxPaRamEntries', $$T_CNum('(xdc_UInt)'), 128, 'wh');
        po.$$fld('paRamBaseIndex', $$T_CNum('(xdc_UInt)'), 96, 'wh');
        po.$$fld('numPaRamEntries', $$T_CNum('(xdc_UInt)'), 32, 'wh');
        po.$$fld('maxQdmaChannels', $$T_CNum('(xdc_UInt)'), 8, 'wh');
        po.$$fld('numQdmaChannels', $$T_CNum('(xdc_UInt)'), 4, 'wh');
        po.$$fld('qdmaChannels', $$PArr($$T_CNum('(xdc_UInt)'), true), $$SO([0, 1, 2, 3]), 'wh');
        po.$$fld('tccAllocationMaskH', $$T_CNum('(xdc_UInt)'), 0xffffffff, 'wh');
        po.$$fld('tccAllocationMaskL', $$T_CNum('(xdc_UInt)'), 0x00000000, 'wh');
        po.$$fld('heapInternal', $$T_Str, "DMAN3_heapInternal_undefined", 'wh');
        po.$$fld('heapExternal', $$T_Str, "DMAN3_heapExternal_undefined", 'wh');
        po.$$fld('numTccGroup', $$PArr($$T_CNum('(xdc_Int)'), false, 20), $$SO([0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]), 'wh');
        po.$$fld('numPaRamGroup', $$PArr($$T_CNum('(xdc_Int)'), false, 20), $$SO([0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]), 'wh');
        po.$$fld('scratchAllocFxn', $$T_Str, null, 'wh');
        po.$$fld('scratchFreeFxn', $$T_Str, null, 'wh');
        po.$$fld('nullPaRamIndex', $$T_CNum('(xdc_UInt)'), 0, 'wh');
        po.$$fld('idma3Internal', $$T_Bool, true, 'wh');
        po.$$fld('debug', $$T_Bool, false, 'wh');

/* ======== SINGLETONS ======== */

// package ti.sdo.fc.dman3
    var po = $om['ti.sdo.fc.dman3.Package'].$$init('ti.sdo.fc.dman3.Package', $om['xdc.IPackage.Module']);
    var cap = $om.$$bind('xdc.IPackage$$capsule', xdc.loadCapsule('../package.xs'));
        if (cap['init']) po.$$fxn('init', $om['xdc.IPackage$$init'], cap['init']);
        if (cap['close']) po.$$fxn('close', $om['xdc.IPackage$$close'], cap['close']);
        if (cap['validate']) po.$$fxn('validate', $om['xdc.IPackage$$validate'], cap['validate']);
        if (cap['exit']) po.$$fxn('exit', $om['xdc.IPackage$$exit'], cap['exit']);
        if (cap['getLibs']) po.$$fxn('getLibs', $om['xdc.IPackage$$getLibs'], cap['getLibs']);
        if (cap['getSects']) po.$$fxn('getSects', $om['xdc.IPackage$$getSects'], cap['getSects']);
    var pkg = $om['ti.sdo.fc.dman3'].$$init(po, 'ti.sdo.fc.dman3', $$DEFAULT, false);
    $om.$packages.$add(pkg);
    pkg.$$bind('$name', 'ti.sdo.fc.dman3');
    pkg.$$bind('$category', 'Package');
    pkg.$$bind('$$qn', 'ti.sdo.fc.dman3.');
    pkg.$$bind('$vers', [1, 0, 1]);
    pkg.$attr.$seal('length');
    pkg.$$bind('$imports', [
    ]);
    var $PKG = pkg;
    pkg.build.libraries = [
        'dman3cpud.a64P',
        'dman3d.a64P',
        'dman3Cfgd.a64P',
        'dman3cpu.a64P',
        'dman3Cfg.a64P',
        'dman3.a64P',
    ];
// module DMAN3
    var vo = $om['ti.sdo.fc.dman3.DMAN3'];
    var po = $om['ti.sdo.fc.dman3.DMAN3.Module'];
    vo.$$init(po, 'ti.sdo.fc.dman3.DMAN3', $$DEFAULT, false);
    vo.$$bind('Module', po);
    vo.$$bind('$category', 'Module');
    vo.$$bind('$package', $om['ti.sdo.fc.dman3']);
    vo.$$bind('$$tdefs', []);
    vo.$$bind('$$proxies', []);
    vo.$$bind('MAXDMARECS', $om['ti.sdo.fc.dman3.DMAN3.MAXDMARECS']);
    vo.$$bind('MAXGROUPS', $om['ti.sdo.fc.dman3.DMAN3.MAXGROUPS']);
    vo.$$bind('$private', {});
    pkg.$modules.$add(vo);
    $om.$modules.$add(vo);
    vo.$$bind('$$instflag', 0);
    vo.$$bind('$$iobjflag', 1);
    vo.$$bind('$$sizeflag', 1);
    vo.$$bind('$$dlgflag', 0);
    vo.$$bind('$$iflag', 0);
    if ('Module__Object' in vo) vo.$$bind('$object', new vo.Module__Object);
    vo.$$bind('$$meta_iobj', 0 + ('ti.sdo.fc.dman3.DMAN3$$instance$static$init' in $om));
    vo.$$bind('$$fxntab', []);
    vo.$attr['@Template'] = "./DMAN3.xdt";
    vo.$attr.$seal('length');
    vo.$$bind('TEMPLATE$', './DMAN3.xdt');
    pkg.$$bind('DMAN3', vo);
    pkg.$unitNames.$add('DMAN3');

/* ======== INITIALIZATION ======== */

    $om['ti.sdo.fc.dman3.DMAN3'].module$meta$init();
    pkg.init();
    $om['ti.sdo.fc.dman3.DMAN3'].$$bless();
