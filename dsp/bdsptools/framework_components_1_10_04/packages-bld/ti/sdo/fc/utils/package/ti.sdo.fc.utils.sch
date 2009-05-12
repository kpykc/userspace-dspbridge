xdc.loadCapsule('xdc/om2.xs');

var $om = xdc.om;


/* ======== IMPORTS ======== */

    xdc.loadPackage('xdc');

/* ======== OBJECTS ======== */

// package ti.sdo.fc.utils
    var po = $om.$$bind('ti.sdo.fc.utils.Package', $$PObj());
    $om.$$bind('ti.sdo.fc.utils', $$VObj('ti.sdo.fc.utils', po));

/* ======== CONSTS ======== */


/* ======== CREATES ======== */


/* ======== FUNCTIONS ======== */


/* ======== SIZES ======== */


/* ======== TYPES ======== */


/* ======== SINGLETONS ======== */

// package ti.sdo.fc.utils
    var po = $om['ti.sdo.fc.utils.Package'].$$init('ti.sdo.fc.utils.Package', $om['xdc.IPackage.Module']);
    var pkg = $om['ti.sdo.fc.utils'].$$init(po, 'ti.sdo.fc.utils', $$DEFAULT, false);
    $om.$packages.$add(pkg);
    pkg.$$bind('$name', 'ti.sdo.fc.utils');
    pkg.$$bind('$category', 'Package');
    pkg.$$bind('$$qn', 'ti.sdo.fc.utils.');
    pkg.$$bind('$vers', [1, 0, 0]);
    pkg.$attr.$seal('length');
    pkg.$$bind('$imports', [
    ]);
    var $PKG = pkg;
    pkg.build.libraries = [
    ];

/* ======== INITIALIZATION ======== */

    pkg.init();
