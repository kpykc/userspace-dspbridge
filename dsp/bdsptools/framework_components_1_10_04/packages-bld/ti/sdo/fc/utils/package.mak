#
#  Copyright 2007 by Texas Instruments Incorporated.
#  
#  All rights reserved. Property of Texas Instruments Incorporated.
#  Restricted rights to use, duplicate or disclose this code are
#  granted through contract.
#  
#

#
#  Do not edit this file.  This file is generated from 
#  package.bld.  Any modifications to this file will be 
#  overwritten whenever makefiles are re-generated.
#

unexport MAKEFILE_LIST
override PKGDIR = ti/sdo/fc/utils
override PKGROOT = ../../../..
XDCINCS = -I. -I$(strip $(subst ;, -I,$(subst $(space),\$(space),$(XPKGPATH))))
XDCCFGDIR = package/cfg/

#
# The following dependencies ensure package.mak is rebuilt
# in the event that some included BOM script changes.
#
ifndef XDCBOOT
ifneq (clean,$(MAKECMDGOALS))
/db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/packages/xdc/bld/package/xdc.bld.sch:
package.mak: /db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/packages/xdc/bld/package/xdc.bld.sch
/db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/packages/xdc/bld/package.xs:
package.mak: /db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/packages/xdc/bld/package.xs
/db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/packages/xdc/bld/Executable.xs:
package.mak: /db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/packages/xdc/bld/Executable.xs
package.mak: package.bld
/db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/packages/xdc/bld/Library.xs:
package.mak: /db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/packages/xdc/bld/Library.xs
/db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/packages/xdc/package.xs:
package.mak: /db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/packages/xdc/package.xs
/db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/packages/ti/targets/package/ti.targets.sch:
package.mak: /db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/packages/ti/targets/package/ti.targets.sch
/db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/packages/xdc/bld/Repository.xs:
package.mak: /db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/packages/xdc/bld/Repository.xs
/db/atree/library/trees/fc-b12x/src/config.bld:
package.mak: /db/atree/library/trees/fc-b12x/src/config.bld
/db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/packages/xdc/bld/ITarget.xs:
package.mak: /db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/packages/xdc/bld/ITarget.xs
/db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/packages/ti/targets/package.xs:
package.mak: /db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/packages/ti/targets/package.xs
/db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/packages/xdc/bld/Utils.xs:
package.mak: /db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/packages/xdc/bld/Utils.xs
/db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/packages/xdc/IPackage.xs:
package.mak: /db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/packages/xdc/IPackage.xs
/db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/packages/xdc/bld/BuildEnvironment.xs:
package.mak: /db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/packages/xdc/bld/BuildEnvironment.xs
/db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/packages/xdc/xdc.tci:
package.mak: /db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/packages/xdc/xdc.tci
/db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/include/utils.tci:
package.mak: /db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/include/utils.tci
/db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/packages/xdc/bld/PackageContents.xs:
package.mak: /db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/packages/xdc/bld/PackageContents.xs
/db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/packages/ti/targets/ITarget.xs:
package.mak: /db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/packages/ti/targets/ITarget.xs
/db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/packages/xdc/bld/_gen.xs:
package.mak: /db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/packages/xdc/bld/_gen.xs
/db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/packages/xdc/package/xdc.sch:
package.mak: /db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/packages/xdc/package/xdc.sch
/db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/packages/xdc/om2.xs:
package.mak: /db/toolsrc/library/vendors2005/xdc/xdcdist-d36/Linux/packages/xdc/om2.xs
endif
endif

all,64P .libraries,64P .dlls,64P .executables,64P test,64P:;
all,55L .libraries,55L .dlls,55L .executables,55L test,55L:;

all: .executables 
.executables: .libraries .dlls
.libraries: .interfaces

PKGCFGS := $(wildcard package.xs) package/build.cfg
.interfaces: package/package.xdc.xml package/package.defs.h package.xdc $(PKGCFGS)

