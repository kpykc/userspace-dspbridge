xdc.loadCapsule('xdc/om2.xs');

var $om = xdc.om;


/* ======== IMPORTS ======== */

    xdc.loadPackage('xdc');

/* ======== OBJECTS ======== */

// package ti.sdo.fc.acpy2
    var po = $om.$$bind('ti.sdo.fc.acpy2.Package', $$PObj());
    $om.$$bind('ti.sdo.fc.acpy2', $$VObj('ti.sdo.fc.acpy2', po));

/* ======== CONSTS ======== */


/* ======== CREATES ======== */


/* ======== FUNCTIONS ======== */


/* ======== SIZES ======== */


/* ======== TYPES ======== */


/* ======== SINGLETONS ======== */

// package ti.sdo.fc.acpy2
    var po = $om['ti.sdo.fc.acpy2.Package'].$$init('ti.sdo.fc.acpy2.Package', $om['xdc.IPackage.Module']);
    var pkg = $om['ti.sdo.fc.acpy2'].$$init(po, 'ti.sdo.fc.acpy2', $$DEFAULT, false);
    $om.$packages.$add(pkg);
    pkg.$$bind('$name', 'ti.sdo.fc.acpy2');
    pkg.$$bind('$category', 'Package');
    pkg.$$bind('$$qn', 'ti.sdo.fc.acpy2.');
    pkg.$$bind('$vers', []);
    pkg.$attr.$seal('length');
    pkg.$$bind('$imports', [
    ]);
    var $PKG = pkg;
    pkg.build.libraries = [
        'acpy2_mgs3.a55L',
        'acpy2_mgs3_rtsd.a55L',
        'acpy2_mgs3_rts.a55L',
        'acpy2_dma4.a55L',
        'acpy2_mgs3d.a55L',
        'acpy2_dma4_rtsd.a55L',
        'acpy2_dma4_rts.a55L',
        'acpy2_dma4d.a55L',
    ];

/* ======== INITIALIZATION ======== */

    pkg.init();
