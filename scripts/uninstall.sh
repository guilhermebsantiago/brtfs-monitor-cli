#!/usr/bin/env sh
set -eu

PREFIX=${PREFIX:-/usr/local}
BINDIR=${BINDIR:-"$PREFIX/bin"}
SYSCONFDIR=${SYSCONFDIR:-/etc}

echo "Removing $BINDIR/btrfs-monitor (requires root)"
rm -f "$BINDIR/btrfs-monitor"
echo "Config in $SYSCONFDIR/btrfs-monitor/config preserved. Remove manually if undesired."


