#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = /data/omapts/linux/dsp-tc/BIOS-5.33.02/bios_5_33_02/packages;/data/omapts/linux/dsp-tc/framework_components_1_10_04/fctools/packages;/vobs/SDS/Source/Bridge/dsp/bdsptools/packages;/vobs/SDS/Source/Bridge/dsp/bdsptools/framework_components_1_10_04/packages-bld
override XDCROOT = /data/omapts/linux/dsp-tc/BIOS-5.33.02/bios_5_33_02/xdctools
override XDCBUILDCFG = /vobs/SDS/Source/Bridge/dsp/config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = DEPOT=/data/omapts/linux/dsp-tc CGT55_VER=3.2.2 CGT6X_VER=6.0.7 TESTBUILDTYPE=ALL IPC= FC_DIR=/vobs/SDS/Source/Bridge/dsp/bdsptools/framework_components_1_10_04/packages-bld FC_PACKAGE=TRUE L_MONITOR=TRUE SCALING=TRUE DBS_CGTOOLS_DIR_C64=
override XDCTARGETS = C64P
endif
#
ifeq (0,1)
PKGPATH = /data/omapts/linux/dsp-tc/BIOS-5.33.02/bios_5_33_02/packages;/data/omapts/linux/dsp-tc/framework_components_1_10_04/fctools/packages;/vobs/SDS/Source/Bridge/dsp/bdsptools/packages;/vobs/SDS/Source/Bridge/dsp/bdsptools/framework_components_1_10_04/packages-bld;/data/omapts/linux/dsp-tc/BIOS-5.33.02/bios_5_33_02/xdctools/packages;../../../..
HOSTOS = Linux
endif
