BIOS_530=/db/atree/library/trees/depot/BIOS-5.30.00.10/bios_5_30_00_10
BIOS_521=/db/atree/library/trees/depot/BIOS-5.21.01.10/bios_5_21_01
BIOS_510=/db/atree/library/trees/depot/BIOS-5.10.04.28/bios_5_10_04

BIOS=$(BIOS_530)

IMPORTPATH = $(BIOS)/packages;$(BIOS)/xdctools/packages;../../../../..;..

CHIP=2430

*.tc*: FORCE
	@echo $@
	$(BIOS)/xdctools/tconf -Dconfig.importPath="$(IMPORTPATH)" -Dconfig.chipType=$(CHIP) $@

FORCE:

clean:
	rm -f *.cdb
	rm -f *cfg*
