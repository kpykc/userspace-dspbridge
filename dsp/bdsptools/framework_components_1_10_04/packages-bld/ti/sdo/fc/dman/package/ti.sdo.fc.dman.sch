xdc.loadCapsule('xdc/om2.xs');

var $om = xdc.om;


/* ======== IMPORTS ======== */

    xdc.loadPackage('xdc');

/* ======== OBJECTS ======== */

// package ti.sdo.fc.dman
    var po = $om.$$bind('ti.sdo.fc.dman.Package', $$PObj());
    $om.$$bind('ti.sdo.fc.dman', $$VObj('ti.sdo.fc.dman', po));

/* ======== CONSTS ======== */


/* ======== CREATES ======== */


/* ======== FUNCTIONS ======== */


/* ======== SIZES ======== */


/* ======== TYPES ======== */


/* ======== SINGLETONS ======== */

// package ti.sdo.fc.dman
    var po = $om['ti.sdo.fc.dman.Package'].$$init('ti.sdo.fc.dman.Package', $om['xdc.IPackage.Module']);
    var pkg = $om['ti.sdo.fc.dman'].$$init(po, 'ti.sdo.fc.dman', $$DEFAULT, false);
    $om.$packages.$add(pkg);
    pkg.$$bind('$name', 'ti.sdo.fc.dman');
    pkg.$$bind('$category', 'Package');
    pkg.$$bind('$$qn', 'ti.sdo.fc.dman.');
    pkg.$$bind('$vers', []);
    pkg.$attr.$seal('length');
    pkg.$$bind('$imports', [
    ]);
    var $PKG = pkg;
    pkg.build.libraries = [
        'dman_mgs3d.a55L',
        'dman_dma4d.a55L',
        'dman_mgs3.a55L',
        'dman_dma4.a55L',
    ];

/* ======== INITIALIZATION ======== */

    pkg.init();
