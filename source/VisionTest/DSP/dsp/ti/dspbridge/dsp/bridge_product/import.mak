#
#! ============================================================
#! Copyright 2004 by Texas Instruments Incorporated.
#! All rights reserved. Property of Texas Instruments Incorporated.
#! Restricted rights to use, duplicate or disclose this code are
#! granted through contract.
#! ============================================================
#
#
#  ======== import.mak ========
#

all: .all-local

.all-local: import.mak
	$(MKDIR) -p exports/include
	$(MKDIR) -p exports/lib
	$(RM) exports/include/*
	$(RM) exports/lib/*
	@$(MSG) importing bridge files ...
	$(CP) -f ../bridge/inc/dbc.h ./exports/include/.
	$(CP) -f ../bridge/inc/dbg.h ./exports/include/.
	$(CP) -f ../bridge/msg/msg.h ./exports/include/.
	$(CP) -f ../bridge/msg/msg_sh.h ./exports/include/.
	$(CP) -f ../bridge/node/node.h ./exports/include/.
	$(CP) -f ../bridge/rms/rms.h ./exports/include/.
	$(CP) -f ../bridge/rms/rmsdefs.h ./exports/include/.
	$(CP) -f ../bridge/rms/rms_sh.h ./exports/include/.
	$(CP) -f ../bridge/rms/rmstypes.h ./exports/include/.
	$(CP) -f ../bridge/rms/icache.h ./exports/include/.
	$(CP) -f ../bridge/usr/usr.h ./exports/include/.
	$(CP) -f ../bridge/strm/strm.h ./exports/include/.
	$(CP) -f ../ldrv/newdev/ddma.h ./exports/include/.
	$(CP) -f ../bpwr/bpwr.h ./exports/include/.
	$(CP) -f ../inst2/inst2.h ./exports/include/.
	$(CP) -f ../inst2/_inst2.h ./exports/include/.

ifneq (,$(findstring C55_large,$(TARGETS)))
	$(CP) -f ../bridge/bridge.a55L ./exports/lib/.
	$(CP) -f ../bridge/bridged.a55L ./exports/lib/.
	$(CP) -f ../bridge/bridgev1035.a55L ./exports/lib/.
	$(CP) -f ../bridge/bridgev1035d.a55L ./exports/lib/.
	$(CP) -f ../ldrv/newdev/newdev1510.a55L ./exports/lib/.
	$(CP) -f ../ldrv/newdev/newdev1510d.a55L ./exports/lib/.
	$(CP) -f ../ldrv/newdev/newdev24xx.a55L ./exports/lib/.
	$(CP) -f ../ldrv/newdev/newdev24xxd.a55L ./exports/lib/.
	$(CP) -f ../ldrv/newdev/newdevv1035.a55L ./exports/lib/.
	$(CP) -f ../ldrv/newdev/newdevv1035d.a55L ./exports/lib/.
	$(CP) -f ../ldrv/tiomap/tiomap1510.a55L ./exports/lib/.
	$(CP) -f ../ldrv/tiomap/tiomap1510d.a55L ./exports/lib/.
	$(CP) -f ../ldrv/tiomap/tiomap24xx.a55L ./exports/lib/.
	$(CP) -f ../ldrv/tiomap/tiomap24xxd.a55L ./exports/lib/.
	$(CP) -f ../ldrv/tiomap/tiomapv1035d.a55L ./exports/lib/.
	$(CP) -f ../ldrv/tiomap/tiomapv1035.a55L ./exports/lib/.
	$(CP) -f ../qos/thrload.a55L ./exports/lib/.
	$(CP) -f ../qos/thrloadd.a55L ./exports/lib/.
ifeq ($(IPC), TRUE)
	$(CP) -f ../mipc/mipcv1035.a55L ./exports/lib/.
	$(CP) -f ../mipc/mipcv1035d.a55L ./exports/lib/.
endif
endif

ifneq (,$(findstring C64P,$(TARGETS)))
	$(CP) -f ../bridge/bridge.a64P ./exports/lib/.
	$(CP) -f ../bridge/bridged.a64P ./exports/lib/.
	$(CP) -f ../bpwr/bpwr.a64P ./exports/lib/.
	$(CP) -f ../bpwr/bpwrd.a64P ./exports/lib/.
	$(CP) -f ../bpwr/bpwr343x.a64P ./exports/lib/.
	$(CP) -f ../bpwr/bpwr343xd.a64P ./exports/lib/.
	$(CP) -f ../ldrv/newdev/newdev2430.a64P ./exports/lib/.
	$(CP) -f ../ldrv/newdev/newdev2430d.a64P ./exports/lib/.
	$(CP) -f ../ldrv/tiomap/tiomap24xx.a64P ./exports/lib/.
	$(CP) -f ../ldrv/tiomap/tiomap24xxd.a64P ./exports/lib/.
	$(CP) -f ../qos/thrload.a64P ./exports/lib/.
	$(CP) -f ../qos/thrloadd.a64P ./exports/lib/.
	$(CP) -f ../inst2/inst2.a64P ./exports/lib/.
	$(CP) -f ../inst2/inst2d.a64P ./exports/lib/.

endif

	@$(MSG) import compleleted ...

clean::
	$(RM) exports/include/*
	$(RM) exports/lib/*
