#!/bin/sh
# Full-stop the dev Toon's local data publishers so boxtalk_mirror's injected
# (mirrored) data is the only source the stock qt-gui sees. Reversible:
#   boxtalk_mirror_mute.sh          -> comment the rows + kill the daemons
#   boxtalk_mirror_mute.sh --undo   -> restore them
#
# We stop ONLY the data publishers we replace. We KEEP hcb_comm (the bus),
# hcb_config, lighttpd, etc. — qt-gui still needs those.
set -u
ROWS="ther sens p1p1 hvac"     # happ_thermstat hdrv_sensory hdrv_p1 happ_hvac
IT=/etc/inittab
if [ "${1:-}" = "--undo" ]; then
    for r in $ROWS; do sed -i "s/^#MIRROR#${r}:/${r}:/" "$IT"; done
    telinit q 2>/dev/null || kill -HUP 1 2>/dev/null || true
    echo "restored: $ROWS"
    exit 0
fi
for r in $ROWS; do sed -i "s/^${r}:/#MIRROR#${r}:/" "$IT"; done
telinit q 2>/dev/null || kill -HUP 1 2>/dev/null || true
pkill -x happ_thermstat 2>/dev/null; pkill -x hdrv_sensory 2>/dev/null
pkill -x hdrv_p1 2>/dev/null; pkill -x happ_hvac 2>/dev/null
echo "muted local publishers ($ROWS) — boxtalk_mirror is now the data source"
