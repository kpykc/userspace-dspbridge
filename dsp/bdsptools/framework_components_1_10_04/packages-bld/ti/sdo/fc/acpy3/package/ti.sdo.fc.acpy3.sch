xdc.loadCapsule('xdc/om2.xs');

var $om = xdc.om;


/* ======== IMPORTS ======== */

    xdc.loadPackage('xdc');

/* ======== OBJECTS ======== */

// package ti.sdo.fc.acpy3
    var po = $om.$$bind('ti.sdo.fc.acpy3.Package', $$PObj());
    $om.$$bind('ti.sdo.fc.acpy3', $$VObj('ti.sdo.fc.acpy3', po));
// module ACPY3
    var po = $om.$$bind('ti.sdo.fc.acpy3.ACPY3.Module', $$PObj());
    $om.$$bind('ti.sdo.fc.acpy3.ACPY3', $$VObj('ti.sdo.fc.acpy3.ACPY3', po));

/* ======== CONSTS ======== */

// module ACPY3

/* ======== CREATES ======== */

// module ACPY3

/* ======== FUNCTIONS ======== */


/* ======== SIZES ======== */


/* ======== TYPES ======== */

// module ACPY3
    var po = $om['ti.sdo.fc.acpy3.ACPY3.Module'].$$init('ti.sdo.fc.acpy3.ACPY3.Module', $$Module);
        po.$$fld('$hostonly', $$T_Num, 1, 'r');
        po.$$fld('debug', $$T_Bool, false, 'wh');

/* ======== SINGLETONS ======== */

// package ti.sdo.fc.acpy3
    var po = $om['ti.sdo.fc.acpy3.Package'].$$init('ti.sdo.fc.acpy3.Package', $om['xdc.IPackage.Module']);
    var cap = $om.$$bind('xdc.IPackage$$capsule', xdc.loadCapsule('../package.xs'));
        if (cap['init']) po.$$fxn('init', $om['xdc.IPackage$$init'], cap['init']);
        if (cap['close']) po.$$fxn('close', $om['xdc.IPackage$$close'], cap['close']);
        if (cap['validate']) po.$$fxn('validate', $om['xdc.IPackage$$validate'], cap['validate']);
        if (cap['exit']) po.$$fxn('exit', $om['xdc.IPackage$$exit'], cap['exit']);
        if (cap['getLibs']) po.$$fxn('getLibs', $om['xdc.IPackage$$getLibs'], cap['getLibs']);
        if (cap['getSects']) po.$$fxn('getSects', $om['xdc.IPackage$$getSects'], cap['getSects']);
    var pkg = $om['ti.sdo.fc.acpy3'].$$init(po, 'ti.sdo.fc.acpy3', $$DEFAULT, false);
    $om.$packages.$add(pkg);
    pkg.$$bind('$name', 'ti.sdo.fc.acpy3');
    pkg.$$bind('$category', 'Package');
    pkg.$$bind('$$qn', 'ti.sdo.fc.acpy3.');
    pkg.$$bind('$vers', [1, 0, 0]);
    pkg.$attr.$seal('length');
    pkg.$$bind('$imports', [
    ]);
    var $PKG = pkg;
    pkg.build.libraries = [
        'acpy3cpud.a64P',
        'acpy3d.a64P',
        'acpy3.a64P',
        'acpy3cpu.a64P',
    ];
// module ACPY3
    var vo = $om['ti.sdo.fc.acpy3.ACPY3'];
    var po = $om['ti.sdo.fc.acpy3.ACPY3.Module'];
    vo.$$init(po, 'ti.sdo.fc.acpy3.ACPY3', $$DEFAULT, false);
    vo.$$bind('Module', po);
    vo.$$bind('$category', 'Module');
    vo.$$bind('$package', $om['ti.sdo.fc.acpy3']);
    vo.$$bind('$$tdefs', []);
    vo.$$bind('$$proxies', []);
    vo.$$bind('$private', {});
    pkg.$modules.$add(vo);
    $om.$modules.$add(vo);
    vo.$$bind('$$instflag', 0);
    vo.$$bind('$$iobjflag', 1);
    vo.$$bind('$$sizeflag', 1);
    vo.$$bind('$$dlgflag', 0);
    vo.$$bind('$$iflag', 0);
    if ('Module__Object' in vo) vo.$$bind('$object', new vo.Module__Object);
    vo.$$bind('$$meta_iobj', 0 + ('ti.sdo.fc.acpy3.ACPY3$$instance$static$init' in $om));
    vo.$$bind('$$fxntab', []);
    vo.$attr.$seal('length');
    pkg.$$bind('ACPY3', vo);
    pkg.$unitNames.$add('ACPY3');

/* ======== INITIALIZATION ======== */

    $om['ti.sdo.fc.acpy3.ACPY3'].module$meta$init();
    pkg.init();
    $om['ti.sdo.fc.acpy3.ACPY3'].$$bless();
