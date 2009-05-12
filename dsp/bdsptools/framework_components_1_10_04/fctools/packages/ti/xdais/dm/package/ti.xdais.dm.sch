xdc.loadCapsule('xdc/om2.xs');

var $om = xdc.om;


/* ======== IMPORTS ======== */

    xdc.loadPackage('xdc');

/* ======== OBJECTS ======== */

// package ti.xdais.dm
    var po = $om.$$bind('ti.xdais.dm.Package', $$PObj());
    $om.$$bind('ti.xdais.dm', $$VObj('ti.xdais.dm', po));

/* ======== CONSTS ======== */


/* ======== CREATES ======== */


/* ======== FUNCTIONS ======== */


/* ======== SIZES ======== */


/* ======== TYPES ======== */


/* ======== SINGLETONS ======== */

// package ti.xdais.dm
    var po = $om['ti.xdais.dm.Package'].$$init('ti.xdais.dm.Package', $om['xdc.IPackage.Module']);
    var pkg = $om['ti.xdais.dm'].$$init(po, 'ti.xdais.dm', $$DEFAULT, false);
    $om.$packages.$add(pkg);
    pkg.$$bind('$name', 'ti.xdais.dm');
    pkg.$$bind('$category', 'Package');
    pkg.$$bind('$$qn', 'ti.xdais.dm.');
    pkg.$$bind('$vers', [1, 0, 1, 0]);
    pkg.$attr.$seal('length');
    pkg.$$bind('$imports', [
    ]);
    var $PKG = pkg;
    pkg.build.libraries = [
    ];

/* ======== INITIALIZATION ======== */

    pkg.init();
