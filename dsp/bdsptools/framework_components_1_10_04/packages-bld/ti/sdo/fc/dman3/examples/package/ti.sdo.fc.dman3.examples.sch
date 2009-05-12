xdc.loadCapsule('xdc/om2.xs');

var $om = xdc.om;


/* ======== IMPORTS ======== */

    xdc.loadPackage('ti.bios');
    xdc.loadPackage('ti.sdo.fc.dman3');
    xdc.loadPackage('ti.sdo.fc.acpy3');
    xdc.loadPackage('ti.sdo.fc.utils');
    xdc.loadPackage('ti.sdo.fc.utils.api');
    xdc.loadPackage('xdc');

/* ======== OBJECTS ======== */

// package ti.sdo.fc.dman3.examples
    var po = $om.$$bind('ti.sdo.fc.dman3.examples.Package', $$PObj());
    $om.$$bind('ti.sdo.fc.dman3.examples', $$VObj('ti.sdo.fc.dman3.examples', po));

/* ======== CONSTS ======== */


/* ======== CREATES ======== */


/* ======== FUNCTIONS ======== */


/* ======== SIZES ======== */


/* ======== TYPES ======== */


/* ======== SINGLETONS ======== */

// package ti.sdo.fc.dman3.examples
    var po = $om['ti.sdo.fc.dman3.examples.Package'].$$init('ti.sdo.fc.dman3.examples.Package', $om['xdc.IPackage.Module']);
    var cap = $om.$$bind('xdc.IPackage$$capsule', xdc.loadCapsule('../package.xs'));
        if (cap['init']) po.$$fxn('init', $om['xdc.IPackage$$init'], cap['init']);
        if (cap['close']) po.$$fxn('close', $om['xdc.IPackage$$close'], cap['close']);
        if (cap['validate']) po.$$fxn('validate', $om['xdc.IPackage$$validate'], cap['validate']);
        if (cap['exit']) po.$$fxn('exit', $om['xdc.IPackage$$exit'], cap['exit']);
        if (cap['getLibs']) po.$$fxn('getLibs', $om['xdc.IPackage$$getLibs'], cap['getLibs']);
        if (cap['getSects']) po.$$fxn('getSects', $om['xdc.IPackage$$getSects'], cap['getSects']);
    var pkg = $om['ti.sdo.fc.dman3.examples'].$$init(po, 'ti.sdo.fc.dman3.examples', $$DEFAULT, false);
    $om.$packages.$add(pkg);
    pkg.$$bind('$name', 'ti.sdo.fc.dman3.examples');
    pkg.$$bind('$category', 'Package');
    pkg.$$bind('$$qn', 'ti.sdo.fc.dman3.examples.');
    pkg.$$bind('$vers', []);
    pkg.$attr.$seal('length');
    pkg.$$bind('$imports', [
        ['ti.bios', []],
        ['ti.sdo.fc.dman3', []],
        ['ti.sdo.fc.acpy3', []],
        ['ti.sdo.fc.utils', []],
        ['ti.sdo.fc.utils.api', []],
    ]);
    var $PKG = pkg;
    pkg.build.libraries = [
        'fcpy_ti.a64P',
    ];

/* ======== INITIALIZATION ======== */

    pkg.init();
