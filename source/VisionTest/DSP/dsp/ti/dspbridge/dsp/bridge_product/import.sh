#
# ======== import.sh ========
#

echo importing bridge files ...

cp -f ../bridge/dbp/dbp.h55 ./exports/include/.
cp -f ../bridge/inc/dbc.h ./exports/include/.
cp -f ../bridge/inc/dbg.h ./exports/include/.
cp -f ../bridge/msg/msg.h ./exports/include/.
cp -f ../bridge/msg/msg_sh.h ./exports/include/.
cp -f ../bridge/node/node.h ./exports/include/.
cp -f ../bridge/pwr/bpwr.h ./exports/include/.
cp -f ../bridge/rms/rms.h ./exports/include/.
cp -f ../bridge/rms/rmsdefs.h ./exports/include/.
cp -f ../bridge/rms/rms_sh.h ./exports/include/.
cp -f ../bridge/rms/rmstypes.h ./exports/include/.
cp -f ../bridge/rms/icache.h ./exports/include/.
cp -f ../bridge/strm/strm.h ./exports/include/.
cp -f ../dais/acpy/acpy2_omap.h ./exports/include/.
cp -f ../dais/dman/dman.h ./exports/include/.
cp -f ../dais/dman24xx/dman24xx.h ./exports/include/.
cp -f ../dais/dman24xx/_dman24xx.h ./exports/include/.
cp -f ../dais/dskt2/dskt.h ./exports/include/.
cp -f ../dais/dskt2/dskt2.h ./exports/include/.
cp -f ../ldrv/newdev/ddma.h ./exports/include/.
cp -f ../ldrv/newdev/dsm.h55 ./exports/include/.
cp -f ../ldrv/tiomap/tiomap1510shm.h55 ./exports/include/.

cp -f ../bridge/bridge.a55L ./exports/lib/.
cp -f ../bridge/bridged.a55L ./exports/lib/.
cp -f ../bridge/bpwr.a55L ./exports/lib/.
cp -f ../bridge/bpwrd.a55L ./exports/lib/.
cp -f ../dais/acpy/acpy.a55L ./exports/lib/.
cp -f ../dais/acpy/acpyd.a55L ./exports/lib/.
cp -f ../dais/acpy24xx/acpy24xx.a55L ./exports/lib/.
cp -f ../dais/acpy24xx/acpy24xxd.a55L ./exports/lib/.
cp -f ../dais/dman/dman.a55L ./exports/lib/.
cp -f ../dais/dman/dmand.a55L ./exports/lib/.
cp -f ../dais/dman24xx/dman24xx.a55L ./exports/lib/.
cp -f ../dais/dman24xx/dman24xxd.a55L ./exports/lib/.
cp -f ../dais/dskt2/dskt.a55L ./exports/lib/.
cp -f ../dais/dskt2/dsktd.a55L ./exports/lib/.
cp -f ../ldrv/newdev/newdev.a55L ./exports/lib/.
cp -f ../ldrv/newdev/newdevd.a55L ./exports/lib/.
cp -f ../ldrv/tiomap/tiomap1510.a55L ./exports/lib/.
cp -f ../ldrv/tiomap/tiomap1510d.a55L ./exports/lib/.
cp -f ../ldrv/tiomap/tiomap24xx.a55L ./exports/lib/.
cp -f ../ldrv/tiomap/tiomap24xxd.a55L ./exports/lib/.
cp -f ../qos/thrload.a55L ./exports/lib/.
cp -f ../qos/thrloadd.a55L ./exports/lib/.

## RCL script replaced with cp calls.
echo import compleleted ...
