xdc.loadCapsule('xdc/om2.xs');

var $om = xdc.om;


/* ======== IMPORTS ======== */

    xdc.loadPackage('ti.sdo.fc.dskt2');
    xdc.loadPackage('ti.sdo.fc.dman3');
    xdc.loadPackage('ti.sdo.fc.acpy3');
    xdc.loadPackage('ti.sdo.fc.utils');
    xdc.loadPackage('ti.sdo.fc.utils.api');
    xdc.loadPackage('ti.sdo.fc.acpy2');
    xdc.loadPackage('ti.sdo.fc.dman');
    xdc.loadPackage('ti.sdo.fc.dman3.examples');
    xdc.loadPackage('xdc');

/* ======== OBJECTS ======== */

// package framework_components_1_10_04
    var po = $om.$$bind('framework_components_1_10_04.Package', $$PObj());
    $om.$$bind('framework_components_1_10_04', $$VObj('framework_components_1_10_04', po));

/* ======== CONSTS ======== */


/* ======== CREATES ======== */


/* ======== FUNCTIONS ======== */


/* ======== SIZES ======== */


/* ======== TYPES ======== */


/* ======== SINGLETONS ======== */

// package framework_components_1_10_04
    var po = $om['framework_components_1_10_04.Package'].$$init('framework_components_1_10_04.Package', $om['xdc.IPackage.Module']);
    var pkg = $om['framework_components_1_10_04'].$$init(po, 'framework_components_1_10_04', $$DEFAULT, false);
    $om.$packages.$add(pkg);
    pkg.$$bind('$name', 'framework_components_1_10_04');
    pkg.$$bind('$category', 'Package');
    pkg.$$bind('$$qn', 'framework_components_1_10_04.');
    pkg.$$bind('$vers', []);
    pkg.$attr.$seal('length');
    pkg.$$bind('$imports', [
        ['ti.sdo.fc.dskt2', []],
        ['ti.sdo.fc.dman3', []],
        ['ti.sdo.fc.acpy3', []],
        ['ti.sdo.fc.utils', []],
        ['ti.sdo.fc.utils.api', []],
        ['ti.sdo.fc.acpy2', []],
        ['ti.sdo.fc.dman', []],
        ['ti.sdo.fc.dman3.examples', []],
    ]);
    var $PKG = pkg;
    pkg.build.libraries = [
    ];

/* ======== INITIALIZATION ======== */

    pkg.init();