-include package/package.xdc.dep
package/%.xdc.xml package/%.xdc.inc package/%.c package/%.defs.h: %.xdc $(PKGCFGS) $(ILIADJAR)
	@$(MSG) generating interfaces for package ti.sdo.fc.utils" (because $@ is older than $(firstword $?))" ...
	$(JAVA) $(JOPTS) Iliad.CMD.Build $(MK_CDLOPTS) -m package/package.xdc.dep -i package/package.xdc.inc package.xdc

ifneq (clean,$(MAKECMDGOALS))
-include package/package.cfg.dep
endif

package/package.ext.xml: package/package.cfg.inc
package/package.cfg.inc: $(XDCROOT)/packages/xdc/cfg/cfginc.js package.xdc
	@$(MSG) generating schema include file list ...
	$(CONFIG) $(XDCROOT)/packages/xdc/cfg/cfginc.js ti.sdo.fc.utils $@

test:;
%,copy:
	@$(if $<,,$(MSG) don\'t know how to build $*; exit 1)
	@$(MSG) cp $< $@
	$(RM) $@
	$(CP) $< $@

$(XDCCFGDIR)%.c $(XDCCFGDIR)%.h $(XDCCFGDIR)%.xdl: $(XDCCFGDIR)%.cfg .interfaces $(XDCROOT)/packages/xdc/cfg/cfg.js
	@$(MSG) "configuring $(_PROG_NAME) from $< ..."
	$(CONFIG) $(_PROG_TCOPTS) $(XDCROOT)/packages/xdc/cfg/cfg.js $(XDCCFGDIR)$* $(_PROG_NAME)

.PHONY: release,ti_sdo_fc_utils
ti_sdo_fc_utils.tar: package/package.xdc.xml
ti_sdo_fc_utils.tar: package/package.cfg.inc
ti_sdo_fc_utils.tar: package/package.ext.xml
ti_sdo_fc_utils.tar: package/package.xdc.inc
ti_sdo_fc_utils.tar: package/package.bld.xml
ti_sdo_fc_utils.tar: package/package.rel.dot
ifneq (clean,$(MAKECMDGOALS))
-include package/rel/ti_sdo_fc_utils.tar.dep
-include package/rel/ti_sdo_fc_utils.tar.rcl.dep
endif
package/rel/ti_sdo_fc_utils/ti/sdo/fc/utils/package/package.rel.xml:

ti_sdo_fc_utils.tar: package/rel/ti_sdo_fc_utils.inc package/rel/ti_sdo_fc_utils/ti/sdo/fc/utils/package/package.rel.xml
	@$(MSG) making release file $@ "(because of $(firstword $?))" ...
	-$(RM) $@
	$(call MKREL,package/rel/ti_sdo_fc_utils.inc,package/rel/ti_sdo_fc_utils.tar.dep, tisb/release/filters/copyright.rel)


release release,ti_sdo_fc_utils: all ti_sdo_fc_utils.tar
clean:: .clean
	-$(RM) ti_sdo_fc_utils.tar
	-$(RM) package/rel/ti_sdo_fc_utils.inc
	-$(RM) package/rel/ti_sdo_fc_utils.tar.dep

clean:: .clean
	-$(RM) .libraries .libraries,*
clean:: 
	-$(RM) .dlls .dlls,*
ifneq (clean,$(MAKECMDGOALS))
ifeq (,$(wildcard package))
    $(shell $(MKDIR) package)
endif
ifeq (,$(wildcard package/external))
    $(shell $(MKDIR) package/external)
endif
ifeq (,$(wildcard package/lib))
    $(shell $(MKDIR) package/lib)
endif
ifeq (,$(wildcard package/rel))
    $(shell $(MKDIR) package/rel)
endif
ifeq (,$(wildcard package/cfg))
    $(shell $(MKDIR) package/cfg)
endif
ifeq (,$(wildcard package/internal))
    $(shell $(MKDIR) package/internal)
endif
endif
clean::
	-$(RMDIR) package


#
#  @(#) ti.sdo.fc.utils; 1, 0, 0,58; 5-10-2007 10:50:25; /db/atree/library/trees/fc-b12x/src/
#

